/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 10: Variadic parameter recognition tests.
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
 *  @brief  Test 10: Variadic parameter recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_10) {
    CT_BEGIN;

    // Prepare options
    std::vector<const o::option_template *> options = {};

    options.emplace_back(new o::option_template {
        .description        = "Not Variadic Option",
        .long_names         = { "not-variadic" },
        .short_names        = { 'a' },
        .parameters         = { "parameter" },
        .defaults_from_back = {}
    });

    options.emplace_back(new o::option_template {
        .description        = "Zero-Or-More Variadic Option",
        .long_names         = { "zero-or-more" },
        .short_names        = { 'b' },
        .parameters         = { "..." },
        .defaults_from_back = {}
    });

    options.emplace_back(new o::option_template {
        .description        = "One-Or-More Variadic Option",
        .long_names         = { "one-or-more" },
        .short_names        = { 'c' },
        .parameters         = { "parameter..." },
        .defaults_from_back = {}
    });

    try
    {
        std::size_t test_index = 0;
        std::vector<std::size_t> failed_tests = {};

        // Triangle numbers reappears
        // For each options, test zero, one and two arguments
        for (std::size_t i = 0; i < options.size(); i++)
        {
            for (std::size_t j = 0; j < 3; j++)
            {
                test_index++;

                std::vector<std::string>        args     = {};
                std::vector<o::parsed_argument> expected = {};

                args.emplace_back("--" + options[i]->long_names.front());

                for (std::size_t k = 0; k < j; k++)
                {
                    args.emplace_back(std::format("value-{}", k));
                }

                // Validity and values based on variadicity
                vdt valid = vdt::valid;
                o::variadicity variadicity = o::is_parameter_variadic(
                    options[i]->parameters.back());
                std::vector<std::string> values = {};

                switch (variadicity)
                {
                    case o::variadicity::not_variadic:
                        if (args.size() < 2) valid = vdt::not_enough_values;
                        else values.emplace_back(args[1]);
                        break;
                    case o::variadicity::zero_or_more:
                        values = cu::subordinate(args, 1, args.size());
                        break;
                    case o::variadicity::one_or_more:
                        if (args.size() < 2) valid = vdt::not_enough_values;
                        else values = cu::subordinate(args, 1, args.size());
                        break;
                }

                expected.emplace_back(o::parsed_argument {
                    .argument         = {
                        .original     = {
                            .text     = "--" + options[i]->long_names.front(),
                            .position = 2,
                            .size     = options[i]->long_names.front().size()
                        },
                        .modified     = {
                            .text     = "--" + options[i]->long_names.front(),
                            .position = 2,
                            .size     = options[i]->long_names.front().size()
                        },
                        .arg_type     = at::long_option
                    },
                    .valid          = valid,
                    .is_parsed      = true,
                    .ref_option     = options[i],
                    .ref_subcommand = nullptr,
                    .values         = values
                });

                // Add leftover values as unrecognized subcommand
                if (variadicity == o::variadicity::not_variadic)
                {
                    for (std::size_t k = 2; k < args.size(); k++)
                    {
                        expected.emplace_back(o::parsed_argument {
                            .argument         = {
                                .original     = {
                                    .text     = args[k],
                                    .position = 0,
                                    .size     = args[k].size()
                                },
                                .modified     = {
                                    .text     = args[k],
                                    .position = 0,
                                    .size     = args[k].size()
                                },
                                .arg_type     = at::regular_argument
                            },
                            .valid          = vdt::unrecognized_subcommand,
                            .is_parsed      = true,
                            .ref_option     = nullptr,
                            .ref_subcommand = nullptr,
                            .values         = {}
                        });
                    }
                }

                logln("--- Test 10.{} ---", test_index);
                auto sub_errors = ap_tester(args, expected, options, {});
                logln("--- End of Test 10.{}, {} errors ---", test_index,
                    sub_errors);
                if (sub_errors != 0) failed_tests.emplace_back(test_index);
                errors += sub_errors;
            }
        }

        logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
            " Test 10."));
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_10: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_10");
    }

    for (auto &option : options)
    {
        delete option;
    }

    CT_END;
}
