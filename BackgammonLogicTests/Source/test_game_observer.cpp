#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Game.h"
#include "IGameObserver.h"
#include "MoveResult.h"
#include "Color.h"

// ==============================
// MOCK OBSERVER
// ==============================

class MockObserver : public IGameObserver {
public:
    virtual ~MockObserver() = default;

    MOCK_METHOD(void, onGameStarted, (), (override));
    MOCK_METHOD(void, onDiceRolled, (Color, int, int), (override));
    MOCK_METHOD(void, onTurnChanged, (Color), (override));
};

TEST(GameObserverTests, NotifiesGameStarted) {
    Game g;
    ::testing::NiceMock<MockObserver> obs;
    g.addObserver(&obs);

    EXPECT_CALL(obs, onGameStarted()).Times(1);
    EXPECT_CALL(obs, onDiceRolled).Times(0);
    EXPECT_CALL(obs, onTurnChanged).Times(0);

    g.start();
}

TEST(GameObserverTests, NotifiesDiceRolled) {
    Game g;
    ::testing::NiceMock<MockObserver> obs;
    g.addObserver(&obs);

    g.start();

    EXPECT_CALL(obs, onDiceRolled(g.getCurrentPlayer(), testing::_, testing::_)).Times(1);

    g.rollDice();
}