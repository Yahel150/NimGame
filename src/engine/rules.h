#pragma once

#include <string>
#include <vector>

#include "game_state.h"

int computeNimSum(const std::vector<int>& heaps);

bool isBoardEmpty(const std::vector<int>& heaps);
bool hasNonPoisonStones(const GameState& state);
bool isOnlyPoisonPileRemaining(const GameState& state);

bool isValidHeapIndex(const GameState& state, int heapIndex);
bool isValidStoneIndex(const GameState& state, int heapIndex, int stoneIndex);
bool isValidRemoveCount(const GameState& state, int heapIndex, int removeCount);
bool isValidMove(const GameState& state, int heapIndex, int removeCount, std::string* reason = nullptr);

bool toggleStoneSelection(GameState& state, int heapIndex, int stoneIndex, std::string* message = nullptr);
bool applySelection(GameState& state, std::string* message = nullptr);

bool applyMove(GameState& state, int heapIndex, int removeCount, std::string* message = nullptr);

bool isComputerTurn(const GameState& state);
bool applyBestComputerMove(GameState& state, std::string* message = nullptr);
