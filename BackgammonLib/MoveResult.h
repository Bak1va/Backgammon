#pragma once

enum class MoveResult {
    Success,
    InvalidMove,
    InvalidFromColumn,
    InvalidToColumn,
    NotCurrentPlayer,
    DiceNotRolled,
    NoAvailableMoves,
    CannotBearOff,
    BlockedByOpponent,
    GameNotStarted,
    GameAlreadyOver
};

