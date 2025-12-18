#include <algorithm>
#include <random>
#include <cmath>
#include "Game.hpp"
#include "RollOpeningDiceCommand.hpp"

constexpr uint32_t OFF_BOARD_COLOR_WHITE = 24;
constexpr uint32_t OFF_BOARD_COLOR_BLACK = -1;

Game::Game()
    : m_phase(GamePhase::NOT_STARTED), m_currentPlayer(Color::WHITE), m_dice{ 0, 0 }, m_diceRolled(false),
      m_openingDiceWhite(0), m_openingDiceBlack(0) {
}

Game::~Game() {
}

void Game::start() {
    m_board = Board(); // reset board
    m_phase = GamePhase::OPENING_ROLL_WHITE;
    m_currentPlayer = Color::WHITE;
    m_dice[0] = m_dice[1] = 0;
    m_diceRolled = false;
    m_openingDiceWhite = 0;
    m_openingDiceBlack = 0;
    notifyGameStarted();
}

GamePhase Game::getPhase() const {
    return m_phase;
}

Color Game::getCurrentPlayer() const {
    return m_currentPlayer;
}

void Game::rollDice() {
    if (m_phase != GamePhase::IN_PROGRESS) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 6);
    m_dice[0] = dist(gen);
    m_dice[1] = dist(gen);
    m_diceRolled = true;
    notifyDiceRolled();
}

const std::array<int, 2> Game::getDice() const {
    return m_dice;
}

int Game::playerIndex(Color player) const {
    return (player == Color::WHITE) ? 0 : 1;
}

bool Game::hasAllPiecesHome(Color player) const {
    if (m_board.getBarCount(playerIndex(player)) > 0) return false;

    if (player == Color::WHITE) {
        for (int i = 0; i <= 17; ++i) {
            if (m_board.getColumn(i).getColor() == Color::WHITE &&
                m_board.getColumn(i).getPieceCount() > 0) {
                return false;
            }
        }
    }
    else {
        for (int i = 6; i <= 23; ++i) {
            if (m_board.getColumn(i).getColor() == Color::BLACK &&
                m_board.getColumn(i).getPieceCount() > 0) {
                return false;
            }
        }
    }
    return true;
}

bool Game::canBearOff(Color player) const
{
    if (!hasAllPiecesHome(player)) return false;
    return true;
}

bool Game::isHomeBoard(int index, Color player) const {
    if (player == Color::WHITE) return index >= 18 && index <= 23;
    return index >= 0 && index <= 5;
}

bool Game::hasMovesAvailable() const {
    if (!m_diceRolled) return false;

    int pIndex = playerIndex(m_currentPlayer);

    if (m_board.getBarCount(pIndex) > 0) {
        int entryDice[] = { m_dice[0], m_dice[1] };
        for (int d : entryDice) {
            if (d == 0) continue;
            int entryIndex = (m_currentPlayer == Color::WHITE) ? (d - 1) : (24 - d);
            if (!isMoveBlocked(entryIndex, m_currentPlayer)) return true;
        }
        return false;
    }

    for (int i = 0; i < 24; ++i) {
        if (m_board.getColumn(i).getPieceCount() > 0 && m_board.getColumn(i).getColor() == m_currentPlayer) {
            int distances[] = { m_dice[0], m_dice[1] };
            for (int d : distances) {
                if (d == 0) continue;

                int target = (m_currentPlayer == Color::WHITE) ? i + d : i - d;

                bool offBoardLimit = (m_currentPlayer == Color::WHITE && target > 23) || (m_currentPlayer == Color::BLACK && target < 0);

                if (offBoardLimit) {
                    if (hasAllPiecesHome(m_currentPlayer)) {
                        int distToEdge = (m_currentPlayer == Color::WHITE) ? (24 - i) : (i + 1);
                        if (d == distToEdge) return true;
                        if (d > distToEdge) {
                            bool furthiests = true;
                            if (m_currentPlayer == Color::WHITE) {
                                for (int k = 18; k < i; ++k) {
                                    if (m_board.getColumn(k).getColor() == Color::WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                                        furthiests = false; break;
                                    }
                                }
                            }
                            else {
                                for (int k = i + 1; k <= 5; ++k) {
                                    if (m_board.getColumn(k).getColor() == Color::BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                                        furthiests = false; break;
                                    }
                                }
                            }
                            if (furthiests) return true;
                        }
                    }
                }
                else if (target >= 0 && target < 24) {
                    if (!isMoveBlocked(target, m_currentPlayer)) return true;
                }
            }
        }
    }
    return false;
}

