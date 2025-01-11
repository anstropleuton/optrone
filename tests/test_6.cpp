/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 6: Subcommand parameter recognition tests.
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
 *  @brief  Test 6: Subcommand parameter recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_6) {
    CT_BEGIN;

    // Actual size this time
    std::size_t names_count = 2;
    std::size_t overflow    = 1;

    if (extensive_test)
    {
        names_count = 4;
        overflow    = 4;
    }

    std::vector<const o::subcommand_template *> subcommands = {};

    // Generate subcommands
    for (std::size_t i = 0; i < names_count; i++)
    {
        std::vector<std::string> parameters;
        for (std::size_t j = 0; j < i + 1; j++)
        {
            parameters.emplace_back(std::format("parameter-{}", j));
        }

        subcommands.emplace_back(new o::subcommand_template {
            .description = std::format("Subcommand parameter recognition"
                " - {}", i),
            .names              = { std::format("name-{}", i) },
            .parameters         = parameters,
            .defaults_from_back = {},
            .subcommands        = {},
            .subcommand_options = {}
        });
    }

    try
    {
        std::size_t test_index = 0;
        std::vector<std::size_t> failed_tests = {};

        // Test each subcommand
        for (std::size_t i = 0; i < subcommands.size(); i++)
        {
            auto subcommand = subcommands[i];

            // For each parameter
            for (std::size_t j = 0;
                 j < subcommand->parameters.size() + overflow; j++)
            {
                test_index++;

                std::vector<std::string>        args     = {};
                std::vector<o::parsed_argument> expected = {};

                auto arg = subcommand->names.front();
                args.emplace_back(arg);

                // Add n values per parameter requirements
                // Causes less parameters than required
                for (std::size_t k = 0; k < j; k++)
                {
                    args.emplace_back(std::format("value-{}", k));
                }

                vdt valid = j >= subcommand->parameters.size()
                            ? vdt::valid
                            : vdt::not_enough_values;

                o::parsed_argument expect = {
                    .argument         = {
                        .original     = {
                            .text     = arg,
                            .position = 0,
                            .size     = arg.size()
                        },
                        .modified     = {
                            .text     = arg,
                            .position = 0,
                            .size     = arg.size()
                        },
                        .arg_type     = at::regular_argument
                    },
                    .valid          = valid,
                    .is_parsed      = true,
                    .ref_option     = nullptr,
                    .ref_subcommand = subcommand,
                    .values         = {}
                };

                // Add values to expected
                for (std::size_t k = 0;
                     k < std::min(j, subcommand->parameters.size());
                     k++)
                {
                    expect.values.emplace_back(std::format("value-{}", k));
                }

                expected.emplace_back(expect);

                // Overflowed values are treated as subcommands, and is
                // unrecognized
                for (std::size_t k = subcommand->parameters.size();
                     k < j; k++)
                {
                    auto arg = std::format("value-{}", k);

                    o::parsed_argument expect = {
                        .argument         = {
                            .original     = {
                                .text     = arg,
                                .position = 0,
                                .size     = arg.size()
                            },
                            .modified     = {
                                .text     = arg,
                                .position = 0,
                                .size     = arg.size()
                            },
                            .arg_type     = at::regular_argument
                        },
                        .valid          = vdt::unrecognized_subcommand,
                        .is_parsed      = true,
                        .ref_option     = nullptr,
                        .ref_subcommand = nullptr,
                        .values         = {}
                    };

                    expected.emplace_back(expect);
                }

                logln("--- Test 6.{} ---", test_index);
                auto sub_errors = ap_tester(args, expected, {}, subcommands);
                logln("--- End of Test 6.{}, {} errors ---", test_index,
                    sub_errors);
                if (sub_errors != 0) failed_tests.emplace_back(test_index);
                errors += sub_errors;
            }
        }

        logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
            " Test 6."));
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_6: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_6");
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    CT_END;
}
