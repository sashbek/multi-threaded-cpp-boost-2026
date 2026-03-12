#include <gtest/gtest.h>
#include "src-server/gameroom/GameRoom.hpp"
#include <game/CardsLibrary.hpp>
#include <model/Card.hpp>
#include <thread>
#include <chrono>
#include <iostream>

using namespace game;

class GameRoomTest : public ::testing::Test {
protected:
    void SetUp() override {
        ioContext = std::make_unique<boost::asio::io_context>();
        gameRoom = std::make_unique<GameRoom>(1, 2, *ioContext);
    }

    std::unique_ptr<boost::asio::io_context> ioContext;
    std::unique_ptr<GameRoom> gameRoom;
};

void printGameField(const GameState& state, int playerId) {
    const int WIDTH = 10;
    const int HEIGHT = 15;
    
    std::cout << "\n=== Game Field ===\n";
    std::cout << "Your Health: " << state.yourHealth 
              << " | Enemy Health: " << state.enemyHealth
              << " | Your Power: " << state.yourPower << "\n\n";
    
    std::vector<std::vector<char>> field(HEIGHT, std::vector<char>(WIDTH, '.'));
    
    for (const auto& unit : state.units) {
        if (unit.pos.y >= 0 && unit.pos.y < HEIGHT && 
            unit.pos.x >= 0 && unit.pos.x < WIDTH) {
            
            char symbol;
            switch (unit.card.getType()) {
                case CardType::FOOTMAN: symbol = 'f'; break;
                case CardType::ARCHER: symbol = 'a'; break;
                case CardType::KNIGHT: symbol = 'k'; break;
                case CardType::WIZARD: symbol = 'w'; break;
                case CardType::BARBARIAN: symbol = 'b'; break;
                case CardType::DRAGON: symbol = 'd'; break;
                case CardType::GOLEM: symbol = 'g'; break;
                case CardType::ASSASSIN: symbol = 's'; break;
                case CardType::BOMBER: symbol = 'o'; break;
                default: symbol = '?'; break;
            }
            
            if (unit.ownerId == playerId) {
                symbol = toupper(symbol);
            }
            
            field[unit.pos.y][unit.pos.x] = symbol;
        }
    }
    
    std::cout << "   ";
    for (int x = 0; x < WIDTH; x++) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    
    for (int y = HEIGHT - 1; y >= 0; y--) {
        std::cout << (y < 10 ? " " : "") << y << " ";
        for (int x = 0; x < WIDTH; x++) {
            std::cout << field[y][x] << " ";
        }
        
        if (y == 0) std::cout << "  ← Your base";
        if (y == HEIGHT - 1) std::cout << "  ← Enemy base";
        
        std::cout << "\n";
    }
    
    std::cout << "\nLegend: ";
    std::cout << "Uppercase = Your units, ";
    std::cout << "Lowercase = Enemy units\n";
    std::cout << "f=Footman a=Archer k=Knight w=Wizard ";
    std::cout << "b=Barbarian d=Dragon g=Golem s=Assassin o=Bomber\n";
}

void printUnitDetails(const GameState& state) {
    std::cout << "\n=== Unit Details ===\n";
    
    std::map<int, std::vector<Unit>> unitsByOwner;
    for (const auto& unit : state.units) {
        unitsByOwner[unit.ownerId].push_back(unit);
    }
    
    for (const auto& [ownerId, units] : unitsByOwner) {
        std::cout << "Player " << ownerId << " units:\n";
        for (const auto& unit : units) {
            std::cout << "  " << unit.card.getName() 
                      << " [ID:" << unit.id 
                      << " HP:" << unit.currentHealth << "/" << unit.card.getStats().health
                      << " Pos:(" << unit.pos.x << "," << unit.pos.y << ")]\n";
        }
    }
}


TEST_F(GameRoomTest, CardPlacement) {
    bool result = gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    EXPECT_TRUE(result);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 1);
    EXPECT_EQ(state.yourPower, 3);
}

TEST_F(GameRoomTest, InvalidPlacementOutOfBounds) {
    bool result = gameRoom->placeCard(1, CardType::FOOTMAN, 15);
    EXPECT_FALSE(result);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 0);
    EXPECT_EQ(state.yourPower, 5);
}

TEST_F(GameRoomTest, InvalidPlacementNotEnoughPower) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 6);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.yourPower, 1);
    
    bool result = gameRoom->placeCard(1, CardType::FOOTMAN, 7);
    EXPECT_FALSE(result);
}

