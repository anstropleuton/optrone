/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test Optrone.
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

#include <exception>

#include "confer.hpp"

/**
 *  @brief  Test 0: No arguments test.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_0);

/**
 *  @brief  Test 1: Option recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_1);

/**
 *  @brief  Test 2: Subcommand recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_2);

/**
 *  @brief  Test 3: Nested subcommand recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_3);

/**
 *  @brief  Test 4: Nested option recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_4);

/**
 *  @brief  Test 5: Option parameter recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_5);

/**
 *  @brief  Test 6: Subcommand parameter recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_6);

/**
 *  @brief  Test 7: Option parameter with defaults recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_7);

/**
 *  @brief  Test 8: Subcommand parameter with defaults recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_8);

/**
 *  @brief  Test 9: Argument split tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_9);

/**
 *  @brief  Test 10: Variadic parameter recognition tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_10);

/**
 *  @brief  Test 11: Throw tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_11);

/**
 *  @brief  Test 12: Misc. edge case tests.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_12);

/**
 *  @brief  Test Optrone.
 *  @return  Zero on success.
 */
auto main() -> int
{
    test_suite suite;
    suite.pre_run  = default_pre_runner('=', 5);
    suite.post_run = default_post_runner('=', 5);
    // suite.run_failed = default_run_failed_quitter();

    // Scary memory management

    suite.tests.emplace_back(new test {
        "Test 0: No arguments test",
        "test_0",
        test_0
    });

    suite.tests.emplace_back(new test {
        "Test 1: Option recognition tests",
        "test_1",
        test_1
    });

    suite.tests.emplace_back(new test {
        "Test 2: Subcommand recognition tests",
        "test_2",
        test_2
    });

    suite.tests.emplace_back(new test {
        "Test 3: Nested subcommand recognition tests",
        "test_3",
        test_3
    });

    suite.tests.emplace_back(new test {
        "Test 4: Nested option recognition tests",
        "test_4",
        test_4
    });

    suite.tests.emplace_back(new test {
        "Test 5: Option parameter recognition tests",
        "test_5",
        test_5
    });

    suite.tests.emplace_back(new test {
        "Test 6: Subcommand parameter recognition tests",
        "test_6",
        test_6
    });

    suite.tests.emplace_back(new test {
        "Test 7: Option parameter with defaults recognition tests",
        "test_7",
        test_7
    });

    suite.tests.emplace_back(new test {
        "Test 8: Subcommand parameter with defaults recognition tests",
        "test_8",
        test_8
    });

    suite.tests.emplace_back(new test {
        "Test 9: Argument split tests",
        "test_9",
        test_9
    });

    suite.tests.emplace_back(new test {
        "Test 10: Variadic parameter recognition tests",
        "test_10",
        test_10
    });

    suite.tests.emplace_back(new test {
        "Test 11: Throw tests",
        "test_11",
        test_11
    });

    suite.tests.emplace_back(new test {
        "Test 12: Misc. edge case tests",
        "test_12",
        test_12
    });

    std::size_t errors = (std::size_t)-1;
    try
    {
        auto failed_tests = suite.run();

        print_failed_tests(failed_tests);
        errors = sum_failed_tests_errors(failed_tests);
    }
    catch (const std::exception &e)
    {
        logln("Exception occurred during test: {}", e.what());
    }
    catch (...)
    {
        logln("Unknown exception occurred during test");
    }

    for (auto &test : suite.tests)
    {
        delete test;
    }

    return errors != 0;
}
