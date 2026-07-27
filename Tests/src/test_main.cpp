#include <gtest/gtest.h>

#include "log_level_manager.hpp"
#include "utils.hpp"

int main(int argc, char** argv)
{
    app_logging::GetLogLevelManager().SetLogLevel();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