bool Game::canSelectPoint(int index) const {
    if (m_phase != GamePhase::IN_PROGRESS) return false;
    if (!m_diceRolled) return false;

    int pIndex = playerIndex(m_currentPlayer);

    if (m_board.getBarCount(pIndex) > 0) {
        return index == BAR_INDEX;
    }

    if (index == BAR_INDEX) return false;
    if (index < 0 || index >= 24) return false;
    const Column& col = m_board.getColumn(index);
    if (col.getPieceCount() == 0) return false;
    if (col.getColor() != m_currentPlayer) return false;

    return true;
}

std::vector<int> Game::getLegalTargets(int fromIndex) const {
    std::vector<int> targets;
    if (!m_diceRolled) return targets;
    if (!canSelectPoint(fromIndex)) return targets;

    int pIndex = playerIndex(m_currentPlayer);
    int dirs[2] = { m_dice[0], m_dice[1] };

    if (fromIndex == BAR_INDEX) {
        for (int d : dirs) {
            if (d <= 0) continue;
            int entryIndex = (m_currentPlayer == Color::WHITE) ? (d - 1) : (24 - d);

            if (!isMoveBlocked(entryIndex, m_currentPlayer)) {
                if (std::find(targets.begin(), targets.end(), entryIndex) == targets.end()) {
                    targets.push_back(entryIndex);
                }
            }
        }
        return targets;
    }

    for (int d : dirs) {
        if (d <= 0) continue;

        int toIndex = (m_currentPlayer == Color::WHITE) ? fromIndex + d : fromIndex - d;

        bool isOffBoard = (m_currentPlayer == Color::WHITE && toIndex > 23) || (m_currentPlayer == Color::BLACK && toIndex < 0);

        if (isOffBoard) {
            if (hasAllPiecesHome(m_currentPlayer)) {
                int distToEdge = (m_currentPlayer == Color::WHITE) ? (24 - fromIndex) : (fromIndex + 1);

                bool canBearOff = false;
                if (d == distToEdge) {
                    canBearOff = true;
                }
                else if (d > distToEdge) {
                    bool isFurthest = true;
                    if (m_currentPlayer == Color::WHITE) {
                        for (int k = 18; k < fromIndex; ++k) {
                            if (m_board.getColumn(k).getColor() == Color::WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                                isFurthest = false; break;
                            }
                        }
                    }
                    else {
                        for (int k = fromIndex + 1; k <= 5; ++k) {
                            if (m_board.getColumn(k).getColor() == Color::BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                                isFurthest = false; break;
                            }
                        }
                    }
                    if (isFurthest) canBearOff = true;
                }

                if (canBearOff) {
                    int offTarget = (m_currentPlayer == Color::WHITE) ? OFF_BOARD_COLOR_WHITE : OFF_BOARD_COLOR_BLACK;
                    if (std::find(targets.begin(), targets.end(), offTarget) == targets.end()) {
                        targets.push_back(offTarget);
                    }
                }
            }
            continue;
        }

        if (toIndex < 0 || toIndex >= 24) continue;

        if (isMoveBlocked(toIndex, m_currentPlayer)) {
            continue;
        }

        if (std::find(targets.begin(), targets.end(), toIndex) == targets.end()) {
            targets.push_back(toIndex);
        }
    }

    return targets;
}

