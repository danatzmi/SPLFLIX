
#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"
#include "json.hpp"

class User;
class Watchable;
class Action;

class Session{
public:
    Session(const std::string& configFilePath);
    Session(const Session& other); //Copy constructor.
    Session& operator=(const Session& other); //Copy assignment operator.
    ~Session(); //Destructor.
    Session(Session&& other); //Move constructor.
    Session& operator=(Session&& other); //Move assignment.
    void start();
    const std::vector<Watchable*>& getContent() const;
    std::unordered_map<std::string,User*>& getUserMap();
    std::vector<std::string> getInput();
    std::vector<BaseAction*>& getActionsLog();
    User* getActiveUser() const;
    void setActiveUser(User* user);
    void setNextReco(Watchable* next);
    bool containsUser(const std::string& name);
    void commandAnalyzer(const std::string command);
    void execute(std::string command);
    void clear();
private:
    User* activeUser;
    Watchable* nextReco;
    std::unordered_set<std::string> commandsSet;
    std::vector<std::string> input;
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
};
#endif