TEST_F(GameRoomTest, CannotPlaceOnOccupiedCell) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    bool result = gameRoom->placeCard(1, CardType::ARCHER, 5);
    EXPECT_FALSE(result);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 1);
}

TEST_F(GameRoomTest, UnitMovement) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    
    for (int i = 0; i < 30; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(0);
    EXPECT_LT(state.yourHealth, 10);
}

TEST_F(GameRoomTest, CombatBetweenUnits) {
    gameRoom->placeCard(0, CardType::FOOTMAN, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    
    for (int i = 0; i < 20; i++) {
        gameRoom->updateGame();
        //auto st = gameRoom->getGameState(0);
        //printGameField(st, 0);
        //printUnitDetails(st);
    }
    
    auto state = gameRoom->getGameState(0);
    bool allDead = true;
    for (const auto& unit : state.units) {
        if (unit.currentHealth > 0) {
            allDead = false;
            break;
        }
    }
    EXPECT_TRUE(allDead || state.units.empty());
}

TEST_F(GameRoomTest, SplashDamage) {
    gameRoom->placeCard(0, CardType::WIZARD, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    //gameRoom->updateGame();
    gameRoom->placeCard(1, CardType::FOOTMAN, 4);
    
    for (int i = 0; i < 8; i++) {
        gameRoom->updateGame();
        //auto st = gameRoom->getGameState(0);
        //printGameField(st, 0);
        //printUnitDetails(st);
    }
    
    auto state = gameRoom->getGameState(0);
    int damagedUnits = 0;
    for (const auto& unit : state.units) {
        if (unit.ownerId == 1 && unit.currentHealth < unit.card.getStats().health) {
            damagedUnits++;
        }
    }
    EXPECT_EQ(damagedUnits, 2);
}

TEST_F(GameRoomTest, PowerRegeneration) {
    int initialPower = gameRoom->getGameState(0).yourPower;
    
    for (int i = 0; i < 2; i++) {
        gameRoom->updateGame();
    }
    
    int newPower = gameRoom->getGameState(0).yourPower;
    EXPECT_EQ(newPower, initialPower + 1);
}

TEST_F(GameRoomTest, UnitReachesEnemyBase) {
    gameRoom->placeCard(0, CardType::FOOTMAN, 5);
    
    for (int i = 0; i < 20; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.yourHealth, -20);
}

TEST_F(GameRoomTest, GameEndsWhenBaseDestroyed) {
    for (int i = 0; i < 10; i++) {
        gameRoom->placeCard(1, CardType::FOOTMAN, i % 10);
    }
    
    for (int i = 0; i < 30; i++) {
        gameRoom->updateGame();
        if (!gameRoom->isActive()) break;
    }
    
    EXPECT_FALSE(gameRoom->isActive());
    auto state = gameRoom->getGameState(0);
    EXPECT_LE(state.yourHealth, 0);
}

TEST_F(GameRoomTest, MultipleUnitsSameOwner) {
    for (int i = 0; i < 30; i++) {
        gameRoom->updateGame();
    }

    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 6);
    gameRoom->placeCard(1, CardType::FOOTMAN, 7);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 3);
    EXPECT_EQ(state.yourPower, 4);
}

TEST_F(GameRoomTest, UnitsDontAttackAllies) {
    for (int i = 0; i < 30; i++) {
        gameRoom->updateGame();
    }

    gameRoom->placeCard(1, CardType::BARBARIAN, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 6);
    
    gameRoom->updateGame();
    
    auto state = gameRoom->getGameState(1);
    int totalHealth = 0;
    for (const auto& unit : state.units) {
        totalHealth += unit.currentHealth;
    }
    EXPECT_EQ(totalHealth, 70);
}

