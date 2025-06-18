/// @file
///
/// @author    Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests `argument_error` exception class and error reporting
/// mechanisms.
///
/// This project is licensed under the terms of MIT license.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cstddef>
#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

#include "doctest/doctest.h"
#include "optrone/error.hpp"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

TEST_CASE("Template validation")
{
    auto no_name_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "No name option.",
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, { no_name_option }, {}), std::invalid_argument);

    auto no_name_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description = "No name subcommand.",
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, {}, { no_name_subcommand }), std::invalid_argument);

    auto uppercase_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Uppercase option.",
        .short_names = { 'A' },
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, { uppercase_option }, {}), std::invalid_argument);

    for (char c : { '=', ':', '-', '/' })
    {
        auto separator_option = std::make_shared<optrone::option_template>(optrone::option_template{
            .description = "Separator option.",
            .short_names = { c },
        });

        auto separator_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
            .description = "Separator subcommand.",
            .names       = { std::string(1, c) },
        });

        CHECK_THROWS_AS(optrone::parse_arguments({}, { separator_option }, { separator_subcommand }), std::invalid_argument);
    }

    auto small_long_name_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Small long name option.",
        .long_names  = { "a" },
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, { small_long_name_option }, {}), std::invalid_argument);

    auto too_many_defaults_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Too many defaults option.",
        .short_names = { 'a' },
        .defaults    = { "default-value-0" },
    });

    for (std::size_t i : std::views::iota(0, 4))
    {
        if (i != 0)
        {
            too_many_defaults_option->params.emplace_back(std::format("param-{}", i));
            too_many_defaults_option->defaults.emplace_back(std::format("defaults-{}", i));
        }

        CHECK_THROWS_AS(optrone::parse_arguments({}, { too_many_defaults_option }, {}), std::invalid_argument);
    }

    auto mutex_features_used_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Mutex features used option.",
        .short_names = { 'b' },
        .params      = { "param-1" },
        .defaults    = { "default-1" },
        .variadic    = true,
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, { mutex_features_used_option }, {}), std::invalid_argument);

    auto mutex_features_used_1_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description = "Mutex features used subcommand.",
        .names       = { "name-1" },
        .params      = { "param-1" },
        .defaults    = { "default-1" },
        .variadic    = true,
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, {}, { mutex_features_used_1_subcommand }), std::invalid_argument);

    auto nested_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description = "Nested subcommand.",
        .names       = { "sub-name" },
    });

    auto mutex_features_used_2_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description        = "Mutex features used subcommand.",
        .names              = { "name-1" },
        .variadic           = true,
        .nested_subcommands = { nested_subcommand },
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, {}, { mutex_features_used_2_subcommand }), std::invalid_argument);

    auto mutex_features_used_3_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description        = "Mutex features used subcommand.",
        .names              = { "name-1" },
        .variadic           = true,
        .nested_subcommands = { nested_subcommand },
    });

    CHECK_THROWS_AS(optrone::parse_arguments({}, {}, { mutex_features_used_3_subcommand }), std::invalid_argument);
}

TEST_CASE("Parsing error")
{
    // Unrecognized template

    CHECK_THROWS_AS(optrone::parse_arguments({ "-a" }, {}, {}), optrone::argument_error);
    CHECK_THROWS_AS(optrone::parse_arguments({ "--name" }, {}, {}), optrone::argument_error);
    CHECK_THROWS_AS(optrone::parse_arguments({ "/name" }, {}, {}), optrone::argument_error);
    CHECK_THROWS_AS(optrone::parse_arguments({ "name" }, {}, {}), optrone::argument_error);

    // Too few values

    auto params_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Params option.",
        .short_names = { 'a' },
    });

    std::vector<std::string> values = { "value-1", "value-2", "value-3" };

    for (std::size_t i : std::views::iota(0zu, values.size()))
    {
        params_option->params.emplace_back(std::format("param-{}", i + 1));

        std::vector<std::string> args = values | std::views::take(i) | std::ranges::to<std::vector>();
        args.insert(args.begin(), "-a");

        CHECK_THROWS_AS(optrone::parse_arguments(args, { params_option }, {}), optrone::argument_error);
    }
}
