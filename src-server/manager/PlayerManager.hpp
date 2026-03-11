#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

#include <gameroom/GameRoom.hpp>
#include <map>
#include <queue>
#include <shared_mutex>
#include <memory>
#include <functional>

namespace game {

struct PlayerInfo {
    int id;
    int rating;
    bool inGame;
    bool searching;
};

class PlayerManager {
public:
    PlayerManager(boost::asio::io_context& ioContext);
    
    int registerPlayer();
    void startSearch(int playerId);
    void cancelSearch(int playerId);
    
    std::shared_ptr<GameRoom> getActiveGame(int playerId) const;
    bool isInGame(int playerId) const;
    
    void addRating(int playerId, int points);

private:
    int findBestMatch(int playerId);
    void createGame(int player1, int player2);
    void onGameEnded(int winnerId, int loserId);
    
    mutable std::shared_mutex mutex_;
    std::map<int, PlayerInfo> players_;
    std::queue<int> searchQueue_;
    std::map<int, std::shared_ptr<GameRoom>> activeGames_;
    boost::asio::io_context& ioContext_;
    
    static constexpr int RATING_WIN_BONUS = 10;
    static constexpr int RATING_LOSS_PENALTY = 5;
};

} // namespace game

#endif // PLAYERMANAGER_HPP