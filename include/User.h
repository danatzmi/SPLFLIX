#ifndef USER_H_
#define USER_H_
#include <map>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    User(const User& other); //Copy constructor.
    User& operator=(const User& other); //Copy assignment operator.
    virtual ~User(); //Destructor.
    User(User&& other); //Move constructor.
    User& operator=(User&& other); //Move assignment.
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual User* clone() = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void setName(const std::string& name);
    void addToHistory(Watchable* toAdd);
    virtual void updateTags(Watchable* w) = 0;
    bool isWatchedEverything(const Session& s);
protected:
    std::vector<Watchable*> history;
private:
    std::string name;
};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(const LengthRecommenderUser& other);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTags(Watchable* w);
    User* clone();
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(const RerunRecommenderUser& other);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTags(Watchable* w);
    int getIndex() const;
    User* clone();
private:
    int index;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(const GenreRecommenderUser& other);
    virtual Watchable* getRecommendation(Session& s);
    virtual void updateTags(Watchable* w);
    User* clone();
private:
    std::unordered_map<std::string,int> tagsMap;
};


#endif