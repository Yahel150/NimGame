#include "game_config.h"

namespace {

int normalizeComputerPlayer(GameState::Mode mode, int computerPlayerIndex) {
    if (mode == GameState::Mode::PlayerVsComputer) {
        return (computerPlayerIndex == 0) ? 0 : 1;
    }

    return -1;
}

GameConfig makeBaseConfig(GameState::Mode mode,
                          GameState::StoneType stoneType,
                          int startingPlayer,
                          int computerPlayerIndex) {
    GameConfig config;
    config.mode = mode;
    config.stoneType = stoneType;
    config.startingPlayer = startingPlayer;
    config.computerPlayerIndex = normalizeComputerPlayer(mode, computerPlayerIndex);
    return config;
}

}  // namespace

GameConfig::GameConfig()
    : preset(GameState::Preset::Custom),
      family(GameState::GameFamily::MultiPile),
      specialRule(GameState::SpecialRule::None),
      mode(GameState::Mode::PlayerVsPlayer),
      winRule(GameState::WinRule::LastStoneWins),
      stoneType(GameState::StoneType::Coins),
      maxTake(0),
      poisonHeapIndex(-1),
      startingPlayer(0),
      computerPlayerIndex(-1) {
}

GameConfig makePresetGame1Config(GameState::Mode mode,
                                 GameState::StoneType stoneType,
                                 int startingPlayer,
                                 int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.preset = GameState::Preset::Game1;
    config.family = GameState::GameFamily::MultiPile;
    config.specialRule = GameState::SpecialRule::None;
    config.winRule = GameState::WinRule::LastStoneLoses;
    config.heaps = {1, 3, 5, 7};
    return config;
}

GameConfig makePresetGame2Config(GameState::Mode mode,
                                 GameState::StoneType stoneType,
                                 int startingPlayer,
                                 int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.preset = GameState::Preset::Game2;
    config.family = GameState::GameFamily::MultiPile;
    config.specialRule = GameState::SpecialRule::PoisonFirstPile;
    config.winRule = GameState::WinRule::LastStoneWins;
    config.heaps = {1, 2, 3, 4, 5};
    config.poisonHeapIndex = 0;
    return config;
}

GameConfig makePresetGame3Config(GameState::Mode mode,
                                 GameState::StoneType stoneType,
                                 int startingPlayer,
                                 int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.preset = GameState::Preset::Game3;
    config.family = GameState::GameFamily::TakeAway;
    config.specialRule = GameState::SpecialRule::None;
    config.winRule = GameState::WinRule::LastStoneLoses;
    config.heaps = {17};
    config.maxTake = 3;
    return config;
}

GameConfig makeDefaultCustomConfig(GameState::Mode mode,
                                   GameState::StoneType stoneType,
                                   int startingPlayer,
                                   int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.preset = GameState::Preset::Custom;
    config.family = GameState::GameFamily::MultiPile;
    config.specialRule = GameState::SpecialRule::None;
    config.winRule = GameState::WinRule::LastStoneWins;
    config.heaps = {1, 2, 3};
    return config;
}

GameConfig makeCustomMultiPileConfig(const std::vector<int>& heaps,
                                     GameState::Mode mode,
                                     GameState::WinRule winRule,
                                     GameState::StoneType stoneType,
                                     int startingPlayer,
                                     int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.family = GameState::GameFamily::MultiPile;
    config.winRule = winRule;
    config.heaps = heaps;
    return config;
}

GameConfig makeCustomTakeAwayConfig(int totalStones,
                                    int maxTake,
                                    GameState::Mode mode,
                                    GameState::WinRule winRule,
                                    GameState::StoneType stoneType,
                                    int startingPlayer,
                                    int computerPlayerIndex) {
    GameConfig config = makeBaseConfig(mode, stoneType, startingPlayer, computerPlayerIndex);
    config.family = GameState::GameFamily::TakeAway;
    config.winRule = winRule;
    config.heaps = {totalStones};
    config.maxTake = maxTake;
    return config;
}

bool validateGameConfig(const GameConfig& config, std::string* reason) {
    if (config.heaps.empty()) {
        if (reason != nullptr) {
            *reason = "At least one pile is required.";
        }
        return false;
    }

    for (int heap : config.heaps) {
        if (heap <= 0) {
            if (reason != nullptr) {
                *reason = "All piles must start with a positive size.";
            }
            return false;
        }
    }

    if (config.startingPlayer != 0 && config.startingPlayer != 1) {
        if (reason != nullptr) {
            *reason = "Starting player must be 0 or 1.";
        }
        return false;
    }

    if (config.mode == GameState::Mode::PlayerVsComputer) {
        if (config.computerPlayerIndex != 0 && config.computerPlayerIndex != 1) {
            if (reason != nullptr) {
                *reason = "Computer player must be 0 or 1 in player-vs-computer mode.";
            }
            return false;
        }
    } else if (config.computerPlayerIndex != -1) {
        if (reason != nullptr) {
            *reason = "Player-vs-player games cannot assign a computer player.";
        }
        return false;
    }

    if (config.family == GameState::GameFamily::TakeAway) {
        if (config.heaps.size() != 1) {
            if (reason != nullptr) {
                *reason = "Take-away games must use exactly one pile.";
            }
            return false;
        }

        if (config.maxTake <= 0) {
            if (reason != nullptr) {
                *reason = "Take-away games must allow at least one removable stone.";
            }
            return false;
        }
    }

    if (config.specialRule == GameState::SpecialRule::PoisonFirstPile) {
        if (config.family != GameState::GameFamily::MultiPile) {
            if (reason != nullptr) {
                *reason = "The poisoned-pile rule only applies to multi-pile games.";
            }
            return false;
        }

        if (config.poisonHeapIndex < 0 ||
            config.poisonHeapIndex >= static_cast<int>(config.heaps.size())) {
            if (reason != nullptr) {
                *reason = "The poisoned pile index is out of range.";
            }
            return false;
        }
    }

    return true;
}

GameState createGameState(const GameConfig& config) {
    GameState state;
    std::string reason;
    if (!validateGameConfig(config, &reason)) {
        state.statusMessage = reason;
        return state;
    }

    state.reset(config.heaps,
                config.family,
                config.specialRule,
                config.mode,
                config.winRule,
                config.stoneType,
                config.maxTake,
                config.poisonHeapIndex,
                config.startingPlayer,
                config.computerPlayerIndex,
                config.preset);
    return state;
}
