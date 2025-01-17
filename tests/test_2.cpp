/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 2: Subcommand recognition tests.
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
 *  @brief  Test 2: Subcommand recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_2) {
    CT_BEGIN;

    // Multiplied by 2
    std::size_t names_count = 2;

    if (extensive_test)
    {
        names_count = 4;
    }

    std::vector<const o::subcommand_template *> subcommands = {};

    // Add subcommands with one name
    for (std::size_t i = 0; i < names_count; i++)
    {
        subcommands.emplace_back(new o::subcommand_template {
            .description        = std::format("Subcommand recognition - {}", i),
            .names              = { std::format("name-{}", i) },
            .parameters         = {},
            .defaults_from_back = {},
            .subcommands        = {},
            .subcommand_options = {}
        });
    }

    // Add one more subcommand with n names
    std::vector<std::string> names = {};
    for (std::size_t i = names_count; i < names_count * 2; i++)
    {
        names.emplace_back(std::format("name-{}", i));
    }

    subcommands.emplace_back(new o::subcommand_template {
        .description = std::format("Subcommand recognition - {}",
            names_count),
        .names              = names,
        .parameters         = {},
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });

    // Test each name one by one
    try
    {
        std::size_t test_index = 0;
        std::vector<std::size_t> failed_tests = {};

        for (std::size_t i = 0; i < names_count * 2; i++)
        {
            test_index++;

            std::size_t index = i;
            std::vector<std::string>        args     = {};
            std::vector<o::parsed_argument> expected = {};
            std::size_t subcommand_index = (std::size_t)-1;
            std::size_t name_index       = (std::size_t)-1;

            for (std::size_t i = 0; i < subcommands.size(); i++)
            {
                auto subcommand = subcommands[i];
                if (index < subcommand->names.size())
                {
                    subcommand_index = i;
                    name_index       = index;
                    break;
                }
                index -= subcommand->names.size();
            }

            if (subcommand_index >= subcommands.size())
            {
                logln("Test preparation failed for test {}: Invalid "
                    "`subcommand_index`: {} out of range [0,{}) for index: {}",
                    test_index, subcommand_index, subcommands.size(), index);
                continue;
            }

            auto subcommand = subcommands[subcommand_index];

            if (name_index >= subcommand->names.size())
            {
                logln("Test preparation failed for test {}: Invalid "
                    "`name_index`: {} out of range [0,{}) for index: {}",
                    test_index, name_index, subcommand->names.size(), index);
                continue;
            }

            auto name = subcommand->names[name_index];
            args.emplace_back(name);

            o::parsed_argument expect = {
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

            logln("--- Test 2.{} ---", test_index);
            auto sub_errors = ap_tester(args, expected, {}, subcommands);
            logln("--- End of Test 2.{}, {} errors ---", test_index,
                sub_errors);
            if (sub_errors != 0) failed_tests.emplace_back(test_index);
            errors += sub_errors;
        }

        logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
            " Test 2."));
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_2: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_2");
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    CT_END;
}
