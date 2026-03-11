#ifndef POSITION_HPP
#define POSITION_HPP

struct Position {
    int x;
    int y;
    
    bool isValid() const { return x >= 0 && x < 10 && y >= 0 && y < 15; }
    bool isEnemyBase(int playerId) const { 
        return (playerId == 0 && y == 14) || (playerId == 1 && y == 0); 
    }
};

#endif // POSITION_HPP
