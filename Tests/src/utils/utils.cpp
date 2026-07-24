#include "utils.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>

namespace test_utils
{
bool IsVerboseTestRun()
{
    const char* const verbose_flag{std::getenv("FOOSBALL_TEST_VERBOSE")};
    return verbose_flag != nullptr && verbose_flag[0] != '\0' && verbose_flag[0] != '0';
}

std::string TestFilePath(const std::string& file_name)
{
    return (std::filesystem::path(TEST_SOURCE_DIR) / "test_files" / file_name).string();
}

std::string TestOutputPath(const std::string& file_name)
{
    const auto output_dir = std::filesystem::path(TEST_SOURCE_DIR) / "test_outputs";
    std::filesystem::create_directories(output_dir);
    return (output_dir / file_name).string();
}
}  // namespace test_utils
