#include "../include/Action.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <iostream>

/////////////BaseAction methods implementation:
BaseAction::BaseAction() : errorMsg(""), status(PENDING) {}

BaseAction::BaseAction(const BaseAction& other) : errorMsg(other.errorMsg), status(other.status) {}

BaseAction::~BaseAction() {}

void BaseAction::complete(){
    status = COMPLETED;
}

void BaseAction::error(const std::string& errorMsg) {
    status = ERROR;
    this->errorMsg = errorMsg;
    std::cout << this->toString() << std::endl;
}

const ActionStatus BaseAction::getStatus() const {
    return status;
}

const std::string& BaseAction::getErrorMsg() const {
    return errorMsg;
}

/////////////CreateUser methods implementation:

void CreateUser::act(Session& session) {
    std::vector<std::string> input = session.getInput();
    std::string userName =  input[1];
    std::string recommendationAlgorithm =  input[2];
    if(!session.containsUser(userName)) {
        if(recommendationAlgorithm == "len") {
            session.getUserMap()[userName] = new LengthRecommenderUser(userName);
            complete();
        } else if(recommendationAlgorithm == "rer") {
            session.getUserMap()[userName] = new RerunRecommenderUser(userName);
            complete();
        } else if(recommendationAlgorithm == "gen") {
            session.getUserMap()[userName] = new GenreRecommenderUser(userName);
            complete();
        } else {
            error("Invalid algorithm type.");
        }
    }
    else {
        error("Username already exists.");
    }
}

std::string CreateUser::toString() const{
    std::string output = "CreateUser ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* CreateUser::clone() {
    return new CreateUser(*this);
}


/////////////ChangeActiveUser methods implementation:
void ChangeActiveUser::act(Session& session){
    std::vector<std::string> input = session.getInput();
    std::string userName =  session.getInput()[1];
    if(userName == session.getActiveUser()->getName()) {
        error(userName +" is already active.");
    } else if(session.containsUser(userName)) {
        session.setActiveUser(session.getUserMap()[userName]);
        complete();
    } else  {
        error("Username does not exist.");
    }
}

std::string ChangeActiveUser::toString() const{
    std::string output = "ChangeActiveUser ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}


/////////////DeleteUser methods implementation:
void DeleteUser::act(Session & session) {
    std::vector<std::string> input = session.getInput();
    std::string userName =  input[1];
    if(session.containsUser(userName)) {
        delete session.getUserMap()[userName];
        session.getUserMap().erase(userName);
        complete();
    } else {
        error("Username does not exist.");
    }
}
std::string DeleteUser::toString() const {
    std::string output = "DeleteUser ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* DeleteUser::clone() {
    return new DeleteUser(*this);
}


/////////////DuplicateUser methods implementation:
void DuplicateUser::act(Session & session) {
    std::vector<std::string> input = session.getInput();
    std::string originalUserName =  input[1];
    std::string newUserName =  input[2];
    if (session.containsUser(originalUserName) & !session.containsUser(newUserName)) {
        User* originalUser = session.getUserMap()[originalUserName];
        User* newUser = originalUser->clone();
        newUser->setName(newUserName);
        session.getUserMap()[newUserName] = newUser;
        complete();
    } else if (!session.containsUser(originalUserName)) {
        error("Original username does not exist.");
    } else {
        error("New username is already taken.");
    }

}
std::string DuplicateUser::toString() const{
    std::string output = "DuplicateUser ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* DuplicateUser::clone() {
    return new DuplicateUser(*this);
}


/////////////PrintContentList methods implementation:
void PrintContentList::act (Session& session) {
    for (int i = 0; i < (int)session.getContent().size(); i++) {
        std::cout << i + 1 << ". " << session.getContent()[i]->toStringDetailed() << std::endl;
    }
    complete();
}
std::string PrintContentList::toString() const {
    std::string output = "PrintContentList ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* PrintContentList::clone() {
    return new PrintContentList(*this);
}


/////////////PrintWatchHistory methods implementation:
void PrintWatchHistory::act (Session& session) {
    User* activeUser = session.getActiveUser();
    std::vector<Watchable*> activeUserHistory = activeUser->get_history();
    std::cout << "Watch history for " << activeUser->getName() << std::endl;
    for (int i = 0; i < (int)activeUserHistory.size(); i++) {
        std::cout << i + 1 << ". " << activeUserHistory[i]->toString() << std::endl;
    }
    complete();
}

std::string PrintWatchHistory::toString() const{
    std::string output = "PrintWatchHistory ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}


/////////////Watch methods implementation:

void Watch::act(Session& session) {
    std::vector<std::string> input = session.getInput();
    int contentId =  std::stoi(input[1]);
    User* activeUser = session.getActiveUser();
    if (contentId >= 1 && contentId <= (int)session.getContent().size()) {
        Watchable* content = session.getContent()[contentId - 1];
        std::cout << "Watching " << content->toString() << std::endl;
        activeUser->addToHistory(content);
        activeUser->updateTags(content);
        Watchable* next = activeUser->getRecommendation(session);
        complete();
        if (next != nullptr) {
            session.setNextReco(next);
            std::cout << "We recommend watching " << next->toString() << ", continue watching? [y/n]" << std::endl;
        }
    } else {
        session.setNextReco(nullptr);
        error("Invalid content id.");
    }
}

std::string Watch::toString() const{
    std::string output = "Watch ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* Watch::clone() {
    return new Watch(*this);
}


/////////////PrintActionsLog methods implementation:
void PrintActionsLog::act(Session& sess) {
    std::vector<BaseAction*> actionLog = sess.getActionsLog();
    for(int i = (int)actionLog.size() - 1; i >= 0; i--) {
        std::cout << actionLog[i]->toString() << std::endl;
    }
    complete();
}

std::string PrintActionsLog::toString() const{
    std::string output = "PrintActionsLog ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}


/////////////Exit methods implementation:
void Exit::act(Session& sess) { complete(); }

std::string Exit::toString() const {
    std::string output = "Exit ";
    if(getStatus() == ERROR)
        output = output + "ERROR: " + getErrorMsg();
    else if(getStatus() == PENDING)
        output = output + "PENDING";
    else
        output = output + "COMPLETED";
    return output;
}

BaseAction* Exit::clone() {
    return new Exit(*this);
}

