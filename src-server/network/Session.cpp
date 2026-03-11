#include <network/Session.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace game {

namespace json = boost::json;

Session::Session(tcp::socket socket, std::shared_ptr<PlayerManager> playerManager)
    : socket_(std::move(socket))
    , playerManager_(playerManager)
    , gameStateTimer_(socket_.get_executor()) {}

Session::~Session() {
    disconnect();
}

void Session::start() {
    playerId_ = playerManager_->registerPlayer();
    std::cout << "Player " << playerId_ << " connected" << std::endl;
    
    json::object welcome;
    welcome["type"] = "connected";
    welcome["player_id"] = playerId_;
    sendMessage(json::serialize(welcome));
    
    readMessage();
}

void Session::readMessage() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, readBuffer_, '\n',
        [this, self](boost::system::error_code ec, size_t length) {
            if (!ec) {
                std::string data{
                    asio::buffers_begin(readBuffer_.data()),
                    asio::buffers_begin(readBuffer_.data()) + length
                };
                readBuffer_.consume(length);
                
                processMessage(data);
                readMessage();
            } else {
                if (ec != asio::error::eof) {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
                disconnect();
            }
        });
}

void Session::processMessage(const std::string& data) {
    try {
        json::value jv = json::parse(data);
        json::object obj = jv.as_object();
        
        std::string type = obj["type"].as_string().c_str();
        
        if (type == "start_search") {
            handleStartSearch();
        }
        else if (type == "cancel_search") {
            handleCancelSearch();
        }
        else if (type == "place_card") {
            handlePlaceCard(obj);
        }
        else if (type == "get_state") {
            handleGetGameState();
        }
        else if (type == "surrender") {
            handleSurrender();
        }
        else {
            sendError("Unknown command: " + type);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse message: " << e.what() << std::endl;
        sendError("Invalid JSON format");
    }
}

void Session::handleStartSearch() {
    if (!currentGame_) {
        playerManager_->startSearch(playerId_);
        
        json::object response;
        response["type"] = "search_started";
        sendMessage(json::serialize(response));
    } else {
        sendError("Already in game");
    }
}

void Session::handleCancelSearch() {
    playerManager_->cancelSearch(playerId_);
    
    json::object response;
    response["type"] = "search_cancelled";
    sendMessage(json::serialize(response));
}

void Session::handlePlaceCard(const json::object& obj) {
    if (!currentGame_) {
        currentGame_ = playerManager_->getActiveGame(playerId_);
        if (!currentGame_) {
            sendError("Not in game");
            return;
        }
        handleGameStateTimer();
    }
    
    try {
        int cardTypeInt = obj.at("card").as_int64();
        int x = obj.at("x").as_int64();
        
        CardType cardType = static_cast<CardType>(cardTypeInt);
        
        bool success = currentGame_->placeCard(playerId_, cardType, x);
        
        json::object response;
        response["type"] = "place_result";
        response["success"] = success;
        if (!success) {
            response["reason"] = "Invalid placement";
        }
        sendMessage(json::serialize(response));
    }
    catch (const std::exception& e) {
        sendError("Invalid place_card format");
    }
}

void Session::handleGetGameState() {
    if (!currentGame_) {
        currentGame_ = playerManager_->getActiveGame(playerId_);
        if (!currentGame_) {
            sendError("Not in game");
            return;
        }
    }
    
    auto state = currentGame_->getGameState(playerId_);
    sendGameState(state);
}

void Session::handleSurrender() {
    if (currentGame_ && currentGame_->isActive()) {
        json::object response;
        response["type"] = "surrendered";
        sendMessage(json::serialize(response));
        
        // playerManager_->playerSurrendered(playerId_);
    }
    disconnect();
}

void Session::sendGameState(const GameState& state) {
    json::object response;
    response["type"] = "game_state";
    response["your_health"] = state.yourHealth;
    response["enemy_health"] = state.enemyHealth;
    response["your_power"] = state.yourPower;
    
    json::array unitsJson;
    for (const auto& unit : state.units) {
        json::object unitJson;
        unitJson["id"] = unit.id;
        unitJson["type"] = static_cast<int>(unit.card.getType());
        unitJson["name"] = unit.card.getName();
        unitJson["health"] = unit.currentHealth;
        unitJson["max_health"] = unit.card.getStats().health;
        unitJson["x"] = unit.pos.x;
        unitJson["y"] = unit.pos.y;
        unitJson["owner"] = unit.ownerId;
        unitsJson.push_back(unitJson);
    }
    response["units"] = unitsJson;
    
    sendMessage(json::serialize(response));
}

void Session::sendGameEnded(int winnerId, int loserId) {
    json::object response;
    response["type"] = "game_ended";
    response["winner"] = (winnerId == playerId_);
    response["your_id"] = playerId_;
    response["winner_id"] = winnerId;
    response["loser_id"] = loserId;
    sendMessage(json::serialize(response));
    
    currentGame_.reset();
}

void Session::sendError(const std::string& error) {
    json::object response;
    response["type"] = "error";
    response["message"] = error;
    sendMessage(json::serialize(response));
}

void Session::sendMessage(const std::string& message) {
    bool writeInProgress = !writeQueue_.empty();
    writeQueue_.push(message + "\n");
    
    if (!writeInProgress) {
        sendQueuedMessages();
    }
}

void Session::sendQueuedMessages() {
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(writeQueue_.front()),
        [this, self](boost::system::error_code ec, size_t length) {
            if (!ec) {
                writeQueue_.pop();
                if (!writeQueue_.empty()) {
                    sendQueuedMessages();
                }
            } else {
                std::cerr << "Write error: " << ec.message() << std::endl;
                disconnect();
            }
        });
}

void Session::handleGameStateTimer() {
    if (!currentGame_ || !currentGame_->isActive() || !isConnected_) {
        return;
    }
    
    auto state = currentGame_->getGameState(playerId_);
    sendGameState(state);
    
    if (!currentGame_->isActive()) {
        int opponentId = currentGame_->getOpponentId(playerId_);
        sendGameEnded(opponentId, playerId_);
        return;
    }
    
    auto self = shared_from_this();
    gameStateTimer_.expires_after(std::chrono::milliseconds(GAME_STATE_UPDATE_MS));
    gameStateTimer_.async_wait([this, self](boost::system::error_code ec) {
        if (!ec && isConnected_) {
            handleGameStateTimer();
        }
    });
}

void Session::disconnect() {
    if (isConnected_) {
        isConnected_ = false;
        socket_.close();
        
        if (currentGame_) {
            // playerManager_->playerDisconnected(playerId_);
        }
        
        std::cout << "Player " << playerId_ << " disconnected" << std::endl;
    }
}

} // namespace game