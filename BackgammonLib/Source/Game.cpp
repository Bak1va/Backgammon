#include <algorithm>
#include <random>
#include <cmath>
#include "Game.hpp"

// Constante pentru a identifica zonele "off-board"
const int OFF_BOARD_WHITE = 24;
const int OFF_BOARD_BLACK = -1;

Game::Game()
    : m_phase(GamePhase::NotStarted), m_currentPlayer(WHITE), m_dice{ 0, 0 }, m_diceRolled(false) {
}

Game::~Game() {
}

void Game::start() {
    m_board = Board(); // reset board
    m_phase = GamePhase::InProgress;
    m_currentPlayer = WHITE;
    m_dice[0] = m_dice[1] = 0;
    m_diceRolled = false;
    notifyGameStarted();
}

GamePhase Game::getPhase() const {
    return m_phase;
}

Color Game::getCurrentPlayer() const {
    return m_currentPlayer;
}

void Game::rollDice() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 6);
    m_dice[0] = dist(gen);
    m_dice[1] = dist(gen);
    m_diceRolled = true;
    notifyDiceRolled();
}

const int* Game::getDice() const {
    return m_dice;
}

int Game::playerIndex(Color player) const {
    return (player == WHITE) ? 0 : 1;
}

// Verifica daca toate piesele jucatorului curent sunt in casa (sau scoase deja)
bool Game::areAllPiecesHome(Color player) const {
    // 1. Verifica daca are piese pe bara
    if (m_board.getBarCount(playerIndex(player)) > 0) return false;

    // 2. Verifica piese in afara casei
    // White Home: 18-23. Deci cautam piese in 0-17.
    // Black Home: 0-5. Deci cautam piese in 6-23.

    if (player == WHITE) {
        for (int i = 0; i <= 17; ++i) {
            if (m_board.getColumn(i).getColor() == WHITE &&
                m_board.getColumn(i).getPieceCount() > 0) {
                return false;
            }
        }
    }
    else { // BLACK
        for (int i = 6; i <= 23; ++i) {
            if (m_board.getColumn(i).getColor() == BLACK &&
                m_board.getColumn(i).getPieceCount() > 0) {
                return false;
            }
        }
    }
    return true;
}

bool Game::canBearOff(Color player) const
{
    if (!areAllPiecesHome(player)) return false;
    return true;
}

bool Game::isHomeBoard(int index, Color player) const {
    if (player == WHITE) return index >= 18 && index <= 23;
    return index >= 0 && index <= 5;
}

bool Game::hasMovesAvailable() const {
    if (!m_diceRolled) return false;

    int pIndex = playerIndex(m_currentPlayer);

    // 1. BARA: Daca avem piese pe bara, verificam doar intrarile
    if (m_board.getBarCount(pIndex) > 0) {
        int entryDice[] = { m_dice[0], m_dice[1] };
        for (int d : entryDice) {
            if (d == 0) continue;
            // White intra la 0..5 (index d-1)
            // Black intra la 23..18 (index 24-d)
            int entryIndex = (m_currentPlayer == WHITE) ? (d - 1) : (24 - d);
            if (!isMoveBlocked(entryIndex, m_currentPlayer)) return true;
        }
        return false;
    }

    // 2. TABLA: Iteram toate punctele unde avem piese
    for (int i = 0; i < 24; ++i) {
        if (m_board.getColumn(i).getPieceCount() > 0 && m_board.getColumn(i).getColor() == m_currentPlayer) {
            // Pentru fiecare zar
            int distances[] = { m_dice[0], m_dice[1] };
            for (int d : distances) {
                if (d == 0) continue;

                // Directia miscarii
                int target = (m_currentPlayer == WHITE) ? i + d : i - d;

                // --- LOGICA BEARING OFF (SCOATERE PIESE) ---
                bool offBoardLimit = (m_currentPlayer == WHITE && target > 23) || (m_currentPlayer == BLACK && target < 0);

                if (offBoardLimit) {
                    if (areAllPiecesHome(m_currentPlayer)) {
                        // Verificam daca e miscare valida de bearing off
                        // 1. Zar exact?
                        int distToEdge = (m_currentPlayer == WHITE) ? (24 - i) : (i + 1);
                        if (d == distToEdge) return true;

                        // 2. Zar mai mare, dar este cea mai indepartata piesa?
                        if (d > distToEdge) {
                            // Verificam daca exista piese mai "in spate"
                            bool furthiests = true;
                            if (m_currentPlayer == WHITE) {
                                // Pentru White, cautam piese intre 18 si i-1
                                for (int k = 18; k < i; ++k) {
                                    if (m_board.getColumn(k).getColor() == WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                                        furthiests = false; break;
                                    }
                                }
                            }
                            else {
                                // Pentru Black, cautam piese intre i+1 si 5
                                for (int k = i + 1; k <= 5; ++k) {
                                    if (m_board.getColumn(k).getColor() == BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                                        furthiests = false; break;
                                    }
                                }
                            }
                            if (furthiests) return true;
                        }
                    }
                }
                // --- MISCARE NORMALA ---
                else if (target >= 0 && target < 24) {
                    if (!isMoveBlocked(target, m_currentPlayer)) return true;
                }
            }
        }
    }
    return false;
}

