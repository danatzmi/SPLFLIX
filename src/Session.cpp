#include "../include/Session.h"
#include "../include/User.h"
#include <iostream>
#include "../include/json.hpp"
#include "../include/Watchable.h"
#include <fstream>
#include <string>
#include <unordered_set>
using namespace std;
using json = nlohmann::json;


Session::Session(const string& configFilePath) : activeUser(), nextReco(nullptr), commandsSet({}), input({}), content({}), actionsLog({}), userMap({}) {

    commandsSet.insert("createuser");
    commandsSet.insert("changeuser");
    commandsSet.insert("deleteuser");
    commandsSet.insert("dupuser");
    commandsSet.insert("content");
    commandsSet.insert("watchhist");
    commandsSet.insert("watch");
    commandsSet.insert("log");
    commandsSet.insert("exit");
    commandsSet.insert("y");
    commandsSet.insert("n");

    activeUser = new LengthRecommenderUser("default");
    userMap.insert({"default", activeUser});

    json j;
    std::ifstream ifs(configFilePath);
    ifs >> j;
    ifs.close();
    long id = 0;
    string name;
    int length;
    vector<string> tags;
    for (auto movie : j["movies"]) {
        name = movie["name"];
        length = movie["length"];
        for (auto& tag : movie["tags"]) {
            tags.push_back(tag);
        }
        content.push_back(new Movie(id, name, length, tags));
        id++;
        tags.clear();
    }
    for (auto tv_show : j["tv_series"]) {
        name = tv_show["name"];
        length = tv_show["episode_length"];
        for (auto &tag : tv_show["tags"]) {
            tags.push_back(tag);
        }

        int seasonNumber = 1;
        Episode *curr;
        for (auto &season : tv_show["seasons"]) {
            int numberOfEpisodes = (int) season;
            for (int episodeNumber = 1; episodeNumber <= numberOfEpisodes; episodeNumber++) {
                curr = new Episode(id, name, length, seasonNumber, episodeNumber, tags);
                content.push_back(curr);
                id++;
            }
            seasonNumber++;
        }
        curr->setNextEpisodeId(-1);
    }
}

Session::Session(const Session& other) : activeUser(), nextReco(nullptr), commandsSet({}), input({}), content({}), actionsLog({}), userMap({}) {
    if(other.nextReco != nullptr) {
        nextReco = other.nextReco;
    }
    for(std::string command : other.commandsSet) {
        commandsSet.insert(command);
    }
    for(std::string str : other.input) {
        input.push_back(str);
    }
    for(int i = 0; i < (int)other.content.size(); i++) {
        this->content.push_back(other.content[i]->clone());
    }
    for(int i = 0; i < (int)other.actionsLog.size(); i++) {
        this->actionsLog.push_back(other.actionsLog[i]->clone());
    }
    for(const auto& user : other.userMap) {
        if (user.first == other.activeUser->getName()){
            activeUser = other.activeUser->clone();
            this->userMap.insert({user.first, activeUser});
        }
        else {
            this->userMap.insert({user.first, user.second->clone()});
        }
    }
}

Session& Session::operator=(const Session& other) {
    if(this != &other) {
        activeUser = nullptr;
        nextReco = nullptr;
        for(Watchable* watchable : content) {
            delete(watchable);
            watchable = nullptr;
        }
        for(BaseAction* action : actionsLog) {
            delete(action);
            action = nullptr;
        }
        for(auto user : userMap) {
            delete(user.second);
            user.second = nullptr;
        }
        clear();
        if(other.nextReco != nullptr) {
            nextReco = other.nextReco;
        }
        for(std::string command : other.commandsSet) {
            commandsSet.insert(command);
        }
        for(std::string i : other.input) {
            input.push_back(string(i));
        }
        for(const auto& watchable : other.content) {
            content.push_back(watchable->clone());
        }
        for(const auto& action : other.actionsLog) {
            actionsLog.push_back(action->clone());
        }
        for(const auto& user : other.userMap) {
            userMap[user.first] = user.second->clone();
            if(user.first == other.activeUser->getName())
                activeUser = userMap[user.first];
        }
    }
    return *this;
}

Session::~Session() {
    for(int i = 0; i < (int)content.size(); i++)
        delete(content[i]);
    for(int i = 0; i < (int)actionsLog.size(); i++)
        delete (actionsLog[i]);
    for(std::pair<std::string, User*> user : userMap) {
        if (user.second != nullptr)
            delete (user.second);
    }
    commandsSet.clear();
    actionsLog.clear();
    content.clear();
    userMap.clear();
    input.clear();
}

