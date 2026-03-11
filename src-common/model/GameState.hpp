#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <model/Unit.hpp>
#include <boost/json.hpp>
#include <vector>
#include <array>

namespace game {

struct GameState {
    int yourHealth;
    int enemyHealth;
    int yourPower;
    std::vector<Unit> units;
    
    boost::json::object toJson() const {
        boost::json::object obj;
        obj["your_health"] = yourHealth;
        obj["enemy_health"] = enemyHealth;
        obj["your_power"] = yourPower;
        
        boost::json::array unitsJson;
        for (const auto& unit : units) {
            boost::json::object unitObj;
            unitObj["id"] = unit.id;
            unitObj["type"] = static_cast<int>(unit.card.getType());
            unitObj["name"] = unit.card.getName();
            unitObj["health"] = unit.currentHealth;
            unitObj["max_health"] = unit.card.getStats().health;
            unitObj["x"] = unit.pos.x;
            unitObj["y"] = unit.pos.y;
            unitObj["owner"] = unit.ownerId;
            unitsJson.push_back(unitObj);
        }
        obj["units"] = unitsJson;
        
        return obj;
    }
};

} // namespace game

#endif // GAMESTATE_HPP
