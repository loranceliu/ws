#include "../include/Event.h"
#include <thread>

void event::EventManager::registerHandler(std::shared_ptr<event::EventHandler> handler) {
    handlers.push_back(std::move(handler));
}

void event::EventManager::handleEvent(const std::string& event,std::shared_ptr<std::string> msg,std::shared_ptr<server::WebsocketSession> session) {
    bool handlerFound = false;
    for (const auto &handler: handlers) {
        if (handler->getType() == event) {
            handler->handle(std::move(session), std::move(msg));
            handlerFound = true;
            break;
        }
    }

    if(!handlerFound) {
        std::cout << "No handler found for event: " << event << std::endl;
    }
//    if (handlers.find(event) != handlers.end()) {
//        handlers[event]->handle(std::move(session),std::move(msg));
//    } else {
//        std::cout << "No handler found for event: " << event << std::endl;
//    }
}

void event::EventHandlerOne::handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) {
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(*msg, root))
    {
        std::string id = root["id"].asString();
        session->sendToAll(msg);
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
    }
}

std::string event::EventHandlerOne::getType() {
    return "All";
}

void event::EventHandlerTwo::handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) {
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(*msg, root))
    {
        std::string id = root["id"].asString();
        std::string to = root["to"].asString();
        std::cout << "to:" << id << std::endl;
        session->sendTo(to,msg);
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
    }
}

std::string event::EventHandlerTwo::getType() {
    return "Single";
}

void event::LoginEventHandler::handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) {
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(*msg, root))
    {
       std::string id = root["id"].asString();
       std::cout << "id:" << id << std::endl;
       session->login(id);
       std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
    }
    //auto message = std::make_shared<std::string>("{'code':'200','msg':'success'}");
    //session->send(message);
}

std::string event::LoginEventHandler::getType() {
    return "Login";
}
