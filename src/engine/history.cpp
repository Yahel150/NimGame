#include "history.h"

GameHistory::GameHistory() {
}

void GameHistory::clear() {
    snapshots.clear();
}

void GameHistory::push(const GameState& state) {
    GameSnapshot snapshot;
    snapshot.preset = state.preset;
    snapshot.heaps = state.heaps;
    snapshot.family = state.family;
    snapshot.specialRule = state.specialRule;
    snapshot.mode = state.mode;
    snapshot.winRule = state.winRule;
    snapshot.stoneType = state.stoneType;
    snapshot.maxTake = state.maxTake;
    snapshot.poisonHeapIndex = state.poisonHeapIndex;
    snapshot.currentPlayer = state.currentPlayer;
    snapshot.computerPlayer = state.computerPlayer;
    snapshot.selectedHeap = state.selectedHeap;
    snapshot.selectedStones = state.selectedStones;
    snapshot.gameOver = state.gameOver;
    snapshot.winner = state.winner;
    snapshot.statusMessage = state.statusMessage;

    snapshots.push_back(snapshot);
}

bool GameHistory::canUndo() const {
    return !snapshots.empty();
}

bool GameHistory::undo(GameState& state) {
    if (!canUndo()) {
        return false;
    }

    GameSnapshot snapshot = snapshots.back();
    snapshots.pop_back();

    state.preset = snapshot.preset;
    state.heaps = snapshot.heaps;
    state.family = snapshot.family;
    state.specialRule = snapshot.specialRule;
    state.mode = snapshot.mode;
    state.winRule = snapshot.winRule;
    state.stoneType = snapshot.stoneType;
    state.maxTake = snapshot.maxTake;
    state.poisonHeapIndex = snapshot.poisonHeapIndex;
    state.currentPlayer = snapshot.currentPlayer;
    state.computerPlayer = snapshot.computerPlayer;
    state.selectedHeap = snapshot.selectedHeap;
    state.selectedStones = snapshot.selectedStones;
    state.gameOver = snapshot.gameOver;
    state.winner = snapshot.winner;
    state.statusMessage = snapshot.statusMessage;

    return true;
}
