#include "serialization.h"

#include <sstream>

namespace {

std::string escapeJson(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (char ch : value) {
        switch (ch) {
            case '\\':
                result += "\\\\";
                break;
            case '"':
                result += "\\\"";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += ch;
                break;
        }
    }

    return result;
}

template <typename T>
void appendVector(std::ostringstream& output, const std::vector<T>& values) {
    output << "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            output << ",";
        }

        output << values[i];
    }
    output << "]";
}

}  // namespace

const char* toString(GameState::Preset preset) {
    switch (preset) {
        case GameState::Preset::Game1:
            return "game1";
        case GameState::Preset::Game2:
            return "game2";
        case GameState::Preset::Game3:
            return "game3";
        case GameState::Preset::Custom:
        default:
            return "custom";
    }
}

const char* toString(GameState::Mode mode) {
    switch (mode) {
        case GameState::Mode::PlayerVsComputer:
            return "player_vs_computer";
        case GameState::Mode::PlayerVsPlayer:
        default:
            return "player_vs_player";
    }
}

const char* toString(GameState::WinRule winRule) {
    switch (winRule) {
        case GameState::WinRule::LastStoneLoses:
            return "last_stone_loses";
        case GameState::WinRule::LastStoneWins:
        default:
            return "last_stone_wins";
    }
}

const char* toString(GameState::GameFamily family) {
    switch (family) {
        case GameState::GameFamily::TakeAway:
            return "take_away";
        case GameState::GameFamily::MultiPile:
        default:
            return "multi_pile";
    }
}

const char* toString(GameState::SpecialRule specialRule) {
    switch (specialRule) {
        case GameState::SpecialRule::PoisonFirstPile:
            return "poison_first_pile";
        case GameState::SpecialRule::None:
        default:
            return "none";
    }
}

const char* toString(GameState::StoneType stoneType) {
    switch (stoneType) {
        case GameState::StoneType::Matchsticks:
            return "matchsticks";
        case GameState::StoneType::Stones:
            return "stones";
        case GameState::StoneType::Coins:
        default:
            return "coins";
    }
}

std::string serializeGameState(const GameState& state) {
    std::ostringstream output;
    output << "{";
    output << "\"preset\":\"" << toString(state.preset) << "\",";
    output << "\"family\":\"" << toString(state.family) << "\",";
    output << "\"specialRule\":\"" << toString(state.specialRule) << "\",";
    output << "\"mode\":\"" << toString(state.mode) << "\",";
    output << "\"winRule\":\"" << toString(state.winRule) << "\",";
    output << "\"stoneType\":\"" << toString(state.stoneType) << "\",";
    output << "\"maxTake\":" << state.maxTake << ",";
    output << "\"poisonHeapIndex\":" << state.poisonHeapIndex << ",";
    output << "\"currentPlayer\":" << state.currentPlayer << ",";
    output << "\"computerPlayer\":" << state.computerPlayer << ",";
    output << "\"selectedHeap\":" << state.selectedHeap << ",";
    output << "\"selectedStones\":";
    appendVector(output, state.selectedStones);
    output << ",";
    output << "\"heaps\":";
    appendVector(output, state.heaps);
    output << ",";
    output << "\"gameOver\":" << (state.gameOver ? "true" : "false") << ",";
    output << "\"winner\":" << state.winner << ",";
    output << "\"statusMessage\":\"" << escapeJson(state.statusMessage) << "\"";
    output << "}";
    return output.str();
}

std::string serializeGameConfig(const GameConfig& config) {
    std::ostringstream output;
    output << "{";
    output << "\"preset\":\"" << toString(config.preset) << "\",";
    output << "\"family\":\"" << toString(config.family) << "\",";
    output << "\"specialRule\":\"" << toString(config.specialRule) << "\",";
    output << "\"mode\":\"" << toString(config.mode) << "\",";
    output << "\"winRule\":\"" << toString(config.winRule) << "\",";
    output << "\"stoneType\":\"" << toString(config.stoneType) << "\",";
    output << "\"maxTake\":" << config.maxTake << ",";
    output << "\"poisonHeapIndex\":" << config.poisonHeapIndex << ",";
    output << "\"startingPlayer\":" << config.startingPlayer << ",";
    output << "\"computerPlayerIndex\":" << config.computerPlayerIndex << ",";
    output << "\"heaps\":";
    appendVector(output, config.heaps);
    output << "}";
    return output.str();
}

std::string serializeMove(const Move& move) {
    std::ostringstream output;
    output << "{";
    output << "\"heapIndex\":" << move.heapIndex << ",";
    output << "\"removeCount\":" << move.removeCount << ",";
    output << "\"valid\":" << (move.isValid() ? "true" : "false");
    output << "}";
    return output.str();
}
