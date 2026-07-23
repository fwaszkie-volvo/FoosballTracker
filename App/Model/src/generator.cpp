#include "generator.hpp"

#include <algorithm>
#include <cctype>
#include <optional>
#include <random>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>

namespace
{
std::string TrimNickname(const std::string& nickname)
{
    const auto first = std::ranges::find_if_not(
      nickname, [](const unsigned char ch) { return std::isspace(ch) != 0; });
    if (first == nickname.end())
    {
        return {};
    }

    const auto reversed_nickname = std::ranges::subrange(nickname.rbegin(), nickname.rend());
    const auto last =
      std::ranges::find_if_not(reversed_nickname,
                               [](const unsigned char ch) { return std::isspace(ch) != 0; })
        .base();
    return std::string(first, last);
}
}  // namespace

namespace generator
{
std::optional<TeamDraw> GenerateTeams(const std::array<std::string, kPlayersCount>& nicknames)
{
    std::vector<std::string> sanitized_nicknames;
    sanitized_nicknames.reserve(nicknames.size());
    std::unordered_set<std::string> unique_nicknames;

    for (const auto& nickname : nicknames)
    {
        auto trimmed_nickname = TrimNickname(nickname);
        if (trimmed_nickname.empty() || !unique_nicknames.insert(trimmed_nickname).second)
        {
            return std::nullopt;
        }

        sanitized_nicknames.push_back(std::move(trimmed_nickname));
    }

    std::mt19937 random_generator{std::random_device{}()};
    std::ranges::shuffle(sanitized_nicknames, random_generator);

    return {
      .first_team =
        Team{.player_one = sanitized_nicknames[0], .player_two = sanitized_nicknames[1]},
      .second_team =
        Team{.player_one = sanitized_nicknames[2], .player_two = sanitized_nicknames[3]},
    };
}
}  // namespace generator
