#include "../include/Watchable.h"
#include "../include/User.h"
#include "../include/Session.h"
#include <iostream>

/////////////Watchable methods implementation:

//Watchable constructor.
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags) : id(id), length(length), tags() {
    for (const std::string& tag : tags){
        this->tags.push_back(tag);
    }
}

Watchable::Watchable(const Watchable& other) : id(other.id), length(other.length), tags({}) {
    for (const std::string& tag : other.tags){
        this->tags.push_back(tag);
    }
}

Watchable::~Watchable() {}

//Getter of watchable id.
long Watchable::getId() const {
    return id;
}

//Getter of watchable length.
int Watchable::getLength() const{
    return length;
}

//Getter of watchable tags.
const std::vector<std::string>& Watchable::getTags() const {
    return tags;
}


//////////////Movie methods implementation:

//Movie constructor.
Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags) : Watchable(id, length, tags), name(name) {}

Movie::Movie(const Movie& other) : Watchable(other), name(other.name) {}

//Movie name getter.
const std::string& Movie::getName() const {
    return name;

}

//Movie toString method.
std::string Movie::toString() const {
    return getName();
}

std::string Movie::toStringDetailed() const {
    std::string tagsToString = "[";
    for (const std::string& tag : getTags()){
        tagsToString.append(tag + ", ");
    }
    tagsToString = tagsToString.substr(0, (int)tagsToString.size() - 2).append("]");
    return getName() + " " + std::to_string(getLength()) + " minutes " + tagsToString;
}

//Movie getNextWatchable method.
Watchable* Movie::getNextWatchable(Session& s) const {
    return nullptr;
}

Watchable* Movie::clone() const {
    Watchable* output = new Movie(*this);
    return output;
}

bool Movie::containsTag(std::string tag) {
    for (int i = 0; i < (int)this->getTags().size(); i++) {
        if (getTags()[i] == tag)
            return true;
    }
    return false;
}

//////////////Episode methods implementation:

//Episode constructor.
Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags) :
Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(id + 1) {}

Episode::Episode(const Episode& other) :
Watchable(other), seriesName(other.seriesName), season(other.season), episode(other.episode), nextEpisodeId(other.getNextEpisodeId()) {}


//Episode name getter.
const std::string& Episode::getName() const {
    return seriesName;
}

//Episode season getter.
int Episode::getSeason() const {
    return season;
}

//Episode episode getter.
int Episode::getEpisode() const {
    return episode;
}

//Episode toString method.
std::string Episode::toString() const {
    return getName() + " S" + std::to_string(getSeason()) + "E" + std::to_string(getEpisode());
}


std::string Episode::toStringDetailed() const {
    std::string tagsToString = "[";
    for (const std::string& tag : getTags()){
        tagsToString.append(tag + ", ");
    }
    tagsToString = tagsToString.substr(0, (int)tagsToString.size() - 2).append("]");
    return getName() + " S" + std::to_string(getSeason()) + "E" + std::to_string(getEpisode()) + " " + std::to_string(getLength()) + " minutes " + tagsToString;
}

//Episode getNextWatchable method.
Watchable* Episode::getNextWatchable(Session& s) const {
    if(getNextEpisodeId() != -1)
        return s.getContent()[getNextEpisodeId()];
    return nullptr;
}

void Episode::setNextEpisodeId(long nextId) {
    nextEpisodeId = nextId;
}

long Episode::getNextEpisodeId() const {
    return nextEpisodeId;
}

Watchable* Episode::clone() const {
    return new Episode(*this);
}

bool Episode::containsTag(std::string tag) {
    for (int i = 0; i < (int)this->getTags().size(); i++) {
        if (getTags()[i] == tag)
            return true;
    }
    return false;
}