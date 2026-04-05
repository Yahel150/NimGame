#pragma once

#include "game_state.h"

class GameHistory {
public:
    GameHistory();

    void clear();
    void push(const GameState& state);
    bool canUndo() const;
    bool undo(GameState& state);

private:
    std::vector<GameSnapshot> snapshots;
};