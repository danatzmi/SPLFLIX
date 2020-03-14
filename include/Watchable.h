#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    Watchable(const Watchable& other);
    virtual ~Watchable();
    virtual std::string toString() const = 0;
    virtual std::string toStringDetailed() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    virtual Watchable* clone() const = 0;
    virtual long getId() const;
    virtual int getLength() const;
    virtual const std::vector<std::string>& getTags() const;
    virtual const std::string& getName() const = 0;
    virtual bool containsTag(std::string tag) = 0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    Movie(const Movie& other);
    virtual std::string toString() const;
    virtual std::string toStringDetailed() const;
    virtual Watchable* getNextWatchable(Session&) const;
    const std::string& getName() const;
    Watchable* clone() const;
    virtual bool containsTag(std::string tag);
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    Episode(const Episode& other);
    virtual std::string toString() const;
    virtual std::string toStringDetailed() const;
    virtual Watchable* getNextWatchable(Session&) const;
    const std::string& getName() const;
    Watchable* clone() const;
    int getSeason() const;
    int getEpisode() const;
    long getNextEpisodeId() const;
    void setNextEpisodeId(long nextId);
    virtual bool containsTag(std::string tag);
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif