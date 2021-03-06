open EggTypes;

/* basically, do we need to smash the block below? */
let checkTileBelowPlayer = (player: player, board: board): board => {
  if (player.falling === false) {
    board;
  } else {

    let belowCoords = {
      ...player.coords,
      y: player.coords.y + 1
    };

    let tile = Board.getTile(belowCoords.x, belowCoords.y, board);

    if (tile.breakable === true) {
      Board.changeTileByID(board, belowCoords.x, belowCoords.y, 1);
    } else {
      board;
    };
  };
};

let playerNotReadyToCheck = (player: player): bool => (
    player.coords.offsetX !== 0 ||
    player.coords.offsetY !== 0 ||
    player.moved === false
);

type tileReturn = { outcome: string, board: board, score: int };

let checkPlayerTileAction = (
  player: player,
  board: board,
  score: int,
  outcome: string
): tileReturn => {
  if (playerNotReadyToCheck(player)) {
    {
      board,
      outcome,
      score
    };
  } else {
    let tile = Board.getTile(player.coords.x, player.coords.y, board);
    switch (tile.action) {
      | Collectable(a) => {
        let newScore = a * player.multiplier;
        let newTile = {
          ...Tiles.blankTile,
          x: player.coords.x,
          y: player.coords.y
        };

        {
          board: Board.changeTile(board, newTile),
          outcome,
          score: score + newScore
        };
      }
      | CompleteLevel =>  {
        board,
        outcome: "completeLevel",
        score
      }
      | PinkSwitch =>  {
        board: EggMap.switchTiles(15, 16, board),
        outcome,
        score
      }
      | GreenSwitch => {
        board: EggMap.switchTiles(18, 19, board),
          outcome,
          score
      }
      | _ => {
        board,
        outcome,
        score
      }
    };
  };
};

let tileActionReduce = (player: player, currentGameState: gameState): gameState => {
  let updated = checkPlayerTileAction(
    player,
    currentGameState.board,
    currentGameState.score,
    currentGameState.outcome
  );

  let postCrateBoard = checkTileBelowPlayer(player, updated.board);
  
  {
    ...currentGameState,
    board: postCrateBoard,
    outcome: updated.outcome,
    score: updated.score
  };
};

let checkAllPlayerTileActions = (gameState: gameState): gameState => 
  List.fold_right(tileActionReduce,gameState.players, gameState);
