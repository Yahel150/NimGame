#pragma once

#include <string>
#include <unordered_map>

#include "game_config.h"
#include "game_state.h"
#include "history.h"
#include "rules.h"
#include "strategies.h"

class GameService {
public:
    struct Session {
        GameState state;
        GameHistory history;
    };

    std::string createPresetGame(const std::string& preset,
                                 GameState::Mode mode,
                                 GameState::StoneType stoneType,
                                 int startingPlayer = 0);

    std::string createCustomGame(const GameConfig& config);

    bool hasGame(const std::string& gameId) const;
    const GameState* getGameState(const std::string& gameId) const;

    bool toggleStone(const std::string& gameId, int heapIndex, int stoneIndex, std::string& message);
    bool confirmSelection(const std::string& gameId, std::string& message);
    bool resetSelection(const std::string& gameId, std::string& message);
    bool applyComputerMove(const std::string& gameId, Move& move, std::string& message);
    bool undoMove(const std::string& gameId, std::string& message);

private:
    std::unordered_map<std::string, Session> sessions;
    int nextId = 1;

    GameConfig buildPresetConfig(const std::string& preset,
                                 GameState::Mode mode,
                                 GameState::StoneType stoneType,
                                 int startingPlayer) const;

    std::string generateGameId();
    Session* findSession(const std::string& gameId);
    const Session* findSession(const std::string& gameId) const;
};
