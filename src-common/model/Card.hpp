#ifndef CARD_HPP
#define CARD_HPP

#include <string>
#include <vector>
#include <functional>

namespace game {

enum class CardType {
    FOOTMAN = 0,
    ARCHER,
    KNIGHT,
    WIZARD,
    BARBARIAN,
    DRAGON,
    GOLEM,
    ASSASSIN,
    BOMBER,
};

struct CardStats {
    int health;
    int cost;
    int damage;
    float attack_speed; // cnt per second
    int range;
    int on_death_amage;
    int weight;
};

class Card {
public:
    Card() = default;
    Card(CardType type_, const std::string& name_, const CardStats& stats_)
        : type(type_), name(name_), stats(stats_) { }
    
    CardType getType() const { return type; }
    const std::string& getName() const { return name; }
    const CardStats& getStats() const { return stats; }
    
    bool hasSplashDamage() const { return splash_damage; }
    
    void setSplashDamage(bool value) { splash_damage = value; }
    
private:
    CardType type;
    std::string name;
    CardStats stats;
    
    bool splash_damage{false};
};

} // namespace game

#endif // CARD_HPP