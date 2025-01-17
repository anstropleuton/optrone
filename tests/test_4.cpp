/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 4: Nested option recognition tests.
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

namespace stdr = std::ranges;

/**
 *  @brief  Test 4: Nested option recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_4) {
    CT_BEGIN;

    std::size_t nest  = 2;
    std::size_t count = 2;

    if (extensive_test)
    {
        nest  = 4;
        count = 4;
    }

    std::vector<const o::subcommand_template *> subcommands;

    // Generate nested subcommands hierarchy
    std::function<void (o::subcommand_template *, std::size_t, std::size_t &)>
    generate_subcommands;
    generate_subcommands = [&](
        o::subcommand_template *subcommand,
        std::size_t nesting,
        std::size_t &name_counter
    )
    {
        *subcommand      = o::subcommand_template {
            .description = std::format("Nested subcommand recognition - {}",
                name_counter),
            .names              = { std::format("name-{}", name_counter) },
            .parameters         = {},
            .defaults_from_back = {},
            .subcommands        = {},
            .subcommand_options = {}
        };

        subcommand->subcommand_options.emplace_back(new o::option_template {
            .description = std::format(
                "Nested subcommand option recognition - {}", name_counter),
            .long_names         = { std::format("long-name-{}", name_counter) },
            .short_names        = {},
            .parameters         = {},
            .defaults_from_back = {}
        });
        name_counter++;

        if (nesting == 0)
        {
            return;
        }

        for (std::size_t i = 0; i < nesting; i++)
        {
            o::subcommand_template *child = new o::subcommand_template;
            generate_subcommands(child, nesting - 1, name_counter);
            subcommand->subcommands.emplace_back(child);
        }
    };

    // Generate n-times
    std::size_t name_counter = 0;
    for (std::size_t i = 0; i < count; i++)
    {
        o::subcommand_template *subcommand = new o::subcommand_template;
        generate_subcommands(subcommand, nest, name_counter);
        subcommands.emplace_back(subcommand);
    }

    // Find the nested subcommand name for test
    std::function<bool (o::subcommand_template *, std::size_t &, std::size_t,
        std::vector<std::string> &, std::vector<o::parsed_argument> &)>
    find_nth_name;
    find_nth_name = [&](
        o::subcommand_template *subcommand,
        std::size_t &current_index,
        std::size_t target_index,
        std::vector<std::string> &args,
        std::vector<o::parsed_argument> &expected
    )
    {
        for (auto &name : subcommand->names)
        {
            if (current_index == target_index)
            {
                // Order is reversed
                std::string arg = std::format("--long-{}", name);
                args.emplace_back(arg);

                o::parsed_argument expect = {
                    .argument         = {
                        .original     = {
                            .text     = arg,
                            .position = 2,
                            .size     = arg.size() - 2
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
                    .ref_option     = subcommand->subcommand_options.front(),
                    .ref_subcommand = nullptr,
                    .values         = {}
                };
                expected.emplace_back(expect);

                args.emplace_back(name);

                expect                = {
                    .argument         = {
                        .original     = {
                            .text     = name,
                            .position = 0,
                            .size     = name.size()
                        },
                        .modified     = {
                            .text     = name,
                            .position = 0,
                            .size     = name.size()
                        },
                        .arg_type     = at::regular_argument
                    },
                    .valid          = vdt::valid,
                    .is_parsed      = true,
                    .ref_option     = nullptr,
                    .ref_subcommand = subcommand,
                    .values         = {}
                };
                expected.emplace_back(expect);

                return true;
            }
            current_index++;
        }

        for (auto &child : subcommand->subcommands)
        {
            if (find_nth_name((o::subcommand_template *)child, current_index,
                target_index, args, expected))
            {
                auto name = subcommand->names.front();

                // Order is reversed
                std::string arg = std::format("--long-{}", name);
                args.emplace_back(arg);

                o::parsed_argument expect = {
                    .argument         = {
                        .original     = {
                            .text     = arg,
                            .position = 2,
                            .size     = arg.size() - 2
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
                    .ref_option     = subcommand->subcommand_options.front(),
                    .ref_subcommand = nullptr,
                    .values         = {}
                };
                expected.emplace_back(expect);

                args.emplace_back(name);

                expect                = {
                    .argument         = {
                        .original     = {
                            .text     = name,
                            .position = 0,
                            .size     = name.size()
                        },
                        .modified     = {
                            .text     = name,
                            .position = 0,
                            .size     = name.size()
                        },
                        .arg_type     = at::regular_argument
                    },
                    .valid          = vdt::valid,
                    .is_parsed      = true,
                    .ref_option     = nullptr,
                    .ref_subcommand = subcommand,
                    .values         = {}
                };
                expected.emplace_back(expect);
                return true;
            }
        }

        return false;
    };

    try
    {
        std::size_t test_index = 0;
        std::vector<std::size_t> failed_tests = {};

        // Test each nested subcommand
        for (std::size_t i = 0; i < name_counter; i++)
        {
            test_index++;

            std::size_t current_index = 0;
            std::vector<std::string>        args     = {};
            std::vector<o::parsed_argument> expected = {};
            bool found = false;
            for (auto &subcommand : subcommands)
            {
                if (find_nth_name((o::subcommand_template *)subcommand,
                    current_index, i, args, expected))
                {
                    found = true;
                    break;
                }
            }

            // Something definitely went wrong
            if (!found)
            {
                logln("Test preparation failed for test {}: `i`th name "
                    "cannot be found, `current_index`: {}, `path`: {}",
                    test_index, i, current_index, sm::to_string(args));
            }

            stdr::reverse(args);
            stdr::reverse(expected);

            logln("--- Test 4.{} ---", test_index);
            auto sub_errors = ap_tester(args, expected, {}, subcommands);
            logln("--- End of Test 4.{}, {} errors ---", test_index,
                sub_errors);
            if (sub_errors != 0) failed_tests.emplace_back(test_index);
            errors += sub_errors;
        }

        logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
            " Test 4."));
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_4: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_4");
    }

    std::function<void (o::subcommand_template *)>
    release_subcommands;
    release_subcommands = [&](o::subcommand_template *subcommand)
    {
        for (auto &child : subcommand->subcommands)
        {
            release_subcommands((o::subcommand_template *)child);
        }
        for (auto &option : subcommand->subcommand_options)
        {
            delete option;
        }
        delete subcommand;
    };

    for (auto &subcommand : subcommands)
    {
        release_subcommands((o::subcommand_template *)subcommand);
    }

    CT_END;
}
