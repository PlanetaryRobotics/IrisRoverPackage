#include "Tester.hpp"

TEST(Nominal, AddOperationTest) {
    CubeRover::Tester tester;
    tester.testAddCommand();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}