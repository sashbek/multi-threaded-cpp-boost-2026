#ifndef UNIT_HPP
#define UNIT_HPP

#include <model/Card.hpp>
#include <model/Position.hpp>

namespace game {

struct Unit {
    int id;
    Card card;
    Position pos;
    int currentHealth;
    int ownerId;
    
    bool isAlive() const { return currentHealth > 0; }
    
    Unit clone() const {
        return Unit{id, card, pos, currentHealth, ownerId};
    }
};

} // namespace game

#endif // UNIT_HPP