MoveResult Game::makeMove(int fromIndex, int toIndex) {
    if (m_phase != GamePhase::IN_PROGRESS) return MoveResult::GAME_NOT_STARTED;
    if (!m_diceRolled) return MoveResult::DICE_NOT_ROLLED;

    int pIndex = playerIndex(m_currentPlayer);

    if (fromIndex == BAR_INDEX) {
        if (m_board.getBarCount(pIndex) == 0) return MoveResult::INVALID_MOVE;

        int dieUsed = 0;
        if (m_currentPlayer == Color::WHITE) dieUsed = toIndex + 1;
        else dieUsed = 24 - toIndex;

        int dieIdx = -1;
        if (m_dice[0] == dieUsed) dieIdx = 0;
        else if (m_dice[1] == dieUsed) dieIdx = 1;

        if (dieIdx == -1) return MoveResult::INVALID_MOVE;
        if (isMoveBlocked(toIndex, m_currentPlayer)) return MoveResult::BLOCKED_BY_OPPONENT;

        if (canHit(toIndex, m_currentPlayer)) {
            Column& toCol = m_board.getColumn(toIndex);
            toCol.removePiece();
            m_board.incrementBarCount(playerIndex(m_currentPlayer == Color::WHITE ? Color::BLACK : Color::WHITE));
        }

        m_board.decrementBarCount(pIndex);
        m_board.getColumn(toIndex).addPiece(m_currentPlayer);
        m_dice[dieIdx] = 0;
    }

    else {

        if (fromIndex < 0 || fromIndex >= 24) return MoveResult::INVALID_FROM_COLUMN;
        if (m_board.getBarCount(pIndex) > 0) return MoveResult::INVALID_MOVE; // Trebuie intai scos de pe bara

        Column& fromCol = m_board.getColumn(fromIndex);
        if (fromCol.getPieceCount() == 0 || fromCol.getColor() != m_currentPlayer) return MoveResult::INVALID_MOVE;

        bool isBearingOff = (toIndex == OFF_BOARD_COLOR_WHITE) || (toIndex == OFF_BOARD_COLOR_BLACK);

        if (isBearingOff) {
            if (!hasAllPiecesHome(m_currentPlayer)) return MoveResult::CANNOT_BEAR_OFF;
            if ((m_currentPlayer == Color::WHITE && toIndex != OFF_BOARD_COLOR_WHITE) ||
                (m_currentPlayer == Color::BLACK && toIndex != OFF_BOARD_COLOR_BLACK)) return MoveResult::INVALID_MOVE;

            int distToEdge = (m_currentPlayer == Color::WHITE) ? (24 - fromIndex) : (fromIndex + 1);

            int dieIdx = -1;

            if (m_dice[0] == distToEdge) dieIdx = 0;
            else if (m_dice[1] == distToEdge) dieIdx = 1;

            if (dieIdx == -1) {
                bool isFurthest = true;
                if (m_currentPlayer == Color::WHITE) {
                    for (int k = 18; k < fromIndex; ++k) {
                        if (m_board.getColumn(k).getColor() == Color::WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                            isFurthest = false; break;
                        }
                    }
                }
                else {
                    for (int k = fromIndex + 1; k <= 5; ++k) {
                        if (m_board.getColumn(k).getColor() == Color::BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                            isFurthest = false; break;
                        }
                    }
                }

                if (isFurthest) {
                    if (m_dice[0] > distToEdge) dieIdx = 0;
                    else if (m_dice[1] > distToEdge) dieIdx = 1;
                }
            }

            if (dieIdx == -1) return MoveResult::INVALID_MOVE;

            fromCol.removePiece();
            m_board.incrementBorneOffCount(pIndex);
            m_dice[dieIdx] = 0;

            notifyMoveMade(fromIndex, toIndex, MoveResult::SUCCESS);

            if (m_board.getBorneOffCount(pIndex) == 15) {
                m_phase = GamePhase::FINISHED;
                notifyGameFinished(m_currentPlayer);
                return MoveResult::SUCCESS;
            }

        }
        else {
            if (toIndex < 0 || toIndex >= 24) return MoveResult::INVALID_TO_COLUMN;

            int distance = std::abs(toIndex - fromIndex);
            int dieIdx = -1;
            if (m_dice[0] == distance) dieIdx = 0;
            else if (m_dice[1] == distance) dieIdx = 1;

            if (dieIdx == -1) return MoveResult::INVALID_MOVE;
            if (isMoveBlocked(toIndex, m_currentPlayer)) return MoveResult::BLOCKED_BY_OPPONENT;

            if (canHit(toIndex, m_currentPlayer)) {
                Column& toCol = m_board.getColumn(toIndex);
                toCol.removePiece();
                m_board.incrementBarCount(playerIndex(m_currentPlayer == Color::WHITE ? Color::BLACK : Color::WHITE));
            }

            fromCol.removePiece();
            m_board.getColumn(toIndex).addPiece(m_currentPlayer);
            m_dice[dieIdx] = 0;
        }
    }

    notifyMoveMade(fromIndex, toIndex, MoveResult::SUCCESS);

    if (m_dice[0] == 0 && m_dice[1] == 0) {
        m_diceRolled = false;
        switchTurn();
    }
    else if (!hasMovesAvailable()) {
        m_diceRolled = false;
        switchTurn();
    }

    return MoveResult::SUCCESS;
}

// ... restul metodelor (getters, observers) raman identice cu fisierul anterior ...
int Game::getColumnCount(int index) const { return m_board.getColumn(index).getPieceCount(); }
Color Game::getColumnColor(int index) const { return m_board.getColumn(index).getColor(); }
int Game::getBarCount(Color player) const { return m_board.getBarCount(playerIndex(player)); }
int Game::getBorneOffCount(Color player) const { return m_board.getBorneOffCount(playerIndex(player)); }

