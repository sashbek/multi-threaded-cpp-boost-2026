#ifndef CARDEFFECTS_HPP
#define CARDEFFECTS_HPP

#include <model/Unit.hpp>
#include <vector>
#include <functional>
#include <array>

namespace game {

class CardEffects {
public:
    struct CombatContext {
        std::vector<Unit>& units;
        std::array<int, 2>& playerPower;
        int currentTick;
    };

    static void applyAttackEffect(Unit& attacker, Unit& target, CombatContext& context);
    
    static void applyTickEffects(Unit& unit, CombatContext& context);
    
    static bool canAttack(const Unit& attacker, const Unit& target);
    
    static void onDeath(Unit& unit, CombatContext& context);

private:
    static void applySplashDamage(Unit& attacker, const Position& attackPos, 
                                   CombatContext& context);
    static void applyHealing(Unit& healer, CombatContext& context);
    static void applyPowerGeneration(Unit& generator, CombatContext& context);
};
    
} // namespace game

#endif // CARDEFFECTS_HPP