# NimGame

NimGame is a browser-based version of Nim. The main goal of the project is to implement an optimal strategy for multiple Nim variants through a simple visual interface. The app also supports playing against another player on the same device.

## Video Demo

## How to Play

  ### Choose Opponent

  -Player vs Computer: The computer evaluates the opening position, decides whether to move first, and plays perfectly — guaranteeing a win every game
  
  -Player vs Player: Two players take turns playing against each other on the same device
  
<img width="550" height="550" alt="image" src="https://github.com/user-attachments/assets/bc586092-f57a-4fb0-98b8-da118f0f2501" />

  ### Choose Game Type
  
  -Game 1 — Misère Nim: On each turn, a player removes stones from only one pile. The player who takes the last stone loses
  
  -Game 2 — Nim (Forbidden First Pile): On each turn, a player removes stones from only one pile. The first player to take a stone from the first pile loses immediately

  -Game 3 — Classic Nim (Single Pile): On each turn, a player may remove 1, 2, or 3 stones. The player who takes the last stone loses.

  -Game 4 — Custom Nim: Create your own Nim variant. Set the number of piles, the stones in each pile, removal limits (e.g., 1–3 stones per turn, or any number from a pile), and win/loss condition (normal or misère)


  <img width="550" height="650" alt="image" src="https://github.com/user-attachments/assets/55d705cf-402c-4b50-a85c-f1e89a077dff" />

  ### Choose Stone Type

  -Coins: Use coin‑style pieces during the game.
  
  -Matchsticks: Use matchstick‑style pieces during the game.

  -Stones: Use stone‑style pieces during the game.

  <img width="550" height="650" alt="image" src="https://github.com/user-attachments/assets/ebe61ed5-b116-43ba-968a-31f66aa94c1e" />

  ### Gameplay

  -Rule reminder: On each turn, a player may remove stones from only one pile. The player who takes the last stone loses.
  
  -Turn indicator: Shows whose turn it is
  
  <img width="550" height="550" alt="image" src="https://github.com/user-attachments/assets/a8de424e-ddd3-488a-a7ba-ed932992f2c8" />

  ### selecting a pile

  After selecting a stone to remove, the screen highlights The pile you choose from(you can choose only from this pile).

  #### Buttons
  
  Confirm: Locks in your move. The game then plays an animation showing stones being removed from the pile.

  Reset – Clears your current selection, allowing you to choose a different pile or a different number of stones.

  Skip: After pressing Confirm, a Skip button appears. Tapping it instantly finishes the removal animation.
   
  <img width="550" height="550" alt="image" src="https://github.com/user-attachments/assets/ee3da484-b2ff-4d93-9a49-7801467650e0" />

## CustomNim
Game 4 in Choose Game Type

### Multi-Pile
Piles (comma separated): Enter stone counts for each pile, e.g., 1,3,5,7.

Note: Trailing commas are ignored.

Winning Condition: Choose between:

Last stone loses (misère)

Last stone wins (normal)

<img width="600" height="500" alt="image" src="https://github.com/user-attachments/assets/f06fdb26-7d62-47bb-9439-97e4d87e45b8" />

### Single Pile
Total Stones: Set the number of stones in the single pile (e.g., 21).

Max Stones per turn (k): Maximum stones a player can remove in one turn (e.g., 3, meaning 1–3 stones per turn).

Winning Condition: Same as above (last stone loses/wins).

<img width="600" height="500" alt="image" src="https://github.com/user-attachments/assets/a72b1199-9c1c-4658-9acc-180c369979af" />



  

