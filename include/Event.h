//
// Created by fangwei on 2024/7/2.
//

#ifndef WS_EVENT_H
#define WS_EVENT_H
#include <iostream>
#include <map>
#include <memory>
#include "WebsocketServer.h"

namespace server {
    class WebsocketSession;
}

namespace event {

    class EventHandler {

    public:
        virtual void handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) = 0;
        virtual std::string  getType() = 0;
        virtual ~EventHandler() = default;
    };


    class EventHandlerOne : public EventHandler {

    public:
        void handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) override;

        std::string getType() override;
    };


    class EventHandlerTwo : public EventHandler {
    public:
        void handle(std::shared_ptr<server::WebsocketSession> session, std::shared_ptr<std::string> msg) override;

        std::string getType() override;
    };

    class LoginEventHandler : public EventHandler {
    public:
        void handle(std::shared_ptr<server::WebsocketSession> session,std::shared_ptr<std::string> msg) override;

        std::string getType() override;

    };

    class EventManager {

    private:
        std::vector<std::shared_ptr<EventHandler>> handlers;

    public:
        void registerHandler(std::shared_ptr<EventHandler> handler);

        void handleEvent(const std::string& event, std::shared_ptr<std::string> msg,std::shared_ptr<server::WebsocketSession> session);
    };

}



#endif //WS_EVENT_H
