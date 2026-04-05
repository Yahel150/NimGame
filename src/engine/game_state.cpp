#include "game_state.h"

GameState::GameState()
    : preset(Preset::Custom),
      family(GameFamily::MultiPile),
      specialRule(SpecialRule::None),
      mode(Mode::PlayerVsPlayer),
      winRule(WinRule::LastStoneWins),
      stoneType(StoneType::Coins),
      maxTake(0),
      poisonHeapIndex(-1),
      currentPlayer(0),
      computerPlayer(-1),
      selectedHeap(-1),
      gameOver(false),
      winner(-1),
      statusMessage("Choose stones from one pile.") {
}

void GameState::clearSelection() {
    selectedHeap = -1;
    selectedStones.clear();
}

bool GameState::hasSelection() const {
    return selectedHeap != -1 && !selectedStones.empty();
}

int GameState::selectedRemoveCount() const {
    return static_cast<int>(selectedStones.size());
}

bool GameState::isSinglePileGame() const {
    return family == GameFamily::TakeAway;
}

void GameState::reset(const std::vector<int>& newHeaps,
                      GameFamily newFamily,
                      SpecialRule newSpecialRule,
                      Mode newMode,
                      WinRule newWinRule,
                      StoneType newStoneType,
                      int newMaxTake,
                      int newPoisonHeapIndex,
                      int startingPlayer,
                      int computerPlayerIndex,
                      Preset newPreset) {
    preset = newPreset;
    heaps = newHeaps;
    family = newFamily;
    specialRule = newSpecialRule;
    mode = newMode;
    winRule = newWinRule;
    stoneType = newStoneType;
    maxTake = newMaxTake;
    poisonHeapIndex = newPoisonHeapIndex;
    currentPlayer = startingPlayer;
    computerPlayer = computerPlayerIndex;
    clearSelection();
    gameOver = false;
    winner = -1;
    statusMessage = "Choose stones from one pile.";
}
