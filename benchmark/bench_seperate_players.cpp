#include <benchmark/registration.h>
#include <benchmark/state.h>

#include <ranges>
#include <tuple>

struct Players
{
    std::vector<int> rectangles_blue_;
    std::vector<int> rectangles_red_;

    std::vector<int> rectangles_blue_defense_;
    std::vector<int> rectangles_blue_offense_;
    std::vector<int> rectangles_red_defense_;
    std::vector<int> rectangles_red_offense_;
};

void SeperateIntoOffenseAndDefenseBranchless(const int size, Players players_)
{
    constexpr int64_t number_of_player_rows = 8;
    const int64_t frame_width = size;
    const int64_t row_width = frame_width / number_of_player_rows;

    // We make a mask of each teams position of offense and deffense, below are the indicies of rows in the mask
    //     7  6  5  4  3  2  1  0
    // 0b ?? ?? ?? ?? ?? ?? ?? ??,
    constexpr uint32_t role_mask_width = 2;
    constexpr uint32_t role_none = 0b00;
    constexpr uint32_t role_defense = 0b01;
    constexpr uint32_t role_offense = 0b10;
    constexpr uint32_t red_mask = 0b0000100010000101;
    constexpr uint32_t blue_mask = 0b0101001000100000;

    for (const auto& rectangle_red : players_.rectangles_red_)
    {
        int64_t row = rectangle_red / row_width;
        int64_t row_shift = row * role_mask_width;

        uint32_t role = red_mask >> row_shift;

        if (role & role_defense)
        {
            players_.rectangles_red_defense_.push_back(rectangle_red);
        }

        else if (role & role_offense)
        {
            players_.rectangles_red_offense_.push_back(rectangle_red);
        }
    }

    for (const auto& rectangle_blue : players_.rectangles_blue_)
    {
        int64_t row = rectangle_blue / row_width;
        int64_t row_shift = row * role_mask_width;

        uint32_t role = red_mask >> row_shift;

        if (role & role_defense)
        {
            players_.rectangles_blue_defense_.push_back(rectangle_blue);
        }

        else if (role & role_offense)
        {
            players_.rectangles_blue_offense_.push_back(rectangle_blue);
        }
    }
}

void SeperateIntoOffenseAndDefense(const int size, Players players_)
{
    constexpr int64_t number_of_player_rows = 8;
    const int64_t frame_width = size;
    const int64_t row_width = frame_width / number_of_player_rows;
    auto get_row_index = [row_width](int64_t x) { return x / row_width; };

    for (const auto& [rectangle_blue, rectangle_red] :
         std::views::zip(players_.rectangles_blue_, players_.rectangles_red_))
    {
        switch (get_row_index(rectangle_red))
        {
            case 0:
            case 1:
                players_.rectangles_red_defense_.push_back(rectangle_red);
                break;
            case 3:
            case 5:
                players_.rectangles_red_offense_.push_back(rectangle_red);
                break;
            default:
                break;
        }

        switch (get_row_index(rectangle_blue))
        {
            case 2:
            case 4:
                players_.rectangles_blue_offense_.push_back(rectangle_blue);
                break;
            case 6:
            case 7:
                players_.rectangles_blue_defense_.push_back(rectangle_blue);
                break;
            default:
                break;
        }
    }
}

void SplitPlayersByVerticalLine(const std::vector<int>& rectangles,
                                const int64_t vertical_line_x,
                                const bool offense_is_right_of_line,
                                std::vector<int>& defense,
                                std::vector<int>& offense)
{
    defense.clear();
    offense.clear();

    for (const auto& rectangle : rectangles)
    {
        const bool is_on_right = rectangle > vertical_line_x;
        if (is_on_right == offense_is_right_of_line)
        {
            offense.push_back(rectangle);
        }
        else
        {
            defense.push_back(rectangle);
        }
    }
}

void SeperateIntoOffenseAndDefenseCudak(const int size, Players players_)
{
    constexpr int64_t field_sections = 8;
    const int64_t red_dividing_line_x = size * 3 / field_sections;
    const int64_t blue_dividing_line_x = size * 5 / field_sections;

    SplitPlayersByVerticalLine(players_.rectangles_red_,
                               red_dividing_line_x,
                               true,
                               players_.rectangles_red_defense_,
                               players_.rectangles_red_offense_);
    SplitPlayersByVerticalLine(players_.rectangles_blue_,
                               blue_dividing_line_x,
                               false,
                               players_.rectangles_blue_defense_,
                               players_.rectangles_blue_offense_);
}

void SeperateIntoOffenseAndDefenseZipped(const int size, Players players_)
{
    constexpr int64_t number_of_player_rows = 8;
    const int64_t row_width = size / number_of_player_rows;

    constexpr uint32_t role_mask_width = 2;
    constexpr uint32_t role_none = 0b00;
    constexpr uint32_t role_defense = 0b01;
    constexpr uint32_t role_offense = 0b10;
    constexpr uint32_t red_mask = 0b0000100010000101;
    constexpr uint32_t blue_mask = 0b0101001000100000;

    for (auto&& [rectangle_blue, rectangle_red] :
         std::views::zip(players_.rectangles_blue_, players_.rectangles_red_))
    {
        int64_t row_red = rectangle_red / row_width;
        int64_t row_shift_red = row_red * role_mask_width;

        int64_t row_blue = rectangle_blue / row_width;
        int64_t row_shift_blue = row_blue * role_mask_width;

        uint32_t role_red = red_mask >> row_shift_red;
        uint32_t role_blue = blue_mask >> row_shift_blue;

        if (role_red & role_defense)
        {
            players_.rectangles_red_defense_.push_back(rectangle_red);
        }
        else if (role_red & role_offense)
        {
            players_.rectangles_red_offense_.push_back(rectangle_red);
        }

        if (role_blue & role_defense)
        {
            players_.rectangles_blue_defense_.push_back(rectangle_blue);
        }
        else if (role_blue & role_offense)
        {
            players_.rectangles_blue_offense_.push_back(rectangle_blue);
        }
    }
}

static void BM_Branch(benchmark::State& state)
{
    Players players{};
    players.rectangles_blue_ = {10, 30, 50, 40, 60, 20, 80, 110, 100, 70};
    players.rectangles_red_ = {5, 60, 10, 70, 40, 80, 70, 100, 110, 10};
    int size = 120;

    // Perform setup here
    for (auto _ : state)
    {
        // This code gets timed
        SeperateIntoOffenseAndDefense(size, players);
    }
}
BENCHMARK(BM_Branch);

static void BM_Branchless(benchmark::State& state)
{
    Players players{};
    players.rectangles_blue_ = {10, 30, 50, 40, 60, 20, 80, 110, 100, 70};
    players.rectangles_red_ = {5, 60, 10, 70, 40, 80, 70, 100, 110, 10};
    int size = 120;

    // Perform setup here
    for (auto _ : state)
    {
        // This code gets timed
        SeperateIntoOffenseAndDefenseBranchless(size, players);
    }
}
BENCHMARK(BM_Branchless);

static void BM_BranchlessZipped(benchmark::State& state)
{
    Players players{};
    players.rectangles_blue_ = {10, 30, 50, 40, 60, 20, 80, 110, 100, 70};
    players.rectangles_red_ = {5, 60, 10, 70, 40, 80, 70, 100, 110, 10};
    int size = 120;

    // Perform setup here
    for (auto _ : state)
    {
        // This code gets timed
        SeperateIntoOffenseAndDefenseZipped(size, players);
    }
}
BENCHMARK(BM_BranchlessZipped);

static void BM_Cudak(benchmark::State& state)
{
    Players players{};
    players.rectangles_blue_ = {10, 30, 50, 40, 60, 20, 80, 110, 100, 70};
    players.rectangles_red_ = {5, 60, 10, 70, 40, 80, 70, 100, 110, 10};
    int size = 120;

    // Perform setup here
    for (auto _ : state)
    {
        // This code gets timed
        SeperateIntoOffenseAndDefenseCudak(size, players);
    }
}
BENCHMARK(BM_Cudak);

BENCHMARK_MAIN();