/**
 * test_control.cpp - Unit tests for Control struct
 */

#include "game/Control.h"
#include <gtest/gtest.h>

TEST(ControlTest, InitializationAllFalse) {
  Control c = initializeControl();

  // Arrow keys
  EXPECT_FALSE(c.arrowUp);
  EXPECT_FALSE(c.arrowDown);
  EXPECT_FALSE(c.arrowLeft);
  EXPECT_FALSE(c.arrowRight);

  // Power controls
  EXPECT_FALSE(c.power_inc);
  EXPECT_FALSE(c.power_dec);

  // Mouse buttons
  EXPECT_FALSE(c.rightPressed);
  EXPECT_FALSE(c.newRightPressed);
  EXPECT_FALSE(c.leftPressed);
  EXPECT_FALSE(c.newLeftPressed);
  EXPECT_FALSE(c.scrollPressed);

  // Action keys
  EXPECT_FALSE(c.space);
  EXPECT_FALSE(c.keyEsc);
}

TEST(ControlTest, InitializationVectorsZero) {
  Control c = initializeControl();

  EXPECT_DOUBLE_EQ(c.pressedPos.getX(), 0.0);
  EXPECT_DOUBLE_EQ(c.pressedPos.getY(), 0.0);
  EXPECT_DOUBLE_EQ(c.pressedPos.getZ(), 0.0);

  EXPECT_DOUBLE_EQ(c.releasedPos.getX(), 0.0);
  EXPECT_DOUBLE_EQ(c.releasedPos.getY(), 0.0);
  EXPECT_DOUBLE_EQ(c.releasedPos.getZ(), 0.0);
}

TEST(ControlTest, CanSetArrowKeys) {
  Control c = initializeControl();

  c.arrowUp = true;
  c.arrowDown = true;
  c.arrowLeft = true;
  c.arrowRight = true;

  EXPECT_TRUE(c.arrowUp);
  EXPECT_TRUE(c.arrowDown);
  EXPECT_TRUE(c.arrowLeft);
  EXPECT_TRUE(c.arrowRight);
}

TEST(ControlTest, CanSetActionKeys) {
  Control c = initializeControl();

  c.space = true;
  c.keyEsc = true;

  EXPECT_TRUE(c.space);
  EXPECT_TRUE(c.keyEsc);
}

TEST(ControlTest, CanSetPowerControls) {
  Control c = initializeControl();

  c.power_inc = true;
  c.power_dec = true;

  EXPECT_TRUE(c.power_inc);
  EXPECT_TRUE(c.power_dec);
}

TEST(ControlTest, CanSetMousePositions) {
  Control c = initializeControl();

  c.pressedPos = Vector(100, 200, 0);
  c.releasedPos = Vector(150, 250, 0);

  EXPECT_DOUBLE_EQ(c.pressedPos.getX(), 100.0);
  EXPECT_DOUBLE_EQ(c.pressedPos.getY(), 200.0);
  EXPECT_DOUBLE_EQ(c.releasedPos.getX(), 150.0);
  EXPECT_DOUBLE_EQ(c.releasedPos.getY(), 250.0);
}

TEST(ControlTest, MultipleInitializationsAreIndependent) {
  Control c1 = initializeControl();
  Control c2 = initializeControl();

  c1.arrowUp = true;
  c1.space = true;

  // c2 should still be all false
  EXPECT_FALSE(c2.arrowUp);
  EXPECT_FALSE(c2.space);
}
