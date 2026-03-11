#include "CardsLibrary.hpp"

namespace game {

CardsLibrary::CardsLibrary() {
    initializeCards();
}

CardsLibrary& CardsLibrary::getInstance() {
    static CardsLibrary instance;
    return instance;
}

void CardsLibrary::initializeCards() {
    Card footman(CardType::FOOTMAN, "Footman", CardStats{30, 2, 5, 1.0f, 0, 0, 3});
    cards[CardType::FOOTMAN] = footman;
    
    Card archer(CardType::ARCHER, "Archer", CardStats{20, 3, 4, 1.5f, 1, 0, 2});
    cards[CardType::ARCHER] = archer;
    
    Card knight(CardType::KNIGHT, "Knight", CardStats{50, 4, 7, 0.8f, 0, 0, 4});
    cards[CardType::KNIGHT] = knight;
    
    Card wizard(CardType::WIZARD, "Wizard", CardStats{25, 5, 8, 1.0f, 1, 0, 3});
    wizard.setSplashDamage(true);
    cards[CardType::WIZARD] = wizard;
    
    Card barbarian(CardType::BARBARIAN, "Barbarian", CardStats{40, 5, 12, 1.0f, 0, 0, 4});
    cards[CardType::BARBARIAN] = barbarian;
    
    Card dragon(CardType::DRAGON, "Dragon", CardStats{60, 8, 15, 1.0f, 1, 0, 6});
    dragon.setSplashDamage(true);
    cards[CardType::DRAGON] = dragon;
    
    Card golem(CardType::GOLEM, "Golem", CardStats{120, 7, 8, 0.5f, 0, 10, 8});
    cards[CardType::GOLEM] = golem;
    
    Card assassin(CardType::ASSASSIN, "Assassin", CardStats{15, 4, 10, 1.5f, 0, 0, 2});
    cards[CardType::ASSASSIN] = assassin;
    
    Card bomber(CardType::BOMBER, "Bomber", CardStats{40, 3, 0, 0.0f, 0, 300, 3});
    cards[CardType::BOMBER] = bomber;
}

const Card& CardsLibrary::getCard(CardType type) const {
    return cards.at(type);
}

std::vector<CardType> CardsLibrary::getAllCardTypes() const {
    std::vector<CardType> types;
    for (const auto& [type, _] : cards) {
        types.push_back(type);
    }
    return types;
}

std::vector<Card> CardsLibrary::getStarterDeck() const {
    return {
        getCard(CardType::FOOTMAN),
        getCard(CardType::ARCHER),
        getCard(CardType::KNIGHT),
        getCard(CardType::WIZARD)
    };
}

} // namespace game