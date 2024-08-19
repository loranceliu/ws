#ifndef WS_WEBSOCKETSERVER_H
#define WS_WEBSOCKETSERVER_H
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <map>
#include <mutex>
#include <functional>
#include <json/json.h>
#include "Event.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace Json;
using ThreadPool = boost::asio::thread_pool;


namespace event {
    class EventManager;
}

namespace server {

    class WebsocketSession;

    class SessionManager {
    public:
        void addSession(const std::string& sessionId, std::shared_ptr<server::WebsocketSession> session);

        std::shared_ptr<server::WebsocketSession> getSession(const std::string& sessionId);

        void removeSession(const std::string& sessionId);

        void applyToSession(std::function<void(const std::string& id,std::shared_ptr<server::WebsocketSession>)> func);

    private:
        std::map<std::string, std::shared_ptr<server::WebsocketSession>> sessions_;
        std::mutex mutex_;
    };

    class WebsocketServer : public std::enable_shared_from_this<WebsocketServer>{

    public:
        WebsocketServer(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<ThreadPool> pool, std::shared_ptr<event::EventManager> eventManager);

        std::shared_ptr<event::EventManager> getEventManager();

        std::shared_ptr<SessionManager> getSessionManager();

        std::shared_ptr<ThreadPool> getPool();
    private:
        void do_accept();

        std::shared_ptr<ThreadPool> pool_;
        tcp::acceptor acceptor_;
        std::shared_ptr<event::EventManager> eventManager_;
        std::shared_ptr<SessionManager> sessionManager_;
    };

    class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {

    public:
        explicit WebsocketSession(tcp::socket socket,std::shared_ptr<WebsocketServer> websocketServer);

        void run();

        void send(const std::shared_ptr<std::string>& message);

        void login(const std::string& id);

        void logout(const std::string& id);

        void sendTo(const std::string& id, const std::shared_ptr<std::string>& message);

        void sendToAll(const std::shared_ptr<std::string>& message);

    private:
        void do_read();

        void onMessage(std::size_t bytes_transferred);

        void do_write(const std::shared_ptr<std::string>& message);

        websocket::stream<tcp::socket> ws_;
        beast::flat_buffer read_buffer_;
        beast::flat_buffer write_buffer_;
        std::shared_ptr<WebsocketServer> server_;
//        std::shared_ptr<event::EventManager> eventManager_;
//        std::shared_ptr<SessionManager> sessionManager_;
    };

}


#endif //WS_WEBSOCKETSERVER_H