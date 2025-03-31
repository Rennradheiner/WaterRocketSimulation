// rocket_test.cpp
#include <gtest/gtest.h>
#include <rocket.hpp>

// Test fixture for Rocket class
class RocketTest : public ::testing::Test
{
  public:
    RocketTest()
        : m_rocket(
              START_PRESSURE_BAR, START_WATER_VOLUME_L, NOZZLE_DIAMETER_MM, BOTTLE_VOLUME_L, EMPTY_ROCKET_MASS_KG,
              ROCKET_DIAMETER_MM, DRAG_COEFFICIENT)
    {
    }

    void SetUp() final {}

    void TearDown() final {}

  protected:
    float const START_PRESSURE_BAR{3.0f};
    float const START_WATER_VOLUME_L{0.8f};
    float const NOZZLE_DIAMETER_MM{9.0f};
    float const BOTTLE_VOLUME_L{1.0f};
    float const EMPTY_ROCKET_MASS_KG{0.3};
    float const ROCKET_DIAMETER_MM{100.0f};
    float const DRAG_COEFFICIENT{0.1f};

    Rocket m_rocket;
};

// Test case for initial state
TEST_F(RocketTest, InitialState)
{
    EXPECT_EQ(m_rocket.getAltitude(), 0);
}
