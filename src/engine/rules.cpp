#include "rules.h"

#include <algorithm>

#include "strategies.h"

namespace {

void setMessage(std::string* output, const std::string& value) {
    if (output != nullptr) {
        *output = value;
    }
}

std::string buildTurnMessage(const GameState& state) {
    if (state.gameOver) {
        if (state.winner == -1) {
            return "Game over.";
        }

        return "Player " + std::to_string(state.winner + 1) + " wins.";
    }

    if (isComputerTurn(state)) {
        return "Computer is thinking.";
    }

    return "Choose stones from one pile.";
}

bool isPoisonMoveBlocked(const GameState& state, int heapIndex) {
    return state.specialRule == GameState::SpecialRule::PoisonFirstPile &&
           heapIndex == state.poisonHeapIndex &&
           hasNonPoisonStones(state);
}

bool resolvePoisonMove(GameState& state, int heapIndex, std::string* message) {
    if (state.specialRule != GameState::SpecialRule::PoisonFirstPile ||
        heapIndex != state.poisonHeapIndex ||
        state.heaps[heapIndex] != 0) {
        return false;
    }

    state.gameOver = true;
    state.winner = 1 - state.currentPlayer;
    state.clearSelection();
    state.statusMessage = "The poisoned pile was taken. Player " +
        std::to_string(state.currentPlayer + 1) + " loses.";
    setMessage(message, state.statusMessage);
    return true;
}

}  // namespace

int computeNimSum(const std::vector<int>& heaps) {
    int result = 0;
    for (int heap : heaps) {
        result ^= heap;
    }
    return result;
}

bool isBoardEmpty(const std::vector<int>& heaps) {
    for (int heap : heaps) {
        if (heap > 0) {
            return false;
        }
    }
    return true;
}

bool hasNonPoisonStones(const GameState& state) {
    for (int i = 0; i < static_cast<int>(state.heaps.size()); ++i) {
        if (i == state.poisonHeapIndex) {
            continue;
        }

        if (state.heaps[i] > 0) {
            return true;
        }
    }

    return false;
}

bool isOnlyPoisonPileRemaining(const GameState& state) {
    if (state.specialRule != GameState::SpecialRule::PoisonFirstPile ||
        !isValidHeapIndex(state, state.poisonHeapIndex)) {
        return false;
    }

    return state.heaps[state.poisonHeapIndex] > 0 && !hasNonPoisonStones(state);
}

bool isValidHeapIndex(const GameState& state, int heapIndex) {
    return heapIndex >= 0 &&
           heapIndex < static_cast<int>(state.heaps.size()) &&
           state.heaps[heapIndex] > 0;
}

bool isValidStoneIndex(const GameState& state, int heapIndex, int stoneIndex) {
    return isValidHeapIndex(state, heapIndex) &&
           stoneIndex >= 0 &&
           stoneIndex < state.heaps[heapIndex];
}

bool isValidRemoveCount(const GameState& state, int heapIndex, int removeCount) {
    if (!isValidHeapIndex(state, heapIndex) || removeCount <= 0) {
        return false;
    }

    if (removeCount > state.heaps[heapIndex]) {
        return false;
    }

    if (state.family == GameState::GameFamily::TakeAway &&
        state.maxTake > 0 &&
        removeCount > state.maxTake) {
        return false;
    }

    return true;
}

bool isValidMove(const GameState& state, int heapIndex, int removeCount, std::string* reason) {
    if (state.gameOver) {
        setMessage(reason, "The game is already over.");
        return false;
    }

    if (!isValidHeapIndex(state, heapIndex)) {
        setMessage(reason, "Choose a non-empty pile.");
        return false;
    }

    if (!isValidRemoveCount(state, heapIndex, removeCount)) {
        if (state.family == GameState::GameFamily::TakeAway && state.maxTake > 0) {
            setMessage(reason, "You can remove between 1 and " + std::to_string(state.maxTake) + " stones.");
        } else {
            setMessage(reason, "Choose at least one stone from the same pile.");
        }
        return false;
    }

    if (isPoisonMoveBlocked(state, heapIndex)) {
        setMessage(reason, "This stone cannot be touched yet.");
        return false;
    }

    return true;
}

