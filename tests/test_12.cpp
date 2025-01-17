/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 12: Misc. edge case tests.
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

#include <vector>

#include "test_helper.hpp"

/**
 *  @brief  Test 12: Misc. edge case tests.
 *  @return  Number of errors.
 *
 *  @todo  Split these tests into even smaller functions.
 */
[[nodiscard]] CT_TESTER_FN(test_12) {
    CT_BEGIN;

    std::vector<const o::option_template *>     options     = {};
    std::vector<const o::subcommand_template *> subcommands = {};

    std::size_t test_index = 0;
    std::vector<std::size_t> failed_tests = {};

    std::vector<std::string>        args     = {};
    std::vector<o::parsed_argument> expected = {};

    // Test `--arg value subcommand`
    options.emplace_back(new o::option_template {
        .description        = "Option test `--arg value subcommand`",
        .long_names         = { "arg" },
        .short_names        = {},
        .parameters         = { "parameter" },
        .defaults_from_back = {}
    });

    subcommands.emplace_back(new o::subcommand_template {
        .description        = "Subcommand test `--arg value subcommand`",
        .names              = { "subcommand" },
        .parameters         = {},
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });

    args     = { "--arg", "value", "subcommand" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "--arg",
                    .position = 2,
                    .size     = 3
                },
                .modified     = {
                    .text     = "--arg",
                    .position = 2,
                    .size     = 3
                },
                .arg_type     = at::long_option
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = options.front(),
            .ref_subcommand = nullptr,
            .values         = { "value" }
        },
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "subcommand",
                    .position = 0,
                    .size     = 10
                },
                .modified     = {
                    .text     = "subcommand",
                    .position = 0,
                    .size     = 10
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = subcommands.front(),
            .values         = {         }
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    // Test `subcommand-1 value subcommand-2`
    subcommands.emplace_back(new o::subcommand_template {
        .description = "Subcommand test - 1 `subcommand-1 value "
                       "subcommand-2`",
        .names              = { "subcommand-1" },
        .parameters         = { "parameter" },
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });
    subcommands.emplace_back(new o::subcommand_template {
        .description = "Subcommand test - 2 `subcommand-1 value "
                       "subcommand-2`",
        .names              = { "subcommand-2" },
        .parameters         = {},
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });

    args     = { "subcommand-1", "value", "subcommand-2" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "subcommand-1",
                    .position = 0,
                    .size     = 12
                },
                .modified     = {
                    .text     = "subcommand-1",
                    .position = 0,
                    .size     = 12
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = subcommands[0],
            .values         = { "value" }
        },
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "subcommand-2",
                    .position = 0,
                    .size     = 12
                },
                .modified     = {
                    .text     = "subcommand-2",
                    .position = 0,
                    .size     = 12
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = subcommands[1],
            .values         = {         }
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    // Test argument starting with `=`
    args     = { "=" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "=",
                    .position = 0,
                    .size     = 1
                },
                .modified     = {
                    .text     = "=",
                    .position = 0,
                    .size     = 1
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::unrecognized_subcommand,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = nullptr,
            .values         = {}
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    // Test `--arg value-1=value-2`
    options.emplace_back(new o::option_template {
        .description        = "Option test `--arg value-1=value-2`",
        .long_names         = { "arg" },
        .short_names        = {},
        .parameters         = { "parameter" },
        .defaults_from_back = {}
    });

    args     = { "--arg", "value-1=value-2" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "--arg",
                    .position = 2,
                    .size     = 3
                },
                .modified     = {
                    .text     = "--arg",
                    .position = 2,
                    .size     = 3
                },
                .arg_type     = at::long_option
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = options.front(),
            .ref_subcommand = nullptr,
            .values         = { "value-1=value-2" }
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    // Test `global-subcommand-1 global-subcommand-2` even when
    // `global-subcommand-1` has subcommand of its own
    subcommands.emplace_back(new o::subcommand_template {
        .description = "Subcommand test - 1 `global-subcommand-1 "
                       "global-subcommand-2`",
        .names              = { "global-subcommand-1" },
        .parameters         = {},
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });
    subcommands.emplace_back(new o::subcommand_template {
        .description = "Subcommand test - 2 `global-subcommand-1 "
                       "global-subcommand-2`",
        .names              = { "global-subcommand-2" },
        .parameters         = {},
        .defaults_from_back = {},
        .subcommands        = {},
        .subcommand_options = {}
    });

    args     = { "global-subcommand-1", "global-subcommand-2" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "global-subcommand-1",
                    .position = 0,
                    .size     = 19
                },
                .modified     = {
                    .text     = "global-subcommand-1",
                    .position = 0,
                    .size     = 19
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = subcommands[0],
            .values         = {}
        },
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "global-subcommand-2",
                    .position = 0,
                    .size     = 19
                },
                .modified     = {
                    .text     = "global-subcommand-2",
                    .position = 0,
                    .size     = 19
                },
                .arg_type     = at::regular_argument
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = nullptr,
            .ref_subcommand = subcommands[1],
            .values         = {}
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    // Test `--arg-1 value --arg-2` even when `--arg-1` has two parameters
    options.emplace_back(new o::option_template {
        .description        = "Option test `--arg-1 value --arg-2`",
        .long_names         = { "arg-1" },
        .short_names        = {},
        .parameters         = { "parameter-1", "parameter-2" },
        .defaults_from_back = {}
    });

    options.emplace_back(new o::option_template {
        .description        = "Option test `--arg-1 value --arg-2`",
        .long_names         = { "arg-2" },
        .short_names        = {},
        .parameters         = {},
        .defaults_from_back = {}
    });

    args     = { "--arg-1", "value", "--arg-2" };
    expected = {
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "--arg-1",
                    .position = 2,
                    .size     = 5
                },
                .modified     = {
                    .text     = "--arg-1",
                    .position = 2,
                    .size     = 5
                },
                .arg_type     = at::long_option
            },
            .valid          = vdt::not_enough_values,
            .is_parsed      = true,
            .ref_option     = options[0],
            .ref_subcommand = nullptr,
            .values         = { "value" }
        },
        o::parsed_argument {
            .argument         = {
                .original     = {
                    .text     = "--arg-2",
                    .position = 2,
                    .size     = 5
                },
                .modified     = {
                    .text     = "--arg-2",
                    .position = 2,
                    .size     = 5
                },
                .arg_type     = at::long_option
            },
            .valid          = vdt::valid,
            .is_parsed      = true,
            .ref_option     = options[1],
            .ref_subcommand = nullptr,
            .values         = {         }
        }
    };

    try
    {
        test_index++;
        logln("--- Test 12.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, subcommands);
        logln("--- End of Test 12.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_12: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_12");
    }

    for (auto &option : options)
    {
        delete option;
    }

    for (auto &subcommand : subcommands)
    {
        delete subcommand;
    }

    options.clear();
    subcommands.clear();

    logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
        " Test 12."));

    CT_END;
}
