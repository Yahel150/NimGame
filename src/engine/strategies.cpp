#include "strategies.h"

#include <vector>

namespace {

int computeXorSum(const std::vector<int>& heaps) {
    int result = 0;
    for (int heap : heaps) {
        result ^= heap;
    }
    return result;
}

Move chooseFallbackMove(const std::vector<int>& heaps, int heapOffset = 0, int removeCount = 1) {
    for (int i = 0; i < static_cast<int>(heaps.size()); ++i) {
        if (heaps[i] > 0) {
            return Move(i + heapOffset, (heaps[i] < removeCount) ? heaps[i] : removeCount);
        }
    }

    return Move();
}

Move chooseStandardNimMove(const std::vector<int>& heaps, int heapOffset = 0) {
    const int nimSum = computeXorSum(heaps);
    if (nimSum != 0) {
        for (int i = 0; i < static_cast<int>(heaps.size()); ++i) {
            const int target = heaps[i] ^ nimSum;
            if (target < heaps[i]) {
                return Move(i + heapOffset, heaps[i] - target);
            }
        }
    }

    return chooseFallbackMove(heaps, heapOffset);
}

}  // namespace

Move::Move(int heapIndexValue, int removeCountValue)
    : heapIndex(heapIndexValue),
      removeCount(removeCountValue) {
}

bool Move::isValid() const {
    return heapIndex >= 0 && removeCount > 0;
}

Move chooseBestMove(const GameState& state) {
    if (state.family == GameState::GameFamily::TakeAway) {
        return chooseBestMoveTakeAway(state);
    }

    if (state.specialRule == GameState::SpecialRule::PoisonFirstPile) {
        return chooseBestMovePoisonFirstPile(state);
    }

    if (state.winRule == GameState::WinRule::LastStoneWins) {
        return chooseBestMoveMultiPileLastStoneWins(state);
    }

    return chooseBestMoveMultiPileLastStoneLoses(state);
}

Move chooseBestMoveMultiPileLastStoneWins(const GameState& state) {
    return chooseStandardNimMove(state.heaps);
}

Move chooseBestMoveMultiPileLastStoneLoses(const GameState& state) {
    int onesCount = 0;
    int bigCount = 0;
    int bigIndex = -1;

    for (int i = 0; i < static_cast<int>(state.heaps.size()); ++i) {
        if (state.heaps[i] == 1) {
            ++onesCount;
        } else if (state.heaps[i] > 1) {
            ++bigCount;
            bigIndex = i;
        }
    }

    if (bigCount == 0) {
        return chooseFallbackMove(state.heaps);
    }

    if (bigCount == 1) {
        if (onesCount % 2 == 0) {
            return Move(bigIndex, state.heaps[bigIndex] - 1);
        }

        return Move(bigIndex, state.heaps[bigIndex]);
    }

    return chooseStandardNimMove(state.heaps);
}

Move chooseBestMovePoisonFirstPile(const GameState& state) {
    std::vector<int> safeHeaps;
    safeHeaps.reserve(state.heaps.size());

    for (int i = 0; i < static_cast<int>(state.heaps.size()); ++i) {
        if (i == state.poisonHeapIndex) {
            continue;
        }

        safeHeaps.push_back(state.heaps[i]);
    }

    bool hasSafeHeap = false;
    for (int heap : safeHeaps) {
        if (heap > 0) {
            hasSafeHeap = true;
            break;
        }
    }

    if (!hasSafeHeap) {
        if (state.poisonHeapIndex >= 0 &&
            state.poisonHeapIndex < static_cast<int>(state.heaps.size()) &&
            state.heaps[state.poisonHeapIndex] > 0) {
            return Move(state.poisonHeapIndex, 1);
        }

        return Move();
    }

    const int nimSum = computeXorSum(safeHeaps);
    if (nimSum != 0) {
        for (int i = 0, safeIndex = 0; i < static_cast<int>(state.heaps.size()); ++i) {
            if (i == state.poisonHeapIndex) {
                continue;
            }

            const int target = safeHeaps[safeIndex] ^ nimSum;
            if (target < safeHeaps[safeIndex]) {
                return Move(i, safeHeaps[safeIndex] - target);
            }

            ++safeIndex;
        }
    }

    for (int i = 0; i < static_cast<int>(state.heaps.size()); ++i) {
        if (i != state.poisonHeapIndex && state.heaps[i] > 0) {
            return Move(i, 1);
        }
    }

    return Move();
}

Move chooseBestMoveTakeAway(const GameState& state) {
    if (state.heaps.empty() || state.heaps[0] <= 0) {
        return Move();
    }

    const int remaining = state.heaps[0];
    const int moduloBase = state.maxTake + 1;

    if (state.winRule == GameState::WinRule::LastStoneWins) {
        int removeCount = remaining % moduloBase;
        if (removeCount == 0) {
            removeCount = 1;
        }

        return Move(0, removeCount);
    }

    int removeCount = (remaining - 1) % moduloBase;
    if (removeCount == 0) {
        removeCount = 1;
    }

    if (removeCount > state.maxTake) {
        removeCount = state.maxTake;
    }

    return Move(0, removeCount);
}
