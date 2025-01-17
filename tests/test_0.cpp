/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Test 0: No arguments test.
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
 *  @brief  Test 0: No arguments test.
 *  @return  Number of errors.
 */
[[nodiscard]] CT_TESTER_FN(test_0)
{
    CT_BEGIN;

    const char *argv[] = { "./program" };
    int         argc   = lenof(argv);

    std::vector<std::string>         args(argv + 1, argv + argc);
    std::vector<o::parsed_argument> expected = {};

    auto parsed_1 = o::parse_arguments(argc, argv, {}, {});
    auto parsed_2 = o::parse_arguments(args,       {}, {});

    logln("args: {}", sm::to_string(args));
    logln("parsed_1:\n{}\n",
        sm::to_string(parsed_1, arg_to_string, "\n"));
    logln("parsed_2:\n{}\n",
        sm::to_string(parsed_2, arg_to_string, "\n"));
    logln("expected:\n{}\n",
        sm::to_string(expected, arg_to_string, "\n"));

    CT_ASSERT_CTR(parsed_1, expected);
    CT_ASSERT_CTR(parsed_2, expected);

    CT_END;
}
