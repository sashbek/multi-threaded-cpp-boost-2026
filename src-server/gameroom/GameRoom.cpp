#include <gameroom/GameRoom.hpp>
#include <algorithm>
#include <iostream>

namespace game {

GameRoom::GameRoom(int player1, int player2, boost::asio::io_context& ioContext)
    : players_{player1, player2}
    , gameTickTimer_(ioContext)
    , ioContext_(ioContext) {
    startGameLoop();
}

GameRoom::~GameRoom() {
    gameActive_ = false;
    if (gameLoopThread_ && gameLoopThread_->joinable()) {
        gameLoopThread_->join();
    }
}

void GameRoom::startGameLoop() {
    gameLoopThread_ = std::make_unique<std::thread>([this]() {
        while (gameActive_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_INTERVAL_MS));
            
            boost::asio::post(ioContext_, [this]() {
                if (gameActive_) {
                    updateGame();
                }
            });
        }
    });
}

void GameRoom::updateGame() {
    std::unique_lock lock(mutex_);
    
    if (gameEnded_) return;
    
    tickCounter_++;
    
    if (tickCounter_ % 2 == 0) {
        for (int i = 0; i < 2; ++i) {
            power_[i] = std::min(power_[i] + 1, MAX_POWER);
        }
    }
    
    CardEffects::CombatContext context{units_, power_, tickCounter_};
    
    std::vector<std::pair<int, int>> attacks;
    
    for (size_t i = 0; i < units_.size(); ++i) {
        for (size_t j = i + 1; j < units_.size(); ++j) {
            auto& unit1 = units_[i];
            auto& unit2 = units_[j];
            
            if (unit1.ownerId == unit2.ownerId) continue;
            
            if (areNeighbors(unit1.pos, unit2.pos)) {
                if (CardEffects::canAttack(unit1, unit2)) {
                    attacks.emplace_back(i, j);
                }
                if (CardEffects::canAttack(unit2, unit1)) {
                    attacks.emplace_back(j, i);
                }
            }
        }
    }
    
    for (const auto& [attackerIdx, targetIdx] : attacks) {
        if (units_[attackerIdx].isAlive() && units_[targetIdx].isAlive()) {
            CardEffects::applyAttackEffect(units_[attackerIdx], units_[targetIdx], context);
        }
    }
    
    for (auto& unit : units_) {
        if (unit.isAlive()) {
            CardEffects::applyTickEffects(unit, context);
        }
    }
    
    std::vector<Unit> deadUnits;
    for (const auto& unit : units_) {
        if (!unit.isAlive()) {
            deadUnits.push_back(unit);
        }
    }
    
    for (auto& deadUnit : deadUnits) {
        CardEffects::onDeath(deadUnit, context);
    }
    
    removeDeadUnits();
    
    if (!units_.empty()) {
        std::sort(units_.begin(), units_.end(), 
            [](const Unit& a, const Unit& b) {
                return a.card.getStats().weight > b.card.getStats().weight;
            });
        
        std::vector<bool> unitInCombat(units_.size(), false);
        
        for (size_t i = 0; i < units_.size(); ++i) {
            for (size_t j = i + 1; j < units_.size(); ++j) {
                auto& unit1 = units_[i];
                auto& unit2 = units_[j];
                
                if (unit1.ownerId == unit2.ownerId) continue;
                
                if (areNeighbors(unit1.pos, unit2.pos)) {
                    unitInCombat[i] = true;
                    unitInCombat[j] = true;
                }
            }
        }
        
        for (size_t i = 0; i < units_.size(); ++i) {
            if (unitInCombat[i]) continue;
            
            auto& unit = units_[i];
            int direction = (unit.ownerId == 0) ? 1 : -1;
            Position newPos{unit.pos.x, unit.pos.y + direction};
            
            bool cellOccupied = false;
            for (size_t j = 0; j < units_.size(); ++j) {
                if (i == j) continue;
                if (units_[j].pos.x == newPos.x && units_[j].pos.y == newPos.y) {
                    cellOccupied = true;
                    break;
                }
            }
            
            if (newPos.isValid() && !cellOccupied) {
                unit.pos = newPos;
            }
            
            if (newPos.isEnemyBase(unit.ownerId)) {
                health_[1 - unit.ownerId] -= unit.currentHealth;
                unit.currentHealth = 0;
            }
        }
        
        removeDeadUnits();
    }
    
    checkGameEnd();
}

void GameRoom::removeDeadUnits() {
    units_.erase(
        std::remove_if(units_.begin(), units_.end(),
            [](const Unit& u) { return !u.isAlive(); }),
        units_.end()
    );
}

void GameRoom::checkGameEnd() {
    if (health_[0] <= 0 || health_[1] <= 0) {
        gameEnded_ = true;
        gameActive_ = false;
        
        int winner = (health_[0] > 0) ? players_[0] : players_[1];
        int loser = (health_[0] > 0) ? players_[1] : players_[0];
        
        if (gameEndedCallback_) {
            gameEndedCallback_(winner, loser);
        }
    }
}

bool GameRoom::placeCard(int playerId, CardType cardType, int x) {
    std::unique_lock lock(mutex_);
    
    if (!gameActive_ || gameEnded_) return false;
    
    if (!isValidPlacement(playerId, cardType, x)) {
        return false;
    }
    
    const auto& card = CardsLibrary::getInstance().getCard(cardType);

    if (power_[playerId] < card.getStats().cost) {
        return false;
    }

    Unit unit{
        nextUnitId_++,
        card,
        {x, playerId == 0 ? 0 : FIELD_HEIGHT - 1},
        card.getStats().health,
        playerId
    };
    
    power_[playerId] -= card.getStats().cost;
    
    units_.push_back(unit);
    return true;
}

GameState GameRoom::getGameState(int playerId) const {
    std::shared_lock lock(mutex_);
    GameState state;
    state.yourHealth = health_[playerId];
    state.enemyHealth = health_[1 - playerId];
    state.yourPower = power_[playerId];
    state.units = units_;
    return state;
}

int GameRoom::getOpponentId(int playerId) const {
    return (players_[0] == playerId) ? players_[1] : players_[0];
}

bool GameRoom::isValidPlacement(int playerId, CardType cardType, int x) const {
    if (x < 0 || x >= FIELD_WIDTH) return false;
    
    const auto& card = CardsLibrary::getInstance().getCard(cardType);
    if (power_[playerId] < card.getStats().cost) return false;
    
    int placeY = (playerId == 0) ? 0 : FIELD_HEIGHT - 1;
    for (const auto& unit : units_) {
        if (unit.pos.x == x && unit.pos.y == placeY) {
            return false;
        }
    }
    
    return true;
}

bool GameRoom::areNeighbors(const Position& p1, const Position& p2) const {
    int dx = std::abs(p1.x - p2.x);
    int dy = std::abs(p1.y - p2.y);
    return dx <= 1 && dy <= 1;
}

} // namespace game