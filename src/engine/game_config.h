#pragma once

#include <string>
#include <vector>

#include "game_state.h"

struct GameConfig {
    GameState::Preset preset;
    GameState::GameFamily family;
    GameState::SpecialRule specialRule;
    GameState::Mode mode;
    GameState::WinRule winRule;
    GameState::StoneType stoneType;
    std::vector<int> heaps;
    int maxTake;
    int poisonHeapIndex;
    int startingPlayer;
    int computerPlayerIndex;

    GameConfig();
};

GameConfig makePresetGame1Config(GameState::Mode mode,
                                 GameState::StoneType stoneType = GameState::StoneType::Coins,
                                 int startingPlayer = 0,
                                 int computerPlayerIndex = 1);

GameConfig makePresetGame2Config(GameState::Mode mode,
                                 GameState::StoneType stoneType = GameState::StoneType::Coins,
                                 int startingPlayer = 0,
                                 int computerPlayerIndex = 1);

GameConfig makePresetGame3Config(GameState::Mode mode,
                                 GameState::StoneType stoneType = GameState::StoneType::Coins,
                                 int startingPlayer = 0,
                                 int computerPlayerIndex = 1);

GameConfig makeDefaultCustomConfig(GameState::Mode mode,
                                   GameState::StoneType stoneType = GameState::StoneType::Coins,
                                   int startingPlayer = 0,
                                   int computerPlayerIndex = 1);

GameConfig makeCustomMultiPileConfig(const std::vector<int>& heaps,
                                     GameState::Mode mode,
                                     GameState::WinRule winRule,
                                     GameState::StoneType stoneType,
                                     int startingPlayer,
                                     int computerPlayerIndex);

GameConfig makeCustomTakeAwayConfig(int totalStones,
                                    int maxTake,
                                    GameState::Mode mode,
                                    GameState::WinRule winRule,
                                    GameState::StoneType stoneType,
                                    int startingPlayer,
                                    int computerPlayerIndex);

bool validateGameConfig(const GameConfig& config, std::string* reason = nullptr);
GameState createGameState(const GameConfig& config);
