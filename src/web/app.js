  const modePage = document.getElementById("modePage");
  const gameTypePage = document.getElementById("gameTypePage");
  const stonePage = document.getElementById("stonePage");
  const summaryPage = document.getElementById("summaryPage");
  const gamePage = document.getElementById("gamePage");
  const customConfigPage = document.getElementById("customConfigPage");
  const screens = [modePage, gameTypePage, stonePage, summaryPage, gamePage, customConfigPage];

  const gameTypeBackButton = document.getElementById("gameTypeBackButton");
  const stoneBackButton = document.getElementById("stoneBackButton");
  const summaryBackButton = document.getElementById("summaryBackButton");
  const enterGameButton = document.getElementById("enterGameButton");
  const gameBackButton = document.getElementById("gameBackButton");
  const customConfigBackButton = document.getElementById("customConfigBackButton");
  const customConfigNextButton = document.getElementById("customConfigNextButton");

  const customFamilySelect = document.getElementById("customFamily");
  const customHeapsInput = document.getElementById("customHeaps");
  const customTotalStonesInput = document.getElementById("customTotalStones");
  const customMaxTakeInput = document.getElementById("customMaxTake");
  const customWinRuleSelect = document.getElementById("customWinRule");
  const multiPileOptions = document.getElementById("multiPileOptions");
  const takeAwayOptions = document.getElementById("takeAwayOptions");

  const modeButtons = document.querySelectorAll(".mode-button");
  const gameButtons = document.querySelectorAll(".game-button");
  const stoneButtons = document.querySelectorAll(".stone-button");

  const MODE_TO_API = {
    "Player vs Computer": "player_vs_computer",
    "Player vs Player": "player_vs_player"
  };

  const GAME_TO_API = {
    "Game 1": "game1",
    "Game 2": "game2",
    "Game 3": "game3",
    Custom: "custom"
  };

  const STONE_TO_API = {
    Coins: "coins",
    Matchsticks: "matchsticks",
    Stones: "stones"
  };

  const summaryMode = document.getElementById("summaryMode");
  const summaryGame = document.getElementById("summaryGame");
  const summaryStone = document.getElementById("summaryStone");

  const gameBoard = document.getElementById("gameBoard");
  const gameMessage = document.getElementById("gameMessage");
  const gameRuleSummary = document.getElementById("gameRuleSummary");
  const gameTitle = document.getElementById("gameTitle");
  const turnLabel = document.getElementById("turnLabel");

  const confirmMoveButton = document.getElementById("confirmMoveButton");
  const resetMoveButton = document.getElementById("resetMoveButton");
  const skipMoveButton = document.getElementById("skipMoveButton");

  const appState = {
    mode: null,
    gameType: null,
    stoneType: null,
    custom: {
      family: "multi_pile",
      heaps: "1,3,5,7",
      totalStones: 21,
      maxTake: 3,
      winRule: "last_stone_loses"
    }
  };

  const gameClientState = {
    gameId: null,
    serverState: null,
    isAnimating: false,
    isComputerAnimating: false,
    skipComputerAnimation: false,
    skipCurrentAnimation: false
  };

  function ensureWasmReady() {
    return new Promise(function (resolve, reject) {
      if (window.__nimWasmReady && window.Module && typeof window.Module.ccall === "function") {
        resolve();
        return;
      }

      if (window.__nimWasmLoadError) {
        reject(new Error(window.__nimWasmLoadError));
        return;
      }

      if (!window.Module) {
        reject(new Error("The WebAssembly loader was not found. Build dist/nim.js and dist/nim.wasm first."));
        return;
      }

      window.addEventListener(
        "nim-wasm-error",
        function () {
          reject(new Error(window.__nimWasmLoadError || "The WebAssembly files could not be loaded."));
        },
        { once: true }
      );

      window.addEventListener(
        "nim-wasm-ready",
        function () {
          resolve();
        },
        { once: true }
      );
    });
  }

  function showScreen(screen, pushState = true) {
    screens.forEach(s => s.classList.remove("active"));
    screen.classList.add("active");

    if (pushState) {
      const screenId = screen.id;
      history.pushState({ screenId }, "", "#" + screenId);
    }
  }

  window.addEventListener("popstate", function (event) {
    if (event.state && event.state.screenId) {
      const screen = document.getElementById(event.state.screenId);
      if (screen) {
        showScreen(screen, false);
      }
    } else {
      showScreen(modePage, false);
    }
  });

  // Initialize first state
  if (!history.state) {
    history.replaceState({ screenId: "modePage" }, "", "#modePage");
  }

  function updateSummary() {
    summaryMode.textContent = appState.mode || "Not selected";
    summaryGame.textContent = appState.gameType || "Not selected";
    summaryStone.textContent = appState.stoneType || "Not selected";
  }

  function mapModeForApi(modeText) {
    return MODE_TO_API[modeText] || "player_vs_player";
  }

  function mapGameForApi(gameText) {
    return GAME_TO_API[gameText] || "custom";
  }

  function mapStoneForApi(stoneText) {
    return STONE_TO_API[stoneText] || "coins";
  }

  function isComputerTurn(state) {
    return Boolean(
      state &&
      state.mode === "player_vs_computer" &&
      state.currentPlayer === state.computerPlayer &&
      !state.gameOver
    );
  }

  function getRuleTextFromState(state) {
    if (!state) {
      return "";
    }

    if (state.preset === "game1") {
      return `On each turn, a player may remove stones from only one pile.
      The player who takes the last stone loses.`;
    }

    if (state.preset === "game2") {
      return `On each turn, a player may remove stones from only one pile.
      The first player to take a stone from the first pile loses.`;
    }

    if (state.preset === "game3") {
      return `On each turn, a player may remove 1, 2, or 3 stones.
      The player who takes the last stone loses.`;
    }

    const ending =
      state.winRule === "last_stone_wins"
        ? "The player who takes the last stone wins."
        : "The player who takes the last stone loses.";

    if (state.family === "take_away") {
      return `On each turn, a player may remove 1 to ${state.maxTake} stones from the pile. ${ending}`;
    }

    return `On each turn, a player may remove one or more stones from exactly one pile. ${ending}`;
  }

  function getTitleFromState(state) {
    if (!state) {
      return "Game Board";
    }

    if (state.preset === "game1") {
      return "Game 1";
    }

    if (state.preset === "game2") {
      return "Game 2";
    }

    if (state.preset === "game3") {
      return "Game 3";
    }

    return "Custom Game";
  }

  function getTurnLabel(state) {
    if (!state) {
      return "Turn: -";
    }

    if (state.gameOver) {
      if (state.winner === -1) {
        return "Game Over";
      }

      if (state.mode === "player_vs_computer") {
        return state.winner === state.computerPlayer
          ? "Winner: Computer"
          : "Winner: You";
      }

      return "Winner: Player " + (state.winner + 1);
    }

    if (state.mode === "player_vs_computer") {
      if (state.winRule === "last_stone_loses" && getTotalStones(state) === 1) {
        return state.currentPlayer === state.computerPlayer
          ? "Winner: You"
          : "Winner: Computer";
      }

      return state.currentPlayer === state.computerPlayer
        ? "Turn: Computer"
        : "Turn: You";
    }

    return "Turn: Player " + (state.currentPlayer + 1);
  }

  function getTotalStones(state) {
    if (!state || !Array.isArray(state.heaps)) {
      return 0;
    }

    return state.heaps.reduce(function (sum, heap) {
      return sum + heap;
    }, 0);
  }

  function getDisplayMessage(state, fallbackText) {
    if (!state) {
      return fallbackText || "";
    }

    if (state.gameOver) {
      if (state.winner === -1) {
        return "Game over.";
      }

      if (state.mode === "player_vs_computer") {
        return state.winner === state.computerPlayer ? "Computer wins." : "You win.";
      }

      return "Player " + (state.winner + 1) + " wins.";
    }

    if (
      state.mode === "player_vs_computer" &&
      state.winRule === "last_stone_loses" &&
      getTotalStones(state) === 1
    ) {
      return state.currentPlayer === state.computerPlayer
        ? "You win. Computer must take the last stone."
        : "Computer wins. You must take the last stone.";
    }

    return fallbackText || state.statusMessage || "";
  }

  function setMessage(text) {
    gameMessage.classList.remove("fade");
    gameMessage.textContent = text;
  }

  function syncStateAndRender(nextState) {
    if (!nextState) {
      return;
    }

    gameClientState.serverState = nextState;
    renderGame(gameClientState.serverState);
  }

  function updateMessageFromState(fallbackText) {
    setMessage(getDisplayMessage(gameClientState.serverState, fallbackText));
  }

  function toggleCustomFamilyOptions(isMultiPile) {
    multiPileOptions.classList.toggle("is-hidden", !isMultiPile);
    takeAwayOptions.classList.toggle("is-hidden", isMultiPile);
  }

  function validateCustomConfig() {
    if (appState.custom.family === "multi_pile") {
      const piles = appState.custom.heaps.split(",").map(s => parseInt(s.trim(), 10));
      if (piles.some(isNaN) || piles.length === 0) {
        alert("Please enter valid pile sizes (e.g. 1, 3, 5)");
        return false;
      }

      return true;
    }

    if (isNaN(appState.custom.totalStones) || appState.custom.totalStones < 1) {
      alert("Total stones must be at least 1");
      return false;
    }

    if (isNaN(appState.custom.maxTake) || appState.custom.maxTake < 1) {
      alert("Max take (k) must be at least 1");
      return false;
    }

    return true;
  }

  function updateActionButtons() {
    const state = gameClientState.serverState;
    const hasSelection = Boolean(
      state &&
      Array.isArray(state.selectedStones) &&
      state.selectedStones.length > 0
    );
    const canInteract = Boolean(
      state &&
      !state.gameOver &&
      !isComputerTurn(state) &&
      !gameClientState.isAnimating &&
      !gameClientState.isComputerAnimating
    );

    confirmMoveButton.hidden = !(hasSelection && canInteract);
    resetMoveButton.hidden = !(hasSelection && canInteract);
    skipMoveButton.hidden = !(gameClientState.isAnimating || gameClientState.isComputerAnimating);
  }

  function parseWasmResponse(rawText) {
    if (typeof rawText !== "string" || rawText.trim() === "") {
      throw new Error("The WebAssembly module returned an empty response.");
    }

    let data;
    try {
      data = JSON.parse(rawText);
    } catch (error) {
      throw new Error("The WebAssembly module returned invalid JSON.");
    }

    if (!data.ok) {
      throw new Error(data.message || "Request failed.");
    }

    return data;
  }

  function callWasm(functionName, argTypes, args) {
    if (!window.Module || typeof window.Module.ccall !== "function") {
      throw new Error("The WebAssembly module is not ready yet.");
    }

    const rawText = window.Module.ccall(functionName, "string", argTypes, args);
    return parseWasmResponse(rawText);
  }

  async function createGame() {
    const mode = mapModeForApi(appState.mode);
    const stoneType = mapStoneForApi(appState.stoneType);
    let data;

    if (appState.gameType === "Custom") {
      data = callWasm(
        "nim_create_custom_game",
        ["string", "string", "string", "string", "string", "number", "number"],
        [
          appState.custom.family,
          appState.custom.winRule,
          mode,
          stoneType,
          appState.custom.family === "multi_pile" ? appState.custom.heaps : appState.custom.totalStones.toString(),
          appState.custom.maxTake,
          mode === "player_vs_computer" ? -1 : 0
        ]
      );
    } else {
      data = callWasm(
        "nim_create_game",
        ["string", "string", "string", "number"],
        [mapGameForApi(appState.gameType), mode, stoneType, 0]
      );
    }

    gameClientState.gameId = data.gameId;
    gameClientState.serverState = data.state;
  }

  async function selectStone(heapIndex, stoneIndex) {
    const data = callWasm("nim_select_stone", ["number", "number"], [heapIndex, stoneIndex]);
    syncStateAndRender(data.state);
    updateMessageFromState(data.message || "Updated.");
  }

  async function resetSelection() {
    const data = callWasm("nim_reset_selection", [], []);
    syncStateAndRender(data.state);
    updateMessageFromState(data.message || "Selection reset.");
  }

  function createPieceElement(heapIndex, stoneIndex) {
    const piece = document.createElement("button");
    piece.className = "game-piece";
    piece.type = "button";
    piece.dataset.heapIndex = String(heapIndex);
    piece.dataset.stoneIndex = String(stoneIndex);

    const state = gameClientState.serverState;
    const isSelected = Boolean(
      state &&
      state.selectedHeap === heapIndex &&
      Array.isArray(state.selectedStones) &&
      state.selectedStones.includes(stoneIndex)
    );

    if (isSelected) {
      piece.classList.add("selected");
    }

    if (state.stoneType === "coins") {
      piece.classList.add("coin-piece");
      const img = document.createElement("img");
      img.src = "assets/images/coin.png";
      img.alt = "coin";
      piece.appendChild(img);
    } else if (state.stoneType === "matchsticks") {
      piece.classList.add("matchstick-piece");
      const img = document.createElement("img");
      img.src = "assets/images/matchstick.png";
      img.alt = "matchstick";
      piece.appendChild(img);
    } else {
      piece.classList.add("stone-piece");
    }

    piece.addEventListener("click", function () {
      if (
        gameClientState.isAnimating ||
        gameClientState.isComputerAnimating ||
        !gameClientState.serverState ||
        gameClientState.serverState.gameOver ||
        isComputerTurn(gameClientState.serverState)
      ) {
        return;
      }

      selectStone(heapIndex, stoneIndex).catch(function (error) {
        setMessage(error.message);
      });
    });

    return piece;
  }

  function renderPilesBoard(state) {
    gameBoard.className = "game-board piles-layout";
    gameBoard.innerHTML = "";

    state.heaps.forEach(function (count, heapIndex) {
      const row = document.createElement("div");
      row.className = "pile-row";

      if (state.selectedHeap === heapIndex) {
        row.classList.add("active-pile");
      }

      for (let stoneIndex = 0; stoneIndex < count; stoneIndex += 1) {
        row.appendChild(createPieceElement(heapIndex, stoneIndex));
      }

      gameBoard.appendChild(row);
    });
  }

  function renderRowBoard(state) {
    gameBoard.className = "game-board row-layout";
    gameBoard.innerHTML = "";

    const heapIndex = 0;
    const count = state.heaps[heapIndex] || 0;
    const container = document.createElement("div");
    container.className = "pile-row active-pile single-pile-container";

    let stonesLeft = count;
    let currentStoneIndex = 0;

    while (stonesLeft > 0) {
      const subRow = document.createElement("div");
      subRow.className = "single-pile-row";
      const numInRow = Math.min(5, stonesLeft);

      for (let i = 0; i < numInRow; i++) {
        subRow.appendChild(createPieceElement(heapIndex, currentStoneIndex));
        currentStoneIndex++;
        stonesLeft--;
      }

      container.appendChild(subRow);
    }

    gameBoard.appendChild(container);
  }

  function renderGame(state) {
    gameTitle.textContent = getTitleFromState(state);
    gameRuleSummary.textContent = getRuleTextFromState(state);
    turnLabel.textContent = getTurnLabel(state);

    if (state.family === "take_away") {
      renderRowBoard(state);
    } else {
      renderPilesBoard(state);
    }

    updateActionButtons();
  }

  function getSelectedElements(heapIndex, selectedStoneIndices) {
    return selectedStoneIndices
      .slice()
      .sort(function (a, b) {
        return a - b;
      })
      .map(function (stoneIndex) {
        return gameBoard.querySelector(
          '.game-piece[data-heap-index="' +
            heapIndex +
            '"][data-stone-index="' +
            stoneIndex +
            '"]'
        );
      })
      .filter(Boolean);
  }

  function wait(ms) {
    return new Promise(function (resolve) {
      setTimeout(resolve, ms);
    });
  }

  async function waitForSkippableDelay(ms) {
    const step = 25;
    let elapsed = 0;

    while (elapsed < ms) {
      if (gameClientState.skipCurrentAnimation) {
        return;
      }

      const delay = Math.min(step, ms - elapsed);
      await wait(delay);
      elapsed += delay;
    }
  }

  async function animateElementsRemoval(elements) {
    for (let i = 0; i < elements.length; i += 1) {
      if (gameClientState.skipCurrentAnimation) {
        return;
      }

      elements[i].classList.add("removing");
      await waitForSkippableDelay(400);
    }

    await waitForSkippableDelay(600);
  }

  async function confirmSelection() {
    const stateBefore = gameClientState.serverState;
    if (!stateBefore || !stateBefore.selectedStones.length) {
      return;
    }

    gameClientState.isAnimating = true;
    gameClientState.skipCurrentAnimation = false;
    updateActionButtons();

    const selectedHeap = stateBefore.selectedHeap;
    const selectedStones = stateBefore.selectedStones.slice();
    const selectedElements = getSelectedElements(selectedHeap, selectedStones);

    const data = callWasm("nim_confirm_selection", [], []);

    setMessage("Removing selected pieces...");
    await animateElementsRemoval(selectedElements);

    gameClientState.isAnimating = false;
    syncStateAndRender(data.state);
    updateMessageFromState(data.message || "Move confirmed.");

    if (isComputerTurn(gameClientState.serverState)) {
      await wait(800); // Delay before computer move
      runComputerMove();
    }
  }

  function getComputerMoveElements(move) {
    // Use the state *before* computer move was applied to find current pieces on board
    const allInPile = Array.from(
      gameBoard.querySelectorAll(
        '.game-piece[data-heap-index="' + move.heapIndex + '"]'
      )
    );

    // Return last N pieces from the pile (top to bottom in visual)
    return allInPile.slice(-move.removeCount);
  }

  async function runComputerMove() {
    if (!isComputerTurn(gameClientState.serverState)) {
      return;
    }

    gameClientState.isComputerAnimating = true;
    gameClientState.skipComputerAnimation = false;
    gameClientState.skipCurrentAnimation = false;
    updateActionButtons();
    setMessage("Computer is thinking...");

    await waitForSkippableDelay(1200);

    const data = callWasm("nim_computer_move", [], []);

    const move = data.move;
    const elements = getComputerMoveElements(move);

    setMessage("Computer is making a move...");

    if (!gameClientState.skipCurrentAnimation && !gameClientState.skipComputerAnimation) {
      await animateElementsRemoval(elements);
    }

    gameClientState.isComputerAnimating = false;
    gameClientState.skipComputerAnimation = false;
    gameClientState.skipCurrentAnimation = false;
    syncStateAndRender(data.state);
    updateMessageFromState(data.message || "Computer moved.");
  }

  gameTypeBackButton.addEventListener("click", function () {
    showScreen(modePage);
  });

  stoneBackButton.addEventListener("click", function () {
    showScreen(gameTypePage);
  });

  summaryBackButton.addEventListener("click", function () {
    showScreen(stonePage);
  });

  gameBackButton.addEventListener("click", function () {
    showScreen(summaryPage);
  });

  enterGameButton.addEventListener("click", async function () {
    showScreen(gamePage);
    setMessage("Loading game...");

    try {
      await ensureWasmReady();
      await createGame();
      renderGame(gameClientState.serverState);

      const defaultMsg =
        "Choose " + (appState.stoneType ? appState.stoneType.toLowerCase() : "stones") + " from one pile.";
      updateMessageFromState(defaultMsg);

      if (isComputerTurn(gameClientState.serverState)) {
        await wait(300);
        runComputerMove();
      }
    } catch (error) {
      setMessage(error.message);
    }
  });

  modeButtons.forEach(function (button) {
    button.addEventListener("click", function () {
      appState.mode = button.dataset.mode;
      showScreen(gameTypePage);
    });
  });

  gameButtons.forEach(function (button) {
    button.addEventListener("click", function () {
      appState.gameType = button.dataset.game;
      if (appState.gameType === "Custom") {
        showScreen(customConfigPage);
      } else {
        showScreen(stonePage);
      }
    });
  });

  stoneButtons.forEach(function (button) {
    button.addEventListener("click", function () {
      appState.stoneType = button.dataset.stone;
      updateSummary();
      showScreen(summaryPage);
    });
  });

  confirmMoveButton.addEventListener("click", function () {
    confirmSelection().catch(function (error) {
      gameClientState.isAnimating = false;
      gameClientState.skipCurrentAnimation = false;
      renderGame(gameClientState.serverState);
      setMessage(error.message);
    });
  });

  resetMoveButton.addEventListener("click", function () {
    resetSelection().catch(function (error) {
      setMessage(error.message);
    });
  });

  skipMoveButton.addEventListener("click", function () {
    gameClientState.skipCurrentAnimation = true;
    gameClientState.skipComputerAnimation = true;
    setMessage(gameClientState.isComputerAnimating ? "Skipping computer move..." : "Skipping move animation...");
  });

  customFamilySelect.addEventListener("change", function () {
    toggleCustomFamilyOptions(customFamilySelect.value === "multi_pile");
  });

  customConfigBackButton.addEventListener("click", function () {
    showScreen(gameTypePage);
  });

  customConfigNextButton.addEventListener("click", function () {
    appState.custom.family = customFamilySelect.value;
    appState.custom.winRule = customWinRuleSelect.value;
    appState.custom.heaps = customHeapsInput.value;
    appState.custom.totalStones = parseInt(customTotalStonesInput.value, 10);
    appState.custom.maxTake = parseInt(customMaxTakeInput.value, 10);

    if (!validateCustomConfig()) {
      return;
    }

    showScreen(stonePage);
  });

  toggleCustomFamilyOptions(appState.custom.family === "multi_pile");
  updateActionButtons();