bool toggleStoneSelection(GameState& state, int heapIndex, int stoneIndex, std::string* message) {
    if (state.gameOver) {
        state.statusMessage = "The game is already over.";
        setMessage(message, state.statusMessage);
        return false;
    }

    if (!isValidStoneIndex(state, heapIndex, stoneIndex)) {
        state.statusMessage = "Choose a stone that is still on the board.";
        setMessage(message, state.statusMessage);
        return false;
    }

    if (isPoisonMoveBlocked(state, heapIndex)) {
        state.statusMessage = "This stone cannot be touched yet.";
        setMessage(message, state.statusMessage);
        return false;
    }

    if (state.selectedHeap != -1 && state.selectedHeap != heapIndex) {
        state.statusMessage = "You can choose stones from only one pile.";
        setMessage(message, state.statusMessage);
        return false;
    }

    if (state.selectedHeap == -1) {
        state.selectedHeap = heapIndex;
    }

    std::vector<int>& selected = state.selectedStones;
    std::vector<int>::iterator existing = std::find(selected.begin(), selected.end(), stoneIndex);

    if (existing != selected.end()) {
        selected.erase(existing);
    } else {
        selected.push_back(stoneIndex);
    }

    std::sort(selected.begin(), selected.end());

    if (state.family == GameState::GameFamily::TakeAway &&
        state.maxTake > 0 &&
        state.selectedRemoveCount() > state.maxTake) {
        selected.pop_back();
        state.statusMessage = "You can remove at most " + std::to_string(state.maxTake) + " stones.";
        setMessage(message, state.statusMessage);
        return false;
    }

    if (selected.empty()) {
        state.clearSelection();
        state.statusMessage = "Choose stones from one pile.";
    } else {
        state.statusMessage = "Confirm or reset your selection.";
    }

    setMessage(message, state.statusMessage);
    return true;
}

bool applySelection(GameState& state, std::string* message) {
    if (!state.hasSelection()) {
        state.statusMessage = "Choose at least one stone before confirming.";
        setMessage(message, state.statusMessage);
        return false;
    }

    return applyMove(state, state.selectedHeap, state.selectedRemoveCount(), message);
}

bool applyMove(GameState& state, int heapIndex, int removeCount, std::string* message) {
    std::string reason;
    if (!isValidMove(state, heapIndex, removeCount, &reason)) {
        state.statusMessage = reason;
        setMessage(message, state.statusMessage);
        return false;
    }

    state.heaps[heapIndex] -= removeCount;

    if (resolvePoisonMove(state, heapIndex, message)) {
        return true;
    }

    if (isBoardEmpty(state.heaps)) {
        state.gameOver = true;
        state.winner = (state.winRule == GameState::WinRule::LastStoneWins)
            ? state.currentPlayer
            : 1 - state.currentPlayer;
        state.clearSelection();
        state.statusMessage = buildTurnMessage(state);
        setMessage(message, state.statusMessage);
        return true;
    }

    state.currentPlayer = 1 - state.currentPlayer;
    state.clearSelection();
    state.statusMessage = buildTurnMessage(state);
    setMessage(message, state.statusMessage);
    return true;
}

bool isComputerTurn(const GameState& state) {
    return state.mode == GameState::Mode::PlayerVsComputer &&
           state.currentPlayer == state.computerPlayer;
}

bool applyBestComputerMove(GameState& state, std::string* message) {
    if (!isComputerTurn(state) || state.gameOver) {
        state.statusMessage = buildTurnMessage(state);
        setMessage(message, state.statusMessage);
        return false;
    }

    Move bestMove = chooseBestMove(state);
    if (!bestMove.isValid()) {
        state.gameOver = true;
        state.winner = 1 - state.currentPlayer;
        state.statusMessage = buildTurnMessage(state);
        setMessage(message, state.statusMessage);
        return false;
    }

    return applyMove(state, bestMove.heapIndex, bestMove.removeCount, message);
}

extern "C" {
bool isWinningInitialPosition(GameState::GameFamily family, GameState::WinRule winRule, const std::vector<int>& heaps, int maxTake) {
    if (family == GameState::GameFamily::TakeAway) {
        int n = heaps.empty() ? 0 : heaps[0];
        int k = maxTake;
        if (winRule == GameState::WinRule::LastStoneWins) {
            return (n % (k + 1)) != 0;
        } else {
            return ((n - 1) % (k + 1)) != 0;
        }
    } else {
        int nimSum = computeNimSum(heaps);
        if (winRule == GameState::WinRule::LastStoneWins) {
            return nimSum != 0;
        } else {
            bool hasPileGreaterThanOne = false;
            for (int h : heaps) if (h > 1) { hasPileGreaterThanOne = true; break; }
            return hasPileGreaterThanOne ? (nimSum != 0) : (nimSum == 0);
        }
    }
}
}
