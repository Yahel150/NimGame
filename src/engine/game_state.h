#pragma once

#include <string>
#include <vector>

struct GameState {
    enum class Mode {
        PlayerVsPlayer,
        PlayerVsComputer
    };

    enum class WinRule {
        LastStoneWins,
        LastStoneLoses
    };

    enum class GameFamily {
        MultiPile,
        TakeAway
    };

    enum class SpecialRule {
        None,
        PoisonFirstPile
    };

    enum class StoneType {
        Coins,
        Matchsticks,
        Stones
    };

    enum class Preset {
        Custom,
        Game1,
        Game2,
        Game3
    };

    Preset preset;
    std::vector<int> heaps;
    GameFamily family;
    SpecialRule specialRule;
    Mode mode;
    WinRule winRule;
    StoneType stoneType;

    int maxTake;
    int poisonHeapIndex;

    int currentPlayer;
    int computerPlayer;

    int selectedHeap;
    std::vector<int> selectedStones;

    bool gameOver;
    int winner;

    std::string statusMessage;

    GameState();

    void clearSelection();
    bool hasSelection() const;
    int selectedRemoveCount() const;
    bool isSinglePileGame() const;

    void reset(const std::vector<int>& newHeaps,
               GameFamily newFamily,
               SpecialRule newSpecialRule,
               Mode newMode,
               WinRule newWinRule,
               StoneType newStoneType,
               int newMaxTake,
               int newPoisonHeapIndex,
               int startingPlayer,
               int computerPlayerIndex,
               Preset newPreset = Preset::Custom);
};

struct GameSnapshot {
    GameState::Preset preset;
    std::vector<int> heaps;
    GameState::GameFamily family;
    GameState::SpecialRule specialRule;
    GameState::Mode mode;
    GameState::WinRule winRule;
    GameState::StoneType stoneType;
    int maxTake;
    int poisonHeapIndex;
    int currentPlayer;
    int computerPlayer;
    int selectedHeap;
    std::vector<int> selectedStones;
    bool gameOver;
    int winner;
    std::string statusMessage;
};
