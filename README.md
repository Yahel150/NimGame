# NimGame

NimGame is a browser-based version of Nim. The main goal of the project is to implement an optimal strategy for multiple Nim variants through a simple visual interface. The app also supports playing against another player on the same device.

## Features

- Play against the computer or another player on the same device
- Choose from classical Nim games or create a custom game
- Use different piece styles: coins, matchsticks, or stones
- Learn the optimal moves by observing the computer's optimal strategy

## How to Play

1. Choose the opponent: computer or player vs player.
2. Choose a classical game or create a custom one.
3. Select the piece style.
4. Play by removing pieces according to the rules shown on the screen.

In multi-pile games, you may remove pieces from only one pile per turn.

## Game Modes

- `Game 1`: piles `1, 3, 5, 7`; last move loses
- `Game 2`: piles `1, 2, 3, 4, 5`; taking from the first pile loses
- `Game 3`: one pile of `17`; each turn removes `1`, `2`, or `3`; last move loses
- `Custom`: define your own Nim variant