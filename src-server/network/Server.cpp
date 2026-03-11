#include <network/Server.hpp>
#include <iostream>
#include <signal.h>

namespace game {

Server::Server(short port, size_t threadCount)
    : workGuard_(boost::asio::make_work_guard(ioContext_))
    , acceptor_(ioContext_, tcp::endpoint(tcp::v4(), port))
    , playerManager_(std::make_shared<PlayerManager>(ioContext_))
    , threadCount_(threadCount)
    , port_(port) {
    
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
}

Server::~Server() {
    stop();
}

void Server::start() {
    std::cout << "Starting game server on port " << port_ << std::endl;
    
    startAccept();
    
    for (size_t i = 0; i < threadCount_; ++i) {
        threadPool_.emplace_back(&Server::runIoContext, this);
    }
    
    std::cout << "Server started with " << threadCount_ << " threads" << std::endl;
}

void Server::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        boost::system::error_code ec;
        acceptor_.close(ec);
        
        workGuard_.reset();
        ioContext_.stop();
        
        for (auto& thread : threadPool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        std::cout << "Server stopped" << std::endl;
    }
}

void Server::startAccept() {
    auto socket = std::make_shared<tcp::socket>(ioContext_);
    
    acceptor_.async_accept(*socket, 
        [this, socket](const boost::system::error_code& ec) {
            handleAccept(socket, ec);
        });
}

void Server::handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec) {
    if (!ec) {
        boost::system::error_code endpointEc;
        auto endpoint = socket->remote_endpoint(endpointEc);
        if (!endpointEc) {
            std::cout << "New connection from " << endpoint.address().to_string() 
                      << ":" << endpoint.port() << std::endl;
        }
        
        auto session = std::make_shared<Session>(std::move(*socket), playerManager_);
        session->start();
        
        if (isRunning_) {
            startAccept();
        }
    } else {
        if (ec != boost::asio::error::operation_aborted) {
            std::cerr << "Accept error: " << ec.message() << std::endl;
            
            if (isRunning_) {
                startAccept();
            }
        }
    }
}

void Server::runIoContext() {
    try {
        ioContext_.run();
    } catch (const std::exception& e) {
        std::cerr << "IO context error: " << e.what() << std::endl;
    }
}

} // namespace game
