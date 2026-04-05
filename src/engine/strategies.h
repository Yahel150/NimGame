#pragma once

#include "game_state.h"

struct Move {
    int heapIndex;
    int removeCount;

    Move(int heapIndex = -1, int removeCount = 0);

    bool isValid() const;
};

Move chooseBestMove(const GameState& state);
Move chooseBestMoveMultiPileLastStoneWins(const GameState& state);
Move chooseBestMoveMultiPileLastStoneLoses(const GameState& state);
Move chooseBestMovePoisonFirstPile(const GameState& state);
Move chooseBestMoveTakeAway(const GameState& state);
