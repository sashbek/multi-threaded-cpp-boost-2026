#ifndef GAMEROOM_HPP
#define GAMEROOM_HPP

#include <model/Unit.hpp>
#include <model/Card.hpp>
#include <model/GameState.hpp>
#include <game/CardsLibrary.hpp>
#include <game/CardEffects.hpp>
#include <boost/asio.hpp>
#include <array>
#include <vector>
#include <thread>
#include <shared_mutex>
#include <atomic>
#include <functional>

namespace game {

class GameRoom : public std::enable_shared_from_this<GameRoom> {
public:
    using GameEndedCallback = std::function<void(int winnerId, int loserId)>;

    GameRoom(int player1, int player2, boost::asio::io_context& ioContext);
    ~GameRoom();
    
    bool placeCard(int playerId, CardType cardType, int x);
    GameState getGameState(int playerId) const;
    bool isActive() const { return gameActive_; }
    int getOpponentId(int playerId) const;
    
    void setGameEndedCallback(GameEndedCallback callback) { gameEndedCallback_ = callback; }
    
    void updateGame();

private:
    void startGameLoop();
    void onTick(const boost::system::error_code& ec);
    void resolveCombats();
    void removeDeadUnits();
    void checkGameEnd();
    bool isValidPlacement(int playerId, CardType cardType, int x) const;
    bool areNeighbors(const Position& p1, const Position& p2) const;
    
    mutable std::shared_mutex mutex_;
    std::array<int, 2> players_;
    std::array<int, 2> health_{10, 10};
    std::array<int, 2> power_{5, 5};
    
    std::vector<Unit> units_;
    int nextUnitId_{0};
    int tickCounter_{0};
    
    std::atomic<bool> gameActive_{true};
    std::atomic<bool> gameEnded_{false};
    std::unique_ptr<std::thread> gameLoopThread_;
    boost::asio::steady_timer gameTickTimer_;
    boost::asio::io_context& ioContext_;
    
    GameEndedCallback gameEndedCallback_;
    
    static constexpr int FIELD_WIDTH = 10;
    static constexpr int FIELD_HEIGHT = 15;
    static constexpr int MAX_POWER = 10;
    static constexpr int TICK_INTERVAL_MS = 500;
};

} // namespace game

#endif // GAMEROOM_HPP