Session::Session(Session&& other) : activeUser(), nextReco(nullptr), commandsSet({}), input({}), content({}), actionsLog({}), userMap({}) {
    activeUser = other.activeUser;
    other.activeUser = nullptr;
    if(other.nextReco != nullptr) {
        nextReco = other.nextReco;
    }
    other.nextReco = nullptr;
    for(std::string command : other.commandsSet) {
        commandsSet.insert(command);
    }
    for(std::string str : other.input) {
        input.push_back(str);
    }
    for(Watchable* watchable : other.content) {
        content.push_back(watchable);
        watchable = nullptr;
    }
    for(BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action);
        action = nullptr;
    }
    for(auto user : other.userMap) {
        userMap.insert({user.first, user.second});
        user.second = nullptr;
    }
    other.clear();
}

Session& Session::operator=(Session&& other) {
    if(this != &other) {
        for(Watchable* watchable : content) {
            delete(watchable);
            watchable = nullptr;
        }
        for(BaseAction* action : actionsLog) {
            delete(action);
            action = nullptr;
        }
        for(auto user : userMap) {
            delete(user.second);
            user.second = nullptr;
        }
        clear();
        activeUser = nullptr;
        activeUser = other.activeUser;
        other.activeUser = nullptr;
        nextReco = other.nextReco;
        other.nextReco = nullptr;

        for(std::string command : other.commandsSet) {
            commandsSet.insert(command);
        }
        for(std::string str : other.input) {
            input.push_back(str);
        }
        for(Watchable* watchable : other.content) {
            content.push_back(watchable);
            watchable = nullptr;
        }
        for(BaseAction* action : other.actionsLog) {
            actionsLog.push_back(action);
            action = nullptr;
        }
        for(auto user : other.userMap) {
            userMap.insert({user.first, user.second});
            user.second = nullptr;
        }
        other.clear();
    }
    return *this;
}

// Session content getter
const std::vector<Watchable*>& Session::getContent() const{
    return content;
}

std::unordered_map<std::string,User*>& Session::getUserMap(){
    return userMap;
}

std::vector<BaseAction*>& Session::getActionsLog() {
    return actionsLog;
}

std::vector<std::string> Session::getInput() {
    return input;
}

bool Session::containsUser(const std::string& name) {
    for (const auto& user : userMap) {
        if (user.first == name)
            return true;
    }
    return false;
}

void Session::clear() {
    commandsSet.clear();
    input.clear();
    content.clear();
    actionsLog.clear();
    userMap.clear();
}

User* Session::getActiveUser() const {
    return activeUser;
}

void Session::setActiveUser(User* user) {
    activeUser = user;
}

void Session::commandAnalyzer(const std::string command) {
    string str;
    int inputIndex = 0;
    for(int i = 0; i < (int)command.length(); i++) {
        if(command[i] != ' ') {
            str += command[i];
        } else {
            input.push_back(string(str));
            inputIndex++;
            if(i != (int)command.length() - 1)
                str = "";
        }
    }
    input.push_back(string(str));
}


void Session::start() {
    cout << "SPLFLIX is now on!" << endl;
    string command;
    bool exit = false;
    while (!exit) {
        getline(cin, command);
        commandAnalyzer(command);
        if (commandsSet.find(input[0]) == commandsSet.end()) {
            throw std::invalid_argument("received invalid command");
        } else {
            exit = (input[0] == "exit");
            execute(input[0]);
            input.clear();
        }
    }
}

void Session::execute(string command) {
    BaseAction *action;
    if (command == "createuser") {
        action = new CreateUser();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "changeuser") {
        action = new ChangeActiveUser();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "deleteuser") {
        action = new DeleteUser();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "dupuser") {
        action = new DuplicateUser();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "content") {
        action = new PrintContentList();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "watchhist") {
        action = new PrintWatchHistory();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if (command == "watch" || command == "y") {
        if(command == "y") {
            string str = to_string(nextReco->getId() + 1);
            input.push_back(string(str));
        }
        action = new Watch();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if(command == "log") {
        action = new PrintActionsLog();
        action->act(*this);
        actionsLog.push_back(action);
    }
    if(command == "exit") {
        action = new Exit();
        action->act(*this);
        actionsLog.push_back(action);
    }
}

void Session::setNextReco(Watchable* next) {
    nextReco = next;
}





