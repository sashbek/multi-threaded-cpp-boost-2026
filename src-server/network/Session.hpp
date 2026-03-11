#ifndef SESSION_HPP
#define SESSION_HPP

#include <manager/PlayerManager.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <memory>
#include <queue>
#include <string>

namespace game {

using boost::asio::ip::tcp;
namespace asio = boost::asio;
namespace json = boost::json;

struct Message {
    std::string type;
    std::string data;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::shared_ptr<PlayerManager> playerManager);
    ~Session();

    void start();
    void sendMessage(const std::string& message);
    void disconnect();

    int getPlayerId() const { return playerId_; }

private:
    void readMessage();
    void processMessage(const std::string& data);
    void sendQueuedMessages();
    void handleGameStateTimer();
    
    void handleStartSearch();
    void handleCancelSearch();
    void handlePlaceCard(const json::object& obj);
    void handleGetGameState();
    void handleSurrender();
    
    void sendGameState(const GameState& state);
    void sendGameEnded(int winnerId, int loserId);
    void sendError(const std::string& error);

    tcp::socket socket_;
    std::shared_ptr<PlayerManager> playerManager_;
    int playerId_{-1};
    
    asio::streambuf readBuffer_;
    std::queue<std::string> writeQueue_;
    asio::steady_timer gameStateTimer_;
    
    std::shared_ptr<GameRoom> currentGame_;
    
    bool isConnected_{true};
    static constexpr int GAME_STATE_UPDATE_MS = 200;
};

} // namespace game

#endif // SESSION_HPP