TEST_F(GameRoomTest, GolemOnDeathDamage) {
    gameRoom->placeCard(1, CardType::GOLEM, 5);
    gameRoom->placeCard(2, CardType::FOOTMAN, 4);
    gameRoom->placeCard(2, CardType::FOOTMAN, 6);
    
    for (int i = 0; i < 20; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(2);
    bool golemDead = true;
    for (const auto& unit : state.units) {
        if (unit.card.getType() == CardType::GOLEM && unit.currentHealth > 0) {
            golemDead = false;
            break;
        }
    }
    EXPECT_TRUE(golemDead || state.units.empty());
}

TEST_F(GameRoomTest, BomberNoAttackButDies) {
    gameRoom->placeCard(1, CardType::BOMBER, 5);
    gameRoom->placeCard(0, CardType::FOOTMAN, 5);
    
    for (int i = 0; i < 10; i++) {
        gameRoom->updateGame();
        //auto st = gameRoom->getGameState(0);
        //printGameField(st, 0);
        //printUnitDetails(st);
    }
    
    auto state = gameRoom->getGameState(1);
    bool bomberExists = false;
    for (const auto& unit : state.units) {
        if (unit.card.getType() == CardType::BOMBER) {
            bomberExists = true;
            break;
        }
    }
    EXPECT_FALSE(bomberExists);
}

TEST_F(GameRoomTest, DifferentUnitsHaveDifferentCosts) {
    for (int i = 0; i < 10; i++) {
        gameRoom->updateGame();
        //auto st = gameRoom->getGameState(0);
        //printGameField(st, 0);
        //printUnitDetails(st);
    }
    bool footmanPlaced = gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    bool archerPlaced = gameRoom->placeCard(1, CardType::ARCHER, 6);
    bool knightPlaced = gameRoom->placeCard(1, CardType::KNIGHT, 7);
    
    EXPECT_TRUE(footmanPlaced);
    EXPECT_TRUE(archerPlaced);
    EXPECT_TRUE(knightPlaced);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.yourPower, 1);
}

TEST_F(GameRoomTest, PowerDoesNotExceedMaximum) {
    for (int i = 0; i < 20; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(1);
    EXPECT_LE(state.yourPower, 10);
}

TEST_F(GameRoomTest, HeavyUnitsMoveSameSpeed) {
    gameRoom->placeCard(1, CardType::GOLEM, 5);
    gameRoom->placeCard(1, CardType::FOOTMAN, 6);
    
    auto initialState = gameRoom->getGameState(1);
    int initialY = -1;
    for (const auto& unit : initialState.units) {
        if (unit.card.getType() == CardType::GOLEM) {
            initialY = unit.pos.y;
            break;
        }
    }
    
    for (int i = 0; i < 2; i++) {
        gameRoom->updateGame();
    }
    
    auto finalState = gameRoom->getGameState(1);
    for (const auto& unit : finalState.units) {
        if (unit.card.getType() == CardType::GOLEM) {
            EXPECT_EQ(unit.pos.y, initialY + 2);
            break;
        }
    }
}

TEST_F(GameRoomTest, MultipleTicksWithNoActions) {
    for (int i = 0; i < 10; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 0);
    EXPECT_EQ(state.yourHealth, 10);
    EXPECT_EQ(state.enemyHealth, 10);
}

TEST_F(GameRoomTest, BothPlayersPlaceUnits) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    gameRoom->placeCard(0, CardType::ARCHER, 5);
    
    auto state = gameRoom->getGameState(1);
    EXPECT_EQ(state.units.size(), 2);
    
    bool hasPlayer1Unit = false;
    bool hasPlayer2Unit = false;
    for (const auto& unit : state.units) {
        if (unit.ownerId == 1) hasPlayer1Unit = true;
        if (unit.ownerId == 0) hasPlayer2Unit = true;
    }
    EXPECT_TRUE(hasPlayer1Unit);
    EXPECT_TRUE(hasPlayer2Unit);
}

TEST_F(GameRoomTest, UnitsStopExistingAfterDeath) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    gameRoom->placeCard(0, CardType::BARBARIAN, 5);
    
    for (int i = 0; i < 10; i++) {
        gameRoom->updateGame();
    }
    
    auto state = gameRoom->getGameState(1);
    int aliveCount = 0;
    for (const auto& unit : state.units) {
        if (unit.currentHealth > 0) aliveCount++;
    }
    EXPECT_LE(aliveCount, 1);
}

TEST_F(GameRoomTest, GameStateForDifferentPlayers) {
    gameRoom->placeCard(1, CardType::FOOTMAN, 5);
    gameRoom->placeCard(0, CardType::ARCHER, 5);
    
    auto state1 = gameRoom->getGameState(1);
    auto state2 = gameRoom->getGameState(0);
    
    EXPECT_EQ(state1.yourHealth, state2.enemyHealth);
    EXPECT_EQ(state1.enemyHealth, state2.yourHealth);
    EXPECT_EQ(state1.yourPower, 3);
    EXPECT_EQ(state2.yourPower, 2);
}
