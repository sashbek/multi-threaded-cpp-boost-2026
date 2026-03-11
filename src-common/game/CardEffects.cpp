#include <game/CardEffects.hpp>
#include <cmath>
#include <algorithm>

namespace game {

bool CardEffects::canAttack(const Unit& attacker, const Unit& target) {
    int dx = std::abs(attacker.pos.x - target.pos.x);
    int dy = std::abs(attacker.pos.y - target.pos.y);
    int distance = dx + dy;
    
    return distance <= attacker.card.getStats().range;
}

void CardEffects::applyAttackEffect(Unit& attacker, Unit& target, CombatContext& context) {
    int damage = attacker.card.getStats().damage;
    
    target.currentHealth -= damage;
    
    if (attacker.card.hasSplashDamage()) {
        applySplashDamage(attacker, target.pos, context);
    }
}

void CardEffects::applySplashDamage(Unit& attacker, const Position& attackPos,
                                      CombatContext& context) {
    for (auto& unit : context.units) {
        if (unit.ownerId != attacker.ownerId && unit.isAlive()) {
            int dx = std::abs(unit.pos.x - attackPos.x);
            int dy = std::abs(unit.pos.y - attackPos.y);
            
            if (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0)) {
                unit.currentHealth -= attacker.card.getStats().damage / 2;
            }
        }
    }
}

void CardEffects::applyTickEffects(Unit& unit, CombatContext& context) {
    
}

void CardEffects::onDeath(Unit& unit, CombatContext& context) {
    int damage = unit.card.getStats().on_death_damage;
    
    if (damage != 0) {
        for (auto& other : context.units) {
            if (other.ownerId != unit.ownerId && other.isAlive()) {
                int dx = std::abs(other.pos.x - unit.pos.x);
                int dy = std::abs(other.pos.y - unit.pos.y);
                
                if (dx <= 1 && dy <= 1) {
                    other.currentHealth -= damage;
                }
            }
        }
    }
}

} // namespace game