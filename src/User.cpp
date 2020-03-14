#include "../include/Watchable.h"
#include "../include/User.h"
#include "../include/Session.h"

using namespace std;

/////////////User methods implementation:

//User constructor.


User::User(const string& name) : history({}), name(name) {}

User::User(const User& other) : User(other.name) {
    for(int i = 0; i < (int)other.get_history().size(); i++) {
        this->history.push_back(other.get_history()[i]->clone());
    }
}

User& User::operator=(const User& other) {
    name = other.name;
    for(int i = 0; i < (int)history.size(); i++) {
        delete(history[i]);
        history[i] = nullptr;
    }
    history.clear();
    for(int i = 0; i < (int)other.get_history().size(); i++) {
        this->history.push_back(other.get_history()[i]->clone());
    }
    return *this;
}

User::User(User&& other) : User(other.name) {
    for(int i = 0; i < (int)other.get_history().size(); i++) {
        this->history.push_back(other.get_history()[i]);
        other.get_history()[i] = nullptr;
    }
    other.history.clear();
}

User& User::operator=(User&& other) {
    if(this != &other) {
        name = other.name;
        for (int i = 0; i < (int) history.size(); i++) {
            delete (history[i]);
            history[i] = nullptr;
        }
        history.clear();
        for (int i = 0; i < (int) other.get_history().size(); i++) {
            this->history.push_back(other.get_history()[i]);
            other.get_history()[i] = nullptr;
        }
        other.history.clear();
    }
    return *this;
}

User::~User() {
    for(int i = 0; i < (int)history.size(); i++)
        delete(history[i]);
}

string User::getName() const {
    return name;
}

vector<Watchable*> User::get_history() const {
    return history;
}

void User::addToHistory(Watchable* w) {
    this->history.push_back(w->clone());
}

void User::setName(const std::string& name) {
    this->name = name;
}

bool User::isWatchedEverything(const Session& s) {
    for (int i = 0; i < (int)s.getContent().size(); i++) {
        bool found = false;
        for (int j = 0; j < (int)history.size() && !found; j++) {
            if (s.getContent()[i]->getId() == history[j]->getId()) {
                found = true;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}


/////////////LengthRecommenderUser methods implementation:

LengthRecommenderUser::LengthRecommenderUser(const std::string& name) : User(name) {}

LengthRecommenderUser::LengthRecommenderUser(const LengthRecommenderUser& other) : User(other){}


Watchable* LengthRecommenderUser::getRecommendation(Session& s) {
    if (isWatchedEverything(s)) {
        return nullptr;
    }
    Watchable* last = history[(int)history.size() - 1];
    if (last->getNextWatchable(s) != nullptr) {
        return last->getNextWatchable(s);
    }
    int sum = 0;
    for (int i = 0; i < (int)history.size(); i++) {
        sum += history[i]->getLength();
    }
    float avg = sum / (int)history.size();
    Watchable* min = nullptr;
    double minDifference = INTMAX_MAX;
    for (int i = 0; i < (int)s.getContent().size(); i++) {
        bool existsInHistory = false;
        for (int j = 0; j < (int)history.size() && !existsInHistory; j++){
            if (s.getContent()[i]->getId() == history[j]->getId())
                existsInHistory = true;
        }
        if (!existsInHistory && minDifference > abs(s.getContent()[i]->getLength() - avg)) {
            min = s.getContent()[i];
            minDifference = abs(s.getContent()[i]->getLength() - avg);
        }
    }
    return min;
}

void LengthRecommenderUser::updateTags(Watchable* w) {}

User* LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}

/////////////RerunRecommenderUser methods implementation:

RerunRecommenderUser::RerunRecommenderUser(const string& name) : User(name), index(-1) {}

RerunRecommenderUser::RerunRecommenderUser(const RerunRecommenderUser& other) : User(other), index(other.getIndex()) {}


Watchable* RerunRecommenderUser::getRecommendation(Session& s) {
    Watchable* last = history[(int)history.size() - 1];
    if (last->getNextWatchable(s) != nullptr) {
        return last->getNextWatchable(s);
    }
    index++;
    return history[(index % (int)history.size())];
}

void RerunRecommenderUser::updateTags(Watchable* w){}

int RerunRecommenderUser::getIndex() const {
    return index;
}

User* RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);
}

/////////////GenreRecommenderUser methods implementation:

GenreRecommenderUser::GenreRecommenderUser(const string& name) : User(name), tagsMap({}) {}

GenreRecommenderUser::GenreRecommenderUser(const GenreRecommenderUser& other) : User(other), tagsMap(other.tagsMap) {}


Watchable* GenreRecommenderUser::getRecommendation(Session& s) {
    if (isWatchedEverything(s)) {
        return nullptr;
    }
    Watchable* last = history[(int)history.size() - 1];
    if (last->getNextWatchable(s) != nullptr) {
        return last->getNextWatchable(s);
    }

    while ((int)tagsMap.size() > 0) {
        int max = 0;
        std::string maxTag;
        for (const auto &tag : tagsMap) {
            if (tag.second > max) {
                max = tag.second;
                maxTag = tag.first;
            } else if ((tag.second == max) && tag.first < maxTag) {
                max = tag.second;
                maxTag = tag.first;
            }
        }
        for (int i = 0; i < (int)s.getContent().size(); i++) {
            if (s.getContent()[i]->containsTag(maxTag)) {
                bool existsInHistory = false;
                for (int j = 0; j < (int)history.size() && !existsInHistory; j++) {
                    if (s.getContent()[i]->getId() == history[j]->getId())
                        existsInHistory = true;
                }
                if (!existsInHistory) {
                    return s.getContent()[i];
                }
            }
        }
        tagsMap.erase(maxTag);
    }
    return nullptr;
}

void GenreRecommenderUser::updateTags(Watchable* w) {
    for (const std::string& tag : w->getTags()) {
        tagsMap[tag]++;
    }
}

User* GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);
}



