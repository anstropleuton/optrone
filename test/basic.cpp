/// @file
///
/// @author    Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This test file tests basic argument parsing of Optrone.
///
/// This project is licensed under the terms of MIT license.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cstddef>
#include <format>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "doctest/doctest.h"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

TEST_CASE("Basic argument parsing")
{
    auto option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Option.",
        .short_names = { 'a',         'b',         'c'         },
        .long_names  = { "name-1",    "name-2",    "name-3"    },
        .params      = { "param-1",   "param-2",   "param-3"   },
        .defaults    = { "default-1", "default-2", "default-3" },
    });

    auto subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description = "Subcommand.",
        .names       = { "name-1",    "name-2",    "name-3"    },
        .params      = { "param-1",   "param-2",   "param-3"   },
        .defaults    = { "default-1", "default-2", "default-3" },
    });

    // POSIX-style

    for (char short_name : option->short_names)
    {
        SUBCASE(std::format("Name: -{}", short_name).c_str())
        {
            auto parsed_args = optrone::parse_arguments({ std::format("-{}", short_name) }, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
        }
    }

    for (const std::string &long_name : option->long_names)
    {
        SUBCASE(std::format("Name: --{}", long_name).c_str())
        {
            auto parsed_args = optrone::parse_arguments({ std::format("--{}", long_name) }, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
        }
    }

    // Microsoft-style

    for (char short_name : option->short_names)
    {
        SUBCASE(std::format("Name: /{}", short_name).c_str())
        {
            auto parsed_args = optrone::parse_arguments({ std::format("/{}", short_name) }, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
        }
    }

    for (const std::string &long_name : option->long_names)
    {
        SUBCASE(std::format("Name: /{}", long_name).c_str())
        {
            auto parsed_args = optrone::parse_arguments({ std::format("/{}", long_name) }, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
        }
    }

    // Subcommand

    for (const std::string &name : subcommand->names)
    {
        SUBCASE(std::format("Name: {}", name).c_str())
        {
            auto parsed_args = optrone::parse_arguments({ std::format("{}", name) }, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_subcommand.lock() == subcommand);
        }
    }

    // Parameters and defautls

    std::vector<std::string> values = { "value-1", "value-2", "value-3" };

    for (std::size_t i : std::views::iota(0zu, option->params.size()))
    {
        auto provided_values = values | std::views::take(i) | std::ranges::to<std::vector>();
        auto default_values  = option->defaults | std::views::drop(i) | std::ranges::to<std::vector>();
        auto expected_values = std::vector{ provided_values, default_values } | std::views::join | std::ranges::to<std::vector>();

        SUBCASE(std::format("Values: {}", provided_values | std::ranges::views::join_with(' ') | std::ranges::to<std::string>()).c_str())
        {
            std::vector<std::string> args = provided_values;
            args.insert(args.begin(), "-a");

            auto parsed_args = optrone::parse_arguments(args, { option }, { subcommand });

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
            CHECK(parsed_args[0].values == expected_values);
        }
    }

    // Splitting values

    SUBCASE("Splitting values")
    {
        auto parsed_args = optrone::parse_arguments({ "-a=value" }, { option }, { subcommand });

        REQUIRE(parsed_args.size() == 1);
        CHECK(parsed_args[0].ref_option.lock() == option);
        CHECK(parsed_args[0].values == std::vector<std::string>{ "value", "default-2", "default-3" });

        parsed_args = optrone::parse_arguments({ "/a:value" }, { option }, { subcommand });

        REQUIRE(parsed_args.size() == 1);
        CHECK(parsed_args[0].ref_option.lock() == option);
        CHECK(parsed_args[0].values == std::vector<std::string>{ "value", "default-2", "default-3" });
    }

    // Splitting short names

    SUBCASE("Splitting short names")
    {
        auto parsed_args = optrone::parse_arguments({ "-abc" }, { option }, { subcommand });

        REQUIRE(parsed_args.size() == 3);

        for (std::size_t i : std::views::iota(0, 3)) CHECK(parsed_args[i].ref_option.lock() == option);
    }
}

TEST_CASE("Variadic arguments parsing")
{
    auto option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Option.",
        .short_names = { 'a' },
        .variadic    = true,
    });

    std::vector<std::string> values = { "value-1", "value-2", "value-3" };

    for (std::size_t i : std::views::iota(0zu, values.size() + 1))
    {
        if (i != 0) option->params.emplace_back(std::format("param-{}", i));

        for (std::size_t j : std::views::iota(option->params.size(), values.size()))
        {
            auto provided_values = values | std::views::take(j) | std::ranges::to<std::vector>();

            std::vector<std::string> args = provided_values;
            args.insert(args.begin(), "-a");

            auto parsed_args = optrone::parse_arguments(args, { option }, {});

            REQUIRE(parsed_args.size() == 1);
            CHECK(parsed_args[0].ref_option.lock() == option);
            CHECK(parsed_args[0].values == provided_values);
        }
    }
}

TEST_CASE("Nested options and subcommand parsing")
{
    auto nested_option = std::make_shared<optrone::option_template>(optrone::option_template{
        .description = "Nested option.",
        .short_names = { 'a' },
    });

    auto nested_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description = "Nested subcommand.",
        .names       = { "sub-name" },
    });

    auto subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
        .description        = "Subcommand.",
        .names              = { "name" },
        .nested_options     = { nested_option },
        .nested_subcommands = { nested_subcommand },
    });

    auto parsed_args = optrone::parse_arguments({ "name" }, {}, { subcommand });

    REQUIRE(parsed_args.size() == 1);
    CHECK(parsed_args[0].ref_subcommand.lock() == subcommand);

    parsed_args = optrone::parse_arguments({ "name", "sub-name" }, {}, { subcommand });

    REQUIRE(parsed_args.size() == 2);
    CHECK(parsed_args[0].ref_subcommand.lock() == subcommand);
    CHECK(parsed_args[1].ref_subcommand.lock() == nested_subcommand);

    parsed_args = optrone::parse_arguments({ "name", "-a" }, {}, { subcommand });

    REQUIRE(parsed_args.size() == 2);
    CHECK(parsed_args[0].ref_subcommand.lock() == subcommand);
    CHECK(parsed_args[1].ref_option.lock() == nested_option);
}
