#include <manager/PlayerManager.hpp>
#include <algorithm>
#include <climits>

namespace game {

PlayerManager::PlayerManager(boost::asio::io_context& ioContext)
    : ioContext_(ioContext) {}

int PlayerManager::registerPlayer() {
    std::unique_lock lock(mutex_);
    static int nextId = 1;
    int id = nextId++;
    players_[id] = PlayerInfo{id, 1000, false, false};
    return id;
}

void PlayerManager::startSearch(int playerId) {
    std::unique_lock lock(mutex_);
    
    auto& player = players_[playerId];
    if (player.inGame || player.searching) return;
    
    if (!searchQueue_.empty()) {
        int opponentId = findBestMatch(playerId);
        if (opponentId != -1) {
            createGame(playerId, opponentId);
            return;
        }
    }
    
    player.searching = true;
    searchQueue_.push(playerId);
}

void PlayerManager::cancelSearch(int playerId) {
    std::unique_lock lock(mutex_);
    
    auto& player = players_[playerId];
    player.searching = false;
    
    // Удаляем из очереди (пришлось бы пересоздать очередь, упрощенно)
    // В реальном проекте лучше использовать priority_queue или set
}

int PlayerManager::findBestMatch(int playerId) {
    std::queue<int> temp;
    int bestMatch = -1;
    int minDiff = INT_MAX;
    
    while (!searchQueue_.empty()) {
        int candidate = searchQueue_.front();
        searchQueue_.pop();
        
        if (candidate == playerId) continue;
        
        int diff = std::abs(players_[candidate].rating - players_[playerId].rating);
        if (diff < minDiff) {
            minDiff = diff;
            bestMatch = candidate;
        }
        temp.push(candidate);
    }
    
    // Возвращаем неиспользованных в очередь
    while (!temp.empty()) {
        if (temp.front() != bestMatch) {
            searchQueue_.push(temp.front());
        }
        temp.pop();
    }
    
    return bestMatch;
}

void PlayerManager::createGame(int player1, int player2) {
    auto game = std::make_shared<GameRoom>(player1, player2, ioContext_);
    
    // Устанавливаем callback для окончания игры
    game->setGameEndedCallback([this](int winner, int loser) {
        onGameEnded(winner, loser);
    });
    
    activeGames_[player1] = game;
    activeGames_[player2] = game;
    
    players_[player1].inGame = true;
    players_[player2].inGame = true;
    players_[player1].searching = false;
    players_[player2].searching = false;
}

void PlayerManager::onGameEnded(int winnerId, int loserId) {
    std::unique_lock lock(mutex_);
    
    // Обновление рейтинга
    players_[winnerId].rating += RATING_WIN_BONUS;
    players_[loserId].rating = std::max(0, players_[loserId].rating - RATING_LOSS_PENALTY);
    
    // Освобождение игроков
    players_[winnerId].inGame = false;
    players_[loserId].inGame = false;
    
    // Удаление игры
    activeGames_.erase(winnerId);
    activeGames_.erase(loserId);
}

std::shared_ptr<GameRoom> PlayerManager::getActiveGame(int playerId) const {
    std::shared_lock lock(mutex_);
    auto it = activeGames_.find(playerId);
    return (it != activeGames_.end()) ? it->second : nullptr;
}

bool PlayerManager::isInGame(int playerId) const {
    std::shared_lock lock(mutex_);
    auto it = players_.find(playerId);
    return (it != players_.end()) && it->second.inGame;
}

void PlayerManager::addRating(int playerId, int points) {
    std::unique_lock lock(mutex_);
    players_[playerId].rating += points;
}

} // namespace game