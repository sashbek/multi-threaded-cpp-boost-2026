#ifndef SERVER_HPP
#define SERVER_HPP

#include <network/Session.hpp>
#include <manager/PlayerManager.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>

namespace game {

class Server {
public:
    Server(short port, size_t threadCount = std::thread::hardware_concurrency());
    ~Server();

    void start();
    void stop();

private:
    void startAccept();
    void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& ec);
    void runIoContext();

    boost::asio::io_context ioContext_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard_;
    tcp::acceptor acceptor_;
    
    std::shared_ptr<PlayerManager> playerManager_;
    std::vector<std::thread> threadPool_;
    std::atomic<bool> isRunning_{true};
    
    size_t threadCount_;
    short port_;
};

} // namespace game

#endif // SERVER_HPP
