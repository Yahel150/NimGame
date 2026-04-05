#include "game_service.h"

#include <stdexcept>

std::string GameService::createPresetGame(const std::string& preset,
                                          GameState::Mode mode,
                                          GameState::StoneType stoneType,
                                          int startingPlayer) {
    GameConfig config = buildPresetConfig(preset, mode, stoneType, startingPlayer);
    GameState state = createGameState(config);

    std::string gameId = generateGameId();

    Session session;
    session.state = state;
    session.history.clear();

    sessions[gameId] = session;
    return gameId;
}

std::string GameService::createCustomGame(const GameConfig& config) {
    GameState state = createGameState(config);
    std::string gameId = generateGameId();

    Session session;
    session.state = state;
    session.history.clear();

    sessions[gameId] = session;
    return gameId;
}

bool GameService::hasGame(const std::string& gameId) const {
    return sessions.find(gameId) != sessions.end();
}

const GameState* GameService::getGameState(const std::string& gameId) const {
    const Session* session = findSession(gameId);
    if (session == nullptr) {
        return nullptr;
    }
    return &session->state;
}

bool GameService::toggleStone(const std::string& gameId, int heapIndex, int stoneIndex, std::string& message) {
    Session* session = findSession(gameId);
    if (session == nullptr) {
        message = "Game not found.";
        return false;
    }

    return toggleStoneSelection(session->state, heapIndex, stoneIndex, &message);
}

bool GameService::confirmSelection(const std::string& gameId, std::string& message) {
    Session* session = findSession(gameId);
    if (session == nullptr) {
        message = "Game not found.";
        return false;
    }

    session->history.push(session->state);
    return applySelection(session->state, &message);
}

bool GameService::resetSelection(const std::string& gameId, std::string& message) {
    Session* session = findSession(gameId);
    if (session == nullptr) {
        message = "Game not found.";
        return false;
    }

    session->state.clearSelection();
    session->state.statusMessage = "Choose stones from one pile.";
    message = session->state.statusMessage;
    return true;
}

bool GameService::applyComputerMove(const std::string& gameId, Move& move, std::string& message) {
    Session* session = findSession(gameId);
    if (session == nullptr) {
        message = "Game not found.";
        return false;
    }

    if (!isComputerTurn(session->state)) {
        message = "It is not the computer's turn.";
        return false;
    }

    move = chooseBestMove(session->state);
    if (!move.isValid()) {
        message = "The computer has no legal move.";
        return false;
    }

    session->history.push(session->state);
    return applyMove(session->state, move.heapIndex, move.removeCount, &message);
}

GameConfig GameService::buildPresetConfig(const std::string& preset,
                                          GameState::Mode mode,
                                          GameState::StoneType stoneType,
                                          int startingPlayer) const {
    int computerPlayerIndex = (mode == GameState::Mode::PlayerVsComputer) ? 1 : -1;

    if (preset == "game1") {
        return makePresetGame1Config(mode, stoneType, startingPlayer, computerPlayerIndex);
    }

    if (preset == "game2") {
        return makePresetGame2Config(mode, stoneType, startingPlayer, computerPlayerIndex);
    }

    if (preset == "game3") {
        return makePresetGame3Config(mode, stoneType, startingPlayer, computerPlayerIndex);
    }

    if (preset == "custom") {
        return makeDefaultCustomConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    }

    throw std::runtime_error("Unknown preset.");
}

std::string GameService::generateGameId() {
    std::string gameId = "game_" + std::to_string(nextId);
    ++nextId;
    return gameId;
}

GameService::Session* GameService::findSession(const std::string& gameId) {
    auto it = sessions.find(gameId);
    if (it == sessions.end()) {
        return nullptr;
    }
    return &it->second;
}

const GameService::Session* GameService::findSession(const std::string& gameId) const {
    auto it = sessions.find(gameId);
    if (it == sessions.end()) {
        return nullptr;
    }
    return &it->second;
}

bool GameService::undoMove(const std::string& gameId, std::string& message) {
    Session* session = findSession(gameId);
    if (session == nullptr) {
        message = "Game not found.";
        return false;
    }

    if (!session->history.canUndo()) {
        message = "No moves to undo.";
        return false;
    }

    session->history.undo(session->state);

    if (isComputerTurn(session->state) && session->history.canUndo()) {
        session->history.undo(session->state);
    }

    message = "Move undone. Your turn.";
    return true;
}