GameStateDTO Game::getState() const {
    GameStateDTO s;
    for (int i = 0; i < 24; ++i) {
        const Column& col = m_board.getColumn(i);
        s.pieceCounts[i] = col.getPieceCount();
        s.colors[i] = col.getColor();
    }
    s.barWhite = m_board.getBarCount(0);
    s.barBlack = m_board.getBarCount(1);
    s.borneOffWhite = m_board.getBorneOffCount(0);
    s.borneOffBlack = m_board.getBorneOffCount(1);
    s.currentPlayer = m_currentPlayer;
    s.dice1 = m_dice[0];
    s.dice2 = m_dice[1];
    s.openingDiceWhite = m_openingDiceWhite;
    s.openingDiceBlack = m_openingDiceBlack;
    return s;
}

void Game::addObserver(IGameObserver* observer) {
    if (!observer) return;
    if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
        m_observers.push_back(observer);
    }
}

void Game::removeObserver(IGameObserver* observer) {
    auto it = std::remove(m_observers.begin(), m_observers.end(), observer);
    m_observers.erase(it, m_observers.end());
}

void Game::notifyGameStarted() { for (auto* o : m_observers) o->onGameStarted(); }
void Game::notifyDiceRolled() { for (auto* o : m_observers) o->onDiceRolled(m_currentPlayer, m_dice[0], m_dice[1]); }
void Game::notifyMoveMade(int fromIndex, int toIndex, MoveResult result) { for (auto* o : m_observers) o->onMoveMade(m_currentPlayer, fromIndex, toIndex, result); }
void Game::notifyTurnChanged() { for (auto* o : m_observers) o->onTurnChanged(m_currentPlayer); }
void Game::notifyGameFinished(Color winner) { for (auto* o : m_observers) o->onGameFinished(winner); }

void Game::switchTurn() {
    m_currentPlayer = (m_currentPlayer == Color::WHITE) ? Color::BLACK : Color::WHITE;
    notifyTurnChanged();
}

bool Game::isMoveBlocked(int toIndex, Color player) const {
    const Column& toCol = m_board.getColumn(toIndex);
    if (toCol.getPieceCount() >= 2 && toCol.getColor() != player) return true;
    return false;
}

bool Game::canHit(int toIndex, Color player) const {
    const Column& toCol = m_board.getColumn(toIndex);
    return toCol.getPieceCount() == 1 && toCol.getColor() != player && toCol.getColor() != Color::NONE;
}

int Game::rollSingleDie() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 6);
    return dist(gen);
}

void Game::rollOpeningDice() {
    auto rollFunc = [this]() { return rollSingleDie(); };

    if (m_phase == GamePhase::OPENING_ROLL_WHITE) {
        RollOpeningDiceCommand rollWhiteCmd(rollFunc, Color::WHITE, m_openingDiceWhite);
        rollWhiteCmd.execute();

        m_dice[0] = m_openingDiceWhite;
        m_dice[1] = 0;

        m_phase = GamePhase::OPENING_ROLL_BLACK;
        m_currentPlayer = Color::BLACK;
        notifyDiceRolled();
        return;
    }

    if (m_phase == GamePhase::OPENING_ROLL_BLACK) {
        RollOpeningDiceCommand rollBlackCmd(rollFunc, Color::BLACK, m_openingDiceBlack);
        rollBlackCmd.execute();

        if (m_openingDiceWhite == m_openingDiceBlack) {
            m_phase = GamePhase::OPENING_ROLL_COMPARE;
            m_dice[0] = m_openingDiceWhite;
            m_dice[1] = m_openingDiceBlack;
            m_currentPlayer = Color::WHITE;
            notifyDiceRolled();
            return;
        }

        if (m_openingDiceWhite > m_openingDiceBlack) {
            m_currentPlayer = Color::WHITE;
        } else {
            m_currentPlayer = Color::BLACK;
        }

        m_phase = GamePhase::OPENING_ROLL_COMPARE;
        m_dice[0] = m_openingDiceWhite;
        m_dice[1] = m_openingDiceBlack;
        m_diceRolled = false;
        notifyDiceRolled();
        return;
    }
}

int Game::getOpeningDiceWhite() const {
    return m_openingDiceWhite;
}

int Game::getOpeningDiceBlack() const {
    return m_openingDiceBlack;
}

void Game::startGameAfterOpening() {
    if (m_phase == GamePhase::OPENING_ROLL_COMPARE) {
        m_phase = GamePhase::IN_PROGRESS;
        m_dice[0] = 0;
        m_dice[1] = 0;
        m_diceRolled = false;
        notifyTurnChanged();
    }
}

