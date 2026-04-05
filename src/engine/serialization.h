#pragma once

#include <string>

#include "game_config.h"
#include "game_state.h"
#include "strategies.h"

const char* toString(GameState::Preset preset);
const char* toString(GameState::Mode mode);
const char* toString(GameState::WinRule winRule);
const char* toString(GameState::GameFamily family);
const char* toString(GameState::SpecialRule specialRule);
const char* toString(GameState::StoneType stoneType);

std::string serializeGameState(const GameState& state);
std::string serializeGameConfig(const GameConfig& config);
std::string serializeMove(const Move& move);
