#include "../include/WebsocketServer.h"
#include <thread>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using Threadpool = boost::asio::thread_pool;

int main () {
    try {
        auto handler1 = std::make_shared<event::EventHandlerOne>();
        auto handler2 = std::make_shared<event::EventHandlerTwo>();
        auto loginHandler = std::make_shared<event::LoginEventHandler>();

        auto eventManager= std::make_shared<event::EventManager>();
        eventManager->registerHandler(std::move(loginHandler));
        eventManager->registerHandler(std::move(handler1));
        eventManager->registerHandler(std::move(handler2));

        auto sessionManager = std::make_shared<server::SessionManager>();

        auto const address = net::ip::make_address("0.0.0.0");
        unsigned short port = 9002;

        net::io_context ioc;
        tcp::endpoint endpoint{address, port};

        std::shared_ptr<Threadpool>  pool = std::make_unique<Threadpool>(5);

        auto server = std::make_shared<server::WebsocketServer>(ioc, endpoint, pool, std::move(eventManager));

        std::cout << "Main Thread ID: " << std::this_thread::get_id() << std::endl;

        ioc.run();

        pool->join();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}