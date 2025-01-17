/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 1: Option recognition tests.
 *
 *  @copyright  Copyright (c) 2024 Anstro Pleuton
 *
 *    ___        _
 *   / _ \ _ __ | |_ _ __ ___  _ __   ___
 *  | | | | '_ \| __| '__/ _ \| '_ \ / _ \
 *  | |_| | |_) | |_| | | (_) | | | |  __/
 *   \___/| .__/ \__|_|  \___/|_| |_|\___|
 *        |_|
 *
 *  Optrone is a simple argument parser for Anstro Pleuton's programs.
 *
 *  This software is licensed under the terms of MIT License.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  Credits where credit's due:
 *  - ASCII Art generated using https://www.patorjk.com/software/taag with font
 *    "Standard".
 */

#include "test_helper.hpp"

/**
 *  @brief  Generate combination of indices from @c min_index up to
 *          @c max_index, and run the function.
 *
 *  @tparam  func_type  Type of function.
 *  @tparam  args_type  Type of additional arguments to function.
 *  @param   combo      Current combination of indices.
 *  @param   min_index  Min size for indices (inclusive).
 *  @param   max_index  Max size for indices (exclusive).
 *  @param   depth      Current depth of recursion.
 *  @param   func       Function to call with combination.
 *  @param   args       Additional arguments for function call.
 */
template<typename func_type, typename ... args_type>
static constexpr auto generate_combo(
    std::vector<std::size_t> &combo,
    std::size_t               min_index,
    std::size_t               max_index,
    std::size_t               depth,
    func_type                 func,
    args_type &&...           args
)
{
    if (depth == 0)
    {
        func(combo, args ...);
        return;
    }

    for (std::size_t i = min_index; i < max_index; i++)
    {
        combo.emplace_back(i);
        generate_combo(combo, min_index, max_index, depth - 1, func, args ...);
        combo.pop_back();
    }
}

/**
 *  @brief  Generate multiple combination up to @c max_combos of indices from
 *          @c min_index up to @c max_index, and run the function.
 *
 *  @tparam  func_type   Type of function.
 *  @tparam  args_type   Type of additional arguments to function.
 *  @param   min_index   Min size for indices (inclusive).
 *  @param   max_index   Max size for indices (exclusive).
 *  @param   max_combos  Min combination (size) of indices (inclusive).
 *  @param   max_combos  Max combination (size) of indices (exclusive).
 *  @param   func        Function to call with combination.
 *  @param   args        Additional arguments for function call.
 */
template<typename func_type, typename ... args_type>
static constexpr auto run_combo(
    std::size_t     min_index,
    std::size_t     max_index,
    std::size_t     min_combos,
    std::size_t     max_combos,
    func_type       func,
    args_type &&... args
)
{
    std::vector<std::size_t> combo = {};
    for (std::size_t i = min_combos; i < max_combos; i++)
    {
        generate_combo(combo, min_index, max_index, i, func, args ...);
    }
}

/**
 *  @brief  Test 1: Option recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_1) {
    CT_BEGIN;

    // 4 tests in one go
    enum class test_type {
        unknown = -1,
        long_names_test,
        short_names_test,
        short_names_combined_test,
        microsoft_switch_test,
        max
    };

    auto test_type_to_string = [](test_type type)
    {
        switch (type)
        {
            case test_type::unknown:
                return "unknown"s;
            case test_type::long_names_test:
                return "long_names_test"s;
            case test_type::short_names_test:
                return "short_names_test"s;
            case test_type::short_names_combined_test:
                return "short_names_combined_test"s;
            case test_type::microsoft_switch_test:
                return "microsoft_switch_test"s;
            case test_type::max:
                return "max"s;
        }
        return ""s;
    };

    std::size_t long_names_count  = 4;
    std::size_t short_names_count = 4;
    std::size_t combo_count       = 2;

    if (extensive_test)
    {
        long_names_count  = 8;
        short_names_count = 8;
        combo_count       = 4;
    }

    // I dare you put 13 as value on all the above variable

    std::vector<const o::option_template *> options = {};

    // Add options with one long name
    for (std::size_t i = 0; i < long_names_count / 2; i++)
    {
        options.emplace_back(new o::option_template {
            .description = std::format(
                "Long name option recognition - {}", i),
            .long_names         = { std::format("long-name-{}", i) },
            .short_names        = {},
            .parameters         = {},
            .defaults_from_back = {}
        });
    }

    // Add one more option with n long names
    std::vector<std::string> flat_long_names = {};
    for (std::size_t i = long_names_count / 2; i < long_names_count; i++)
    {
        flat_long_names.emplace_back(std::format("long-name-{}", i));
    }

    options.emplace_back(new o::option_template {
        .description = std::format("Long name option recognition (flat) - {}",
            long_names_count / 2),
        .long_names         = flat_long_names,
        .short_names        = {},
        .parameters         = {},
        .defaults_from_back = {}
    });

    // Add options with one short name
    for (std::size_t i = 0; i < short_names_count / 2; i++)
    {
        options.emplace_back(new o::option_template {
            .description = std::format(
                "Short name option recognition - {}", i),
            .long_names         = {},
            .short_names        = { char ('a' + std::fmod(i, 25zu)) },
            .parameters         = {},
            .defaults_from_back = {}
        });
    }

    // Add one more option with n short names
    std::vector<char> flat_short_names = {};
    for (std::size_t i = short_names_count / 2; i < short_names_count; i++)
    {
        flat_short_names.emplace_back(char ('a' + std::fmod(i, 25zu)));
    }

    options.emplace_back(new o::option_template {
        .description = std::format("Short name option recognition (flat) - {}",
            short_names_count / 2),
        .long_names         = {},
        .short_names        = flat_short_names,
        .parameters         = {},
        .defaults_from_back = {}
    });

    // Everything is packed, unpack
    auto determine_option = [&](
        std::size_t index,
        std::size_t &option_index,
        std::size_t &short_name_index,
        std::size_t &long_name_index
    )
    {
        for (std::size_t i = 0; i < options.size(); i++)
        {
            auto option = options[i];
            if (!option->long_names.empty())
            {
                if (index < option->long_names.size())
                {
                    option_index    = i;
                    long_name_index = index;
                    return;
                }
                index -= option->long_names.size();
            }
            if (!option->short_names.empty())
            {
                if (index < option->short_names.size())
                {
                    option_index     = i;
                    short_name_index = index;
                    return;
                }
                index -= option->short_names.size();
            }
        }
    };

    // Add long name to test
    auto prepare_long_name = [&](
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected,
        std::size_t test_index,
        std::size_t index,
        std::size_t option_index,
        std::size_t long_name_index,
        std::size_t short_name_index,
        bool &error
    )
    {
        auto option = options[option_index];

        if (long_name_index >= option->long_names.size())
        {
            logln("Test preparation failed for test {}: Invalid "
                "`long_name_index`: {} out of range [0,{}) for `option_index`: "
                "{} and `index`: {}", test_index, long_name_index,
                option->long_names.size(), option_index, index);
            error = true;
            return;
        }

        auto        long_name = option->long_names[long_name_index];
        std::string arg       = std::format("--{}", long_name);
        args.emplace_back(arg);

        o::parsed_argument expect = {
            .argument         = {
                .original     = {
                    .text     = arg,
                    .position = 2,
                    .size     = arg.size() - 2,
                },
                .modified     = {
                    .text     = arg,
                    .position = 2,
                    .size     = arg.size() - 2
                },
                .arg_type     = at::long_option
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = option,
            .ref_subcommand = nullptr,
            .values         = {}
        };

        expected.emplace_back(expect);
        error = false;
    };

    // Add short name to test
    auto prepare_short_name = [&](
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected,
        std::size_t test_index,
        std::size_t index,
        std::size_t option_index,
        std::size_t long_name_index,
        std::size_t short_name_index,
        bool &error
    )
    {
        auto option = options[option_index];

        if (short_name_index >= option->short_names.size())
        {
            logln("Test preparation failed for test {}: Invalid "
                "`short_name_index`: {} out of range [0,{}) for `option_index`:"
                " {} and `index`: {}", test_index, short_name_index,
                option->short_names.size(), option_index, index);
            error = true;
            return;
        }

        auto        short_name = option->short_names[short_name_index];
        std::string arg        = std::format("-{}", short_name);
        args.emplace_back(arg);

        o::parsed_argument expect = {
            .argument         = {
                .original     = {
                    .text     = arg,
                    .position = 1,
                    .size     = 1
                },
                .modified     = {
                    .text     = arg,
                    .position = 1,
                    .size     = 1
                },
                .arg_type     = at::short_option,
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = option,
            .ref_subcommand = nullptr,
            .values         = {}
        };

        expected.emplace_back(expect);
        error = false;
    };

    // Combine and add short name to temporary to test
    std::string combined_short_name_arg = "-";
    std::vector<const o::option_template *> combined_short_name_options = {};
    auto prepare_short_name_combined = [&](
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected,
        std::size_t test_index,
        std::size_t index,
        std::size_t option_index,
        std::size_t long_name_index,
        std::size_t short_name_index,
        bool &error
    )
    {
        auto option = options[option_index];

        if (short_name_index >= option->short_names.size())
        {
            logln("Test preparation failed for test {}: Invalid "
                "`short_name_index`: {} out of range [0,{}) for `option_index`:"
                " {} and `index`: {}", test_index, short_name_index,
                option->short_names.size(), option_index, index);
            error = true;
            return;
        }

        auto short_name          = option->short_names[short_name_index];
        combined_short_name_arg += short_name;
        combined_short_name_options.emplace_back(option);

        error = false;
    };

    // Add temporary to short name to test
    auto append_combined_short_name = [&](
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected,
        std::size_t test_index,
        bool &error
    )
    {
        if (combined_short_name_arg.size() - 1
         != combined_short_name_options.size())
        {
            logln("Test preparation failed for test {}: Unexpected size "
                "difference for `combined_short_name_arg` and "
                "`combined_short_name_options`: {} != {}", test_index,
                combined_short_name_arg.size() - 1,
                combined_short_name_options.size());
            error = true;
            combined_short_name_arg = "-";
            combined_short_name_options.clear();
            return;
        }

        args.emplace_back(combined_short_name_arg);
        for (std::size_t i = 0; i < combined_short_name_options.size(); i++)
        {
            auto &option = combined_short_name_options[i];
            o::parsed_argument expect = {
                .argument         = {
                    .original     = {
                        .text     = combined_short_name_arg,
                        .position = i + 1,
                        .size     = 1
                    },
                    .modified = {
                        .text = std::format("-{}",
                            combined_short_name_arg[i + 1]),
                        .position = 1,
                        .size     = 1
                    },
                    .arg_type     = at::short_option
                },
                .valid          = vdt::valid,
                .is_parsed      = true,
                .ref_option     = option,
                .ref_subcommand = nullptr,
                .values         = {}
            };
            expected.emplace_back(expect);
        }

        error = false;
        combined_short_name_arg = "-";
        combined_short_name_options.clear();
    };

    // Add Microsoft-style long and short name as switch to test
    auto prepare_microsoft_switch = [&](
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected,
        std::size_t test_index,
        std::size_t index,
        std::size_t option_index,
        std::size_t long_name_index,
        std::size_t short_name_index,
        bool &error,
        bool make_upper
    )
    {
        auto option = options[option_index];

        if (long_name_index < option->long_names.size())
        {
            auto        long_name = option->long_names[long_name_index];
            std::string arg       = std::format("/{}", long_name);
            if (make_upper) arg = sm::to_upper(arg);
            args.emplace_back(arg);

            o::parsed_argument expect = {
                .argument         = {
                    .original     = {
                        .text     = arg,
                        .position = 1,
                        .size     = arg.size() - 1
                    },
                    .modified     = {
                        .text     = arg,
                        .position = 1,
                        .size     = arg.size() - 1
                    },
                    .arg_type     = at::microsoft_switch
                },
                .valid          = vdt::valid,
                .is_parsed      = true,
                .ref_option     = option,
                .ref_subcommand = nullptr,
                .values         = {}
            };

            expected.emplace_back(expect);
            error = false;
            return;
        }

        if (short_name_index < option->short_names.size())
        {
            auto        short_name = option->short_names[short_name_index];
            std::string arg        = std::format("/{}", short_name);
            if (make_upper) arg = sm::to_upper(arg);
            args.emplace_back(arg);

            o::parsed_argument expect = {
                .argument         = {
                    .original     = {
                        .text     = arg,
                        .position = 1,
                        .size     = arg.size() - 1
                    },
                    .modified     = {
                        .text     = arg,
                        .position = 1,
                        .size     = arg.size() - 1
                    },
                    .arg_type     = at::microsoft_switch
                },
                .valid          = vdt::valid,
                .is_parsed      = true,
                .ref_option     = option,
                .ref_subcommand = nullptr,
                .values         = {}
            };

            expected.emplace_back(expect);
            error = false;
            return;
        }

        logln(
            "Test preparation failed for test {}: Invalid `long_name_index` and"
            " `short_name_index`: `long_name_index` ({}) out of range [0,{}) "
            "and `short_name_index` ({}) out of range [0,{}) for "
            "`option_index`: {} and `index`: {}", test_index, long_name_index,
            option->long_names.size(), short_name_index,
            option->short_names.size(), option_index, index);
        error = true;
    };

    // Run test with combo
    std::size_t test_index = 0;
    std::vector<std::size_t> failed_tests = {};
    auto combo = [&](
        const std::vector<std::size_t> &indices,
        test_type type
    )
    {
        test_index++;

        std::vector<std::string>        args     = {};
        std::vector<o::parsed_argument> expected = {};

        for (auto index : indices)
        {
            bool make_upper = false;
            if (type == test_type::microsoft_switch_test)
            {
                make_upper = index % 2;
                index      = index / 2;
            }

            std::size_t option_index     = (std::size_t)-1;
            std::size_t long_name_index  = (std::size_t)-1;
            std::size_t short_name_index = (std::size_t)-1;
            determine_option(index, option_index, short_name_index,
                long_name_index);

            if (option_index >= options.size())
            {
                logln("Test preparation failed for test {}: Invalid "
                    "`option_index`:  {} out of range [0,{}) for index: {}",
                    test_index, option_index, options.size(), index);
                return;
            }

            bool error = false;

            switch (type)
            {
                case test_type::long_names_test:
                    prepare_long_name(args, expected, test_index, index,
                        option_index, long_name_index, short_name_index, error);
                    break;
                case test_type::short_names_test:
                    prepare_short_name(args, expected, test_index, index,
                        option_index, long_name_index, short_name_index, error);
                    break;
                case test_type::short_names_combined_test:
                    prepare_short_name_combined(args, expected, test_index,
                        index, option_index, long_name_index, short_name_index,
                        error);
                    break;
                case test_type::microsoft_switch_test:
                    prepare_microsoft_switch(args, expected, test_index, index,
                        option_index, long_name_index, short_name_index, error,
                        make_upper);
                    break;
                default:
                    logln("Error: unknown test type for test {}: {}",
                        test_index, test_type_to_string(type));
                    return;
            }
            if (error) return;
        }

        if (type == test_type::short_names_combined_test)
        {
            bool error = false;
            append_combined_short_name(args, expected, test_index, error);
            if (error) return;
        }

        logln("--- Test 1.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 1.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    };

    try
    {
        run_combo(0, long_names_count * 2, combo_count, combo_count + 1, combo,
            test_type::long_names_test);
        run_combo(long_names_count * 2, long_names_count * 2 +
            short_names_count * 2, combo_count, combo_count + 1, combo,
            test_type::short_names_test);
        run_combo(long_names_count * 2, long_names_count * 2 +
            short_names_count * 2, combo_count + 1, combo_count + 2, combo,
            test_type::short_names_combined_test);
        run_combo(0, (long_names_count * 2 + short_names_count * 2) * 2,
            combo_count, combo_count + 1, combo,
            test_type::microsoft_switch_test);
        logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
            " Test 1."));
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_1: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_1");
    }

    for (auto &option : options)
    {
        delete option;
    }

    CT_END;
}
