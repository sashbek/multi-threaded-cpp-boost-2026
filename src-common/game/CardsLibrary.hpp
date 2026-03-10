#ifndef CARDSLIBRARY_HPP
#define CARDSLIBRARY_HPP

#include <common/model/Card.hpp>
#include <unordered_map>
#include <memory>

namespace game {

class CardsLibrary {
public:
    static CardsLibrary& getInstance();
    
    const Card& getCard(CardType type) const;
    std::vector<CardType> getAllCardTypes() const;
    std::vector<Card> getStarterDeck() const;
    
private:
    CardsLibrary();
    void initializeCards();
    
    std::unordered_map<CardType, Card> cards;
};

} // namespace game

#endif // CARDSLIBRARY_HPP