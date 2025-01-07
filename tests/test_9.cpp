/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 9: Argument split tests.
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
 *  @brief  Test 9: Argument split tests.
 *  @return  Number of errors.
 *
 *  @todo  Split these tests into even smaller functions.
 */
[[nodiscard]] CT_TESTER_FN(test_9)
{
    CT_BEGIN;

    std::vector<const o::option_template *> options = {};

    options.emplace_back(new o::option_template {
        .description        = "Option split test",
        .long_names         = { "name" },
        .short_names        = { 'a' },
        .parameters         = { "parameter" },
        .defaults_from_back = {}
    });

    std::vector<std::string>         args     = {};
    std::vector<o::parsed_argument> expected = {};

    std::size_t test_index = 0;
    std::vector<std::size_t> failed_tests = {};

    args.emplace_back("--name=value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "--name=value",
            .modified = "--name",
            .arg_type = at::long_option,
            .org_pos  = 2,
            .org_size = 4,
            .mod_pos  = 2,
            .mod_size = 4
        },
        .valid          = vdt::valid,
        .is_parsed      = true,
        .ref_option     = options.front(),
        .ref_subcommand = nullptr,
        .values         = { "value" }
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    args.emplace_back("-a=value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "-a=value",
            .modified = "-a",
            .arg_type = at::short_option,
            .org_pos  = 1,
            .org_size = 1,
            .mod_pos  = 1,
            .mod_size = 1
        },
        .valid          = vdt::valid,
        .is_parsed      = true,
        .ref_option     = options.front(),
        .ref_subcommand = nullptr,
        .values         = { "value" }
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    args.emplace_back("/name:value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "/name:value",
            .modified = "/name",
            .arg_type = at::microsoft_switch,
            .org_pos  = 1,
            .org_size = 4,
            .mod_pos  = 1,
            .mod_size = 4
        },
        .valid          = vdt::valid,
        .is_parsed      = true,
        .ref_option     = options.front(),
        .ref_subcommand = nullptr,
        .values         = { "value" }
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    args.emplace_back("/a:value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "/a:value",
            .modified = "/a",
            .arg_type = at::microsoft_switch,
            .org_pos  = 1,
            .org_size = 1,
            .mod_pos  = 1,
            .mod_size = 1
        },
        .valid          = vdt::valid,
        .is_parsed      = true,
        .ref_option     = options.front(),
        .ref_subcommand = nullptr,
        .values         = { "value" }
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    args.emplace_back("--name:value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "--name:value",
            .modified = "--name:value",
            .arg_type = at::long_option,
            .org_pos  = 2,
            .org_size = 10,
            .mod_pos  = 2,
            .mod_size = 10
        },
        .valid          = vdt::unrecognized_option,
        .is_parsed      = true,
        .ref_option     = nullptr,
        .ref_subcommand = nullptr,
        .values         = {}
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    args.emplace_back("/name=value");
    expected.emplace_back(o::parsed_argument {
        .argument     = {
            .original = "/name=value",
            .modified = "/name=value",
            .arg_type = at::microsoft_switch,
            .org_pos  = 1,
            .org_size = 10,
            .mod_pos  = 1,
            .mod_size = 10
        },
        .valid          = vdt::unrecognized_option,
        .is_parsed      = true,
        .ref_option     = nullptr,
        .ref_subcommand = nullptr,
        .values         = {}
    });

    try
    {
        test_index++;
        logln("--- Test 9.{} ---", test_index);
        auto sub_errors = ap_tester(args, expected, options, {});
        logln("--- End of Test 9.{}, {} errors ---", test_index,
            sub_errors);
        if (sub_errors != 0) failed_tests.emplace_back(test_index);
        errors += sub_errors;
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred in test_9: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred in test_9");
    }

    args.clear();
    expected.clear();

    for (auto &option : options)
    {
        delete option;
    }

    logln("Failed tests:\n{}\n", sm::to_string(failed_tests, ",\n"s,
        " Test 9."));

    CT_END;
}