bool Game::canSelectPoint(int index) const {
    if (m_phase != GamePhase::InProgress) return false;
    if (!m_diceRolled) return false;

    int pIndex = playerIndex(m_currentPlayer);

    // CAZ SPECIAL: Avem piese pe bara?
    if (m_board.getBarCount(pIndex) > 0) {
        // Putem selecta DOAR bara (indexul special 25)
        return index == BAR_INDEX;
    }

    // Altfel, nu putem selecta bara
    if (index == BAR_INDEX) return false;

    // Validari normale
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

    // --- LOGICA PENTRU MUTARE DE PE BARA ---
    if (fromIndex == BAR_INDEX) {
        for (int d : dirs) {
            if (d <= 0) continue;
            // White intra pe 0..5 (index = d - 1)
            // Black intra pe 23..18 (index = 24 - d)
            int entryIndex = (m_currentPlayer == WHITE) ? (d - 1) : (24 - d);

            if (!isMoveBlocked(entryIndex, m_currentPlayer)) {
                // Evitam duplicatele
                if (std::find(targets.begin(), targets.end(), entryIndex) == targets.end()) {
                    targets.push_back(entryIndex);
                }
            }
        }
        return targets;
    }

    // --- LOGICA NORMALA DE PE TABLA + BEARING OFF ---
    for (int d : dirs) {
        if (d <= 0) continue;

        int toIndex = (m_currentPlayer == WHITE) ? fromIndex + d : fromIndex - d;

        // 1. Verificare Bearing Off (Scoatere Piese)
        bool isOffBoard = (m_currentPlayer == WHITE && toIndex > 23) || (m_currentPlayer == BLACK && toIndex < 0);

        if (isOffBoard) {
            if (areAllPiecesHome(m_currentPlayer)) {
                int distToEdge = (m_currentPlayer == WHITE) ? (24 - fromIndex) : (fromIndex + 1);

                bool canBearOff = false;
                // Regula 1: Zar exact
                if (d == distToEdge) {
                    canBearOff = true;
                }
                // Regula 2: Zar mai mare, dar nu ai piese mai indepartate
                else if (d > distToEdge) {
                    bool isFurthest = true;
                    if (m_currentPlayer == WHITE) {
                        // Verifica daca mai exista piese la indici mai mici decat fromIndex in casa (18..fromIndex-1)
                        // Atentie: pt White, "mai in spate" inseamna indici mai mici in intervalul 18-23?
                        // Nu, White merge 0->23. Indicii 18, 19... sunt primii din casa. 23 e ultimul.
                        // "Furthest from exit" inseamna indicele cel mai MIC din casa (18).
                        // Deci daca sunt pe 22 si dau 6. Pot scoate daca nu am nimic pe 18, 19, 20, 21.
                        for (int k = 18; k < fromIndex; ++k) {
                            if (m_board.getColumn(k).getColor() == WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                                isFurthest = false; break;
                            }
                        }
                    }
                    else {
                        // Black merge 23->0. "Furthest from exit" inseamna indicele cel mai MARE din casa (5).
                        // Daca sunt pe 2 si dau 6. Pot scoate daca nu am nimic pe 5, 4, 3.
                        for (int k = fromIndex + 1; k <= 5; ++k) {
                            if (m_board.getColumn(k).getColor() == BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                                isFurthest = false; break;
                            }
                        }
                    }
                    if (isFurthest) canBearOff = true;
                }

                if (canBearOff) {
                    // Adaugam tinta speciala "Off Board"
                    // White -> 24, Black -> -1
                    int offTarget = (m_currentPlayer == WHITE) ? OFF_BOARD_WHITE : OFF_BOARD_BLACK;
                    if (std::find(targets.begin(), targets.end(), offTarget) == targets.end()) {
                        targets.push_back(offTarget);
                    }
                }
            }
            continue; // Nu mai verificam blocaje sau altele pt off-board
        }

        // 2. Miscare normala pe tabla
        if (toIndex < 0 || toIndex >= 24) continue; // Safety check

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
    if (m_phase != GamePhase::InProgress) return MoveResult::GAME_NOT_STARTED;
    if (!m_diceRolled) return MoveResult::DICE_NOT_ROLLED;

    int pIndex = playerIndex(m_currentPlayer);

    // --- CAZ 1: MUTARE DE PE BARA ---
    if (fromIndex == BAR_INDEX) {
        if (m_board.getBarCount(pIndex) == 0) return MoveResult::INVALID_MOVE;

        // Calculam zarul folosit
        int dieUsed = 0;
        if (m_currentPlayer == WHITE) dieUsed = toIndex + 1;
        else dieUsed = 24 - toIndex;

        // Gasim zarul in array
        int dieIdx = -1;
        if (m_dice[0] == dieUsed) dieIdx = 0;
        else if (m_dice[1] == dieUsed) dieIdx = 1;

        if (dieIdx == -1) return MoveResult::INVALID_MOVE;
        if (isMoveBlocked(toIndex, m_currentPlayer)) return MoveResult::BLOCKED_BY_OPPONENT;

        // HIT Logic
        if (canHit(toIndex, m_currentPlayer)) {
            Column& toCol = m_board.getColumn(toIndex);
            toCol.removePiece();
            m_board.incrementBarCount(playerIndex(m_currentPlayer == WHITE ? BLACK : WHITE));
        }

        m_board.decrementBarCount(pIndex);
        m_board.getColumn(toIndex).addPiece(m_currentPlayer);
        m_dice[dieIdx] = 0; // Consuma zarul
    }
    // --- CAZ 2: MUTARE DE PE TABLA ---
    else {
        // Validari basic
        if (fromIndex < 0 || fromIndex >= 24) return MoveResult::INVALID_FROM_COLUMN;
        if (m_board.getBarCount(pIndex) > 0) return MoveResult::INVALID_MOVE; // Trebuie intai scos de pe bara

        Column& fromCol = m_board.getColumn(fromIndex);
        if (fromCol.getPieceCount() == 0 || fromCol.getColor() != m_currentPlayer) return MoveResult::INVALID_MOVE;

        // Verificam daca e Bearing Off (destinatia e 24 sau -1)
        bool isBearingOff = (toIndex == OFF_BOARD_WHITE) || (toIndex == OFF_BOARD_BLACK);

        if (isBearingOff) {
            if (!areAllPiecesHome(m_currentPlayer)) return MoveResult::CANNOT_BEAR_OFF;
            if ((m_currentPlayer == WHITE && toIndex != OFF_BOARD_WHITE) ||
                (m_currentPlayer == BLACK && toIndex != OFF_BOARD_BLACK)) return MoveResult::INVALID_MOVE;

            // Calculam zarul necesar
            int distToEdge = (m_currentPlayer == WHITE) ? (24 - fromIndex) : (fromIndex + 1);

            // Cautam un zar valid
            int dieIdx = -1;

            // Prioritate 1: Zar exact
            if (m_dice[0] == distToEdge) dieIdx = 0;
            else if (m_dice[1] == distToEdge) dieIdx = 1;

            // Prioritate 2: Zar mai mare (daca e piesa cea mai indepartata)
            if (dieIdx == -1) {
                // Trebuie sa verificam din nou conditia "furthest piece"
                bool isFurthest = true;
                if (m_currentPlayer == WHITE) {
                    for (int k = 18; k < fromIndex; ++k) {
                        if (m_board.getColumn(k).getColor() == WHITE && m_board.getColumn(k).getPieceCount() > 0) {
                            isFurthest = false; break;
                        }
                    }
                }
                else {
                    for (int k = fromIndex + 1; k <= 5; ++k) {
                        if (m_board.getColumn(k).getColor() == BLACK && m_board.getColumn(k).getPieceCount() > 0) {
                            isFurthest = false; break;
                        }
                    }
                }

                if (isFurthest) {
                    // Folosim zarul mai mare decat distanta, dar luam cel mai mic dintre cele care sunt suficient de mari? 
                    // De obicei se foloseste orice zar > distanta. Luam primul gasit.
                    if (m_dice[0] > distToEdge) dieIdx = 0;
                    else if (m_dice[1] > distToEdge) dieIdx = 1;
                }
            }

            if (dieIdx == -1) return MoveResult::INVALID_MOVE;

            // Executam scoaterea
            fromCol.removePiece();
            m_board.incrementBorneOffCount(pIndex);
            m_dice[dieIdx] = 0;

            notifyMoveMade(fromIndex, toIndex, MoveResult::SUCCESS);

            // VERIFICARE CASTIG
            if (m_board.getBorneOffCount(pIndex) == 15) {
                m_phase = GamePhase::Finished;
                notifyGameFinished(m_currentPlayer);
                return MoveResult::SUCCESS;
            }

        }
        else {
            // --- Mutare Standard pe tabla ---
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
                m_board.incrementBarCount(playerIndex(m_currentPlayer == WHITE ? BLACK : WHITE));
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
    m_currentPlayer = (m_currentPlayer == WHITE) ? BLACK : WHITE;
    notifyTurnChanged();
}

bool Game::isMoveBlocked(int toIndex, Color player) const {
    const Column& toCol = m_board.getColumn(toIndex);
    if (toCol.getPieceCount() >= 2 && toCol.getColor() != player) return true;
    return false;
}

bool Game::canHit(int toIndex, Color player) const {
    const Column& toCol = m_board.getColumn(toIndex);
    return toCol.getPieceCount() == 1 && toCol.getColor() != player && toCol.getColor() != NONE;
}