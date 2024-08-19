#include <boost/asio/post.hpp>
#include "../include/WebsocketServer.h"

server::WebsocketSession::WebsocketSession(tcp::socket socket,std::shared_ptr<WebsocketServer> websocketServer)
        : ws_(std::move(socket)),server_(std::move(websocketServer)) {
    //sessionManager_ = std::move(sessionManager);
}

void server::WebsocketSession::run() {
    ws_.async_accept(
            [self = shared_from_this()](boost::system::error_code ec) {
                if (!ec) {
                    self->do_read();
                }
            });
}

void server::WebsocketSession::send(const std::shared_ptr<std::string>& message) {
    do_write(message);
}

void server::WebsocketSession::login(const std::string& id) {
    server_->getSessionManager()->addSession(id,shared_from_this());
    std::cout << "session " << id << " is connected." << std::endl;
}

void server::WebsocketSession::logout(const std::string& id) {
    server_->getSessionManager()->removeSession(id);
}

void server::WebsocketSession::sendTo(const std::string& id, const std::shared_ptr<std::string>& message) {
    std::shared_ptr<WebsocketSession> targetSession = server_->getSessionManager()->getSession(id);
    if(targetSession != nullptr) {
        targetSession->send(message);
    }
}

void server::WebsocketSession::sendToAll(const std::shared_ptr<std::string>& message) {
    server_->getSessionManager()->applyToSession([self = shared_from_this(),message](const std::string& id, std::shared_ptr<WebsocketSession> session){
        std::shared_ptr<WebsocketSession> targetSession = std::move(session);
       if(self != targetSession) {
           targetSession->send(message);
       }
    });
}

void server::WebsocketSession::do_read() {
    ws_.async_read(
            read_buffer_,
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    self->onMessage(bytes_transferred);
                    self->do_read();
                }
            });
}

void server::WebsocketSession::onMessage(std::size_t bytes_transferred) {

    std::string str = boost::beast::buffers_to_string(read_buffer_.data());

    Json::Reader reader;
    Json::Value root;
    if (reader.parse(str, root))
    {
        std::string type = root["type"].asString();
        std::shared_ptr<std::string> msg = std::make_shared<std::string>(root["msg"].toStyledString());
        boost::asio::post(server_->getPool()->get_executor(),[this,type,msg](){
            server_->getEventManager()->handleEvent(type,msg,std::move(shared_from_this()));
        });
    }
    read_buffer_.consume(bytes_transferred);
}

void server::WebsocketSession::do_write(const std::shared_ptr<std::string>& message) {
    ws_.text(ws_.got_text());
    write_buffer_.commit(boost::asio::buffer_copy(write_buffer_.prepare(message->size()), boost::asio::buffer(*message)));
    ws_.async_write(
            write_buffer_.data(),
            [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    //todo
                }
                self->write_buffer_.consume(bytes_transferred);
            });
}

server::WebsocketServer::WebsocketServer(net::io_context& ioc, const tcp::endpoint& endpoint,std::shared_ptr<ThreadPool> pool,std::shared_ptr<event::EventManager> eventManager)
        : acceptor_(ioc, endpoint), eventManager_(std::move(eventManager)),pool_(std::move(pool)) {
    sessionManager_ = std::make_shared<SessionManager>();
    do_accept();
}

void server::WebsocketServer::do_accept() {
    acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<WebsocketSession>(std::move(socket),std::move(shared_from_this()))->run();
                }
                do_accept();
            });
}

std::shared_ptr<event::EventManager> server::WebsocketServer::getEventManager() {
    return this->eventManager_;
}

std::shared_ptr<ThreadPool> server::WebsocketServer::getPool() {
    return this->pool_;
}

std::shared_ptr<server::SessionManager> server::WebsocketServer::getSessionManager() {
    return this->sessionManager_;
}

void server::SessionManager::addSession(const std::string& sessionId, std::shared_ptr<server::WebsocketSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[sessionId] = std::move(session);
}

std::shared_ptr<server::WebsocketSession> server::SessionManager::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

void server::SessionManager::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(sessionId);
}

void server::SessionManager::applyToSession(std::function<void(const std::string& id,std::shared_ptr<server::WebsocketSession>)> func) {
    if(!sessions_.empty()) {
        for (auto & it : sessions_) {
            const std::string& sessionId = it.first;
            std::shared_ptr<server::WebsocketSession> session = it.second;
            if(session) {
                func(sessionId,std::move(session));
            }
        }
    }
}