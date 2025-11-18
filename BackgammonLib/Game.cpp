#include <algorithm>
#include <random>
#include <cmath>
#include "Game.h"

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

// Verifica daca jucatorul are voie sa scoata piese (toate piesele sunt in casa)
bool Game::areAllPiecesHome(Color player) const {
    if (m_board.getBarCount(playerIndex(player)) > 0) return false;

    int start = (player == WHITE) ? 0 : 6;
    int end = (player == WHITE) ? 17 : 23;

    // Daca gasim orice piesa in afara casei (White Home: 18-23, Black Home: 0-5)
    // White pieces in 0-17 -> false
    // Black pieces in 6-23 -> false
    for (int i = start; i <= end; ++i) {
        if (m_board.getColumn(i).getColor() == player && m_board.getColumn(i).getPieceCount() > 0) {
            return false;
        }
    }
    return true;
}

bool Game::isHomeBoard(int index, Color player) const {
    if (player == WHITE) return index >= 18 && index <= 23;
    return index >= 0 && index <= 5;
}

bool Game::hasMovesAvailable() const {
    if (!m_diceRolled) return false;

    int pIndex = playerIndex(m_currentPlayer);

    // 1. Daca avem piese pe bara, verificam DOAR intrarile posibile
    if (m_board.getBarCount(pIndex) > 0) {
        int entryDice[] = { m_dice[0], m_dice[1] };
        for (int d : entryDice) {
            if (d == 0) continue;
            // White intra pe 0..5 (index = d - 1)
            // Black intra pe 23..18 (index = 24 - d)
            int entryIndex = (m_currentPlayer == WHITE) ? (d - 1) : (24 - d);
            if (!isMoveBlocked(entryIndex, m_currentPlayer)) return true;
        }
        return false;
    }

    // 2. Miscari normale sau Bear Off
    for (int i = 0; i < 24; ++i) {
        if (m_board.getColumn(i).getPieceCount() > 0 && m_board.getColumn(i).getColor() == m_currentPlayer) {
            int distances[] = { m_dice[0], m_dice[1] };
            for (int d : distances) {
                if (d == 0) continue;
                int target = (m_currentPlayer == WHITE) ? i + d : i - d;

                // Verificari Bearing Off
                if (m_currentPlayer == WHITE && target > 23) {
                    if (areAllPiecesHome(WHITE)) return true;
                }
                else if (m_currentPlayer == BLACK && target < 0) {
                    if (areAllPiecesHome(BLACK)) return true;
                }
                // Verificari miscare normala
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

    // LOGICA PENTRU BARA
    if (fromIndex == BAR_INDEX) {
        for (int d : dirs) {
            if (d <= 0) continue;
            // White: 1->0, 6->5. Black: 1->23, 6->18
            int entryIndex = (m_currentPlayer == WHITE) ? (d - 1) : (24 - d);

            if (!isMoveBlocked(entryIndex, m_currentPlayer)) {
                // Evitam duplicatele (daca zarurile sunt egale)
                if (std::find(targets.begin(), targets.end(), entryIndex) == targets.end()) {
                    targets.push_back(entryIndex);
                }
            }
        }
        return targets;
    }

    // LOGICA NORMALA DE PE TABLA
    for (int d : dirs) {
        if (d <= 0) continue;

        int toIndex = (m_currentPlayer == WHITE) ? fromIndex + d : fromIndex - d;

        // Logic for Bearing Off (Scoatere piese)
        bool offBoard = (m_currentPlayer == WHITE && toIndex > 23) || (m_currentPlayer == BLACK && toIndex < 0);

        if (offBoard) {
            if (areAllPiecesHome(m_currentPlayer)) {
                // Simplificare: acceptam scoaterea doar daca zarul e exact sau mai mare (pentru ultima piesa)
                // Aici implementam doar "exact sau mai mare daca e pe cel mai indepartat punct"
                // Pentru simplitate in acest stadiu: permitem daca indexul iese afara
                bool alreadyAdded = false;
                // Reprezentam "Off Board" prin index -100 sau pur si simplu un index invalid grafic dar logic valid?
                // Conventie: White Bearoff > 23, Black Bearoff < 0. UI-ul trebuie sa stie sa nu deseneze highlight gresit.
                // Vom returna indexul exact calculat, BoardWidget trebuie sa il ignore la desenare sau sa il trateze special.
                targets.push_back(toIndex);
            }
            continue;
        }

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
    if (m_phase != GamePhase::InProgress) return MoveResult::GameNotStarted;
    if (!m_diceRolled) return MoveResult::DiceNotRolled;

    int pIndex = playerIndex(m_currentPlayer);

    // 1. Validare BARA
    if (fromIndex == BAR_INDEX) {
        if (m_board.getBarCount(pIndex) == 0) return MoveResult::InvalidMove;

        // Calculam distanta pentru a consuma zarul
        int dieValue = 0;
        if (m_currentPlayer == WHITE) dieValue = toIndex + 1;
        else dieValue = 24 - toIndex;

        // Verificam daca corespunde cu un zar
        int dieIdx = -1;
        if (m_dice[0] == dieValue) dieIdx = 0;
        else if (m_dice[1] == dieValue) dieIdx = 1;

        if (dieIdx == -1) return MoveResult::InvalidMove;
        if (isMoveBlocked(toIndex, m_currentPlayer)) return MoveResult::BlockedByOpponent;

        // HIT Logic la intrare
        if (canHit(toIndex, m_currentPlayer)) {
            Column& toCol = m_board.getColumn(toIndex);
            toCol.removePiece(); // Scoate piesa adversa
            m_board.incrementBarCount(playerIndex(m_currentPlayer == WHITE ? BLACK : WHITE));
        }

        // Muta piesa
        m_board.decrementBarCount(pIndex);
        m_board.getColumn(toIndex).addPiece(m_currentPlayer);

        // Consuma zar
        m_dice[dieIdx] = 0;

    }
    else {
        // 2. Validare miscare normala
        if (fromIndex < 0 || fromIndex >= 24) return MoveResult::InvalidFromColumn;

        // Daca avem piese pe bara, nu putem muta de pe tabla!
        if (m_board.getBarCount(pIndex) > 0) return MoveResult::InvalidMove;

        Column& fromCol = m_board.getColumn(fromIndex);
        if (fromCol.getPieceCount() == 0 || fromCol.getColor() != m_currentPlayer) return MoveResult::InvalidMove;

        // Calcul distanta
        int distance = std::abs(toIndex - fromIndex);

        // Verificare Bearing Off
        bool isBearOff = (m_currentPlayer == WHITE && toIndex > 23) || (m_currentPlayer == BLACK && toIndex < 0);

        if (isBearOff) {
            if (!areAllPiecesHome(m_currentPlayer)) return MoveResult::CannotBearOff;
            // Aici ar trebui logica fina pentru "folosirea zarului mai mare pentru piesa de pe pozitia cea mai mica"
            // Simplificam: distanta trebuie sa fie egala cu un zar.
            int dieIdx = -1;
            // Nota: distanta pentru bearoff e (24 - from) pt White, (from + 1) pt Black
            int bearOffDist = (m_currentPlayer == WHITE) ? (24 - fromIndex) : (fromIndex + 1);

            // Relaxam regula: daca zarul e >= distanta necesara
            if (m_dice[0] >= bearOffDist && m_dice[0] > 0) dieIdx = 0;
            else if (m_dice[1] >= bearOffDist && m_dice[1] > 0) dieIdx = 1;

            if (dieIdx == -1) return MoveResult::InvalidMove;

            fromCol.removePiece();
            m_board.incrementBorneOffCount(pIndex);
            m_dice[dieIdx] = 0;

            notifyMoveMade(fromIndex, toIndex, MoveResult::Success);

            // Check win
            if (m_board.getBorneOffCount(pIndex) == 15) {
                m_phase = GamePhase::Finished;
                notifyGameFinished(m_currentPlayer);
                return MoveResult::Success;
            }
        }
        else {
            // Miscare standard pe tabla
            if (toIndex < 0 || toIndex >= 24) return MoveResult::InvalidToColumn;

            int dieIdx = -1;
            if (m_dice[0] == distance) dieIdx = 0;
            else if (m_dice[1] == distance) dieIdx = 1;

            if (dieIdx == -1) return MoveResult::InvalidMove;
            if (isMoveBlocked(toIndex, m_currentPlayer)) return MoveResult::BlockedByOpponent;

            // HIT LOGIC
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

    // Verifica daca s-a terminat tura
    if ((m_dice[0] == 0 && m_dice[1] == 0) || !hasMovesAvailable()) {
        m_diceRolled = false;
        switchTurn();
    }

    notifyMoveMade(fromIndex, toIndex, MoveResult::Success);
    return MoveResult::Success;
}

// ... restul metodelor raman neschimbate (getters, etc) ...
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