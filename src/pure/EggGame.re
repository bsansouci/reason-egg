open EggTypes;

open Reprocessing;

let playerIsValid = (player: player): bool => player.value > 0;

let countPlayers = (players: list(player)): int => {
  List.length(List.filter(playerIsValid, players));
};

/* get total outstanding points left to grab on board */
let countCollectable = (board: board): int => {
  List.fold_right((tile: tile, score: int) => {
    switch (tile.action) {
    | Collectable(x) => score + x
    | _ => score
    };
  }, Board.getBoardTiles(board), 0);
};

/* check leftovers on board and whether player is over finish tile */
let checkLevelIsCompleted = (gameState: gameState): bool => {
  let collectable = countCollectable(gameState.board);
  let playerCount: int = countPlayers(gameState.players);
  (collectable < 1 && playerCount < 2);
};

let makeRainbowEgg = (player: player) : player => {
  let maybeNewPlayer = Player.getPlayerByType(RainbowEgg);
  switch (maybeNewPlayer) {
    | Some(rainbowEgg) => {
      {
        ...player,
        frames: rainbowEgg.frames,
        filename: rainbowEgg.filename,
        multiplier: rainbowEgg.multiplier,
        playerType: rainbowEgg.playerType,
        value: rainbowEgg.value
      };
    }
    | _ => player
  };
};

let checkNearlyFinished = (
  gameState: gameState
): gameState => {
  let newPlayers = if (checkLevelIsCompleted(gameState)) {
    List.map((player: player) => {
      (player.value > 0) ? makeRainbowEgg(player) : player;
    }, gameState.players);
  } else {
    gameState.players;
  };
  {
    ...gameState,
    players: newPlayers
  };
};


/* this rotates board and players it DOES NOT do animation - not our problem */
let doRotate = (gameState: gameState, clockwise: bool): gameState => {
  let rotations = gameState.rotations + 1;

  let boardSize = List.length(gameState.board);

  let newBoard = EggMap.rotateBoard(gameState.board, clockwise);

  let rotatedPlayers = List.map(player => {
    EggMap.rotatePlayer(boardSize, player, clockwise);
  }, gameState.players);

  let rotateAngle: float = EggMap.changeRenderAngle(
    gameState.rotateAngle,
    clockwise
  );

  {
    ...gameState,
    board: newBoard,
    players: rotatedPlayers,
    rotateAngle,
    rotations,
    gameAction: Playing
  };
};

let resetOutcome = (gameState: gameState) : gameState => {
  ...gameState,
    outcome: ""
};

let doGameMove = (gameState: gameState, timePassed: int): gameState => {
  resetOutcome(gameState)
  |> EggMovement.doCalcs(timePassed)
  |> EggAction.checkAllPlayerTileActions
  |> EggCollisions.checkAll
  |> BoardCollisions.checkAll
  |> checkNearlyFinished
};

let doAction = (
  gameState: gameState,
  timePassed: int
): gameState => {
  switch (gameState.gameAction) {
  | TurnLeft => doRotate(gameState,false)
  | TurnRight => doRotate(gameState, true)
  | Playing => doGameMove(gameState, timePassed)
  | _ => gameState
  };
};

let processRotate = (gameState: gameState, keyCode: Reprocessing_Events.keycodeT): gameState => {
  (gameState.gameAction === Playing) ? 
    {
      ...gameState,
      gameAction: switch (keyCode) {
        | Left => RotatingLeft(0)
        | Right => RotatingRight(0)
        | _ => gameState.gameAction
        }
    } : gameState;
};