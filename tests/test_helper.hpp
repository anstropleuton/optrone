/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Helpers to test Optrone.
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

#pragma once

/**
 *  @brief  Default value for extensive tester.
 *  @see  extensive_test.
 */
#ifndef EXTENSIVE_TEST_VALUE
#define EXTENSIVE_TEST_VALUE false
#endif // EXTENSIVE_TEST_VALUE

/**
 *  @brief  Extensive test performs WAY TOO FUCKING MANY tests for each test.
 *          Be prepared to flood your terminal with test logs.  This is mostly
 *          redundant tests.
 */
[[maybe_unused]] static bool extensive_test = EXTENSIVE_TEST_VALUE;

/**
 *  @brief  Nothing... Just C-style array length expression that "just works".
 *          This is the peak of C-like code you will ever see in this library.
 *          ... maybe.
 */
#define lenof(array_expr) sizeof (array_expr) / sizeof (array_expr)[0]

#include <format>
#include <print>
#include <string>
#include <vector>

#include "alcelin_string_manipulators.hpp"
#include "confer.hpp"
#include "optrone.hpp"

using namespace alcelin;
using namespace sm_operators;
using namespace std::string_literals;

namespace o = optrone;
using at    = o::argument_type;
using vdt   = o::validity;

/**
 *  @brief  Convert option template to string.
 *
 *  @param  option_template  Option template.
 *  @return  String representing option template.
 */
[[nodiscard]] static inline constexpr auto option_to_string(
    const o::option_template &option_template
)
{
    return std::format(
        "  description: {}\n"
        "  long names: {}\n"
        "  short names: {}\n"
        "  parameters: {}\n"
        "  defaults: {}\n",
        option_template.description,
        sm::to_string(option_template.long_names),
        sm::to_string(option_template.short_names),
        sm::to_string(option_template.parameters),
        sm::to_string(option_template.defaults_from_back)
    );
}

/**
 *  @brief  Convert parsed argument to string.
 *
 *  @param  parsed_argument  Parsed Argument.
 *  @return  String representing parsed argument.
 */
[[nodiscard]] static inline constexpr auto arg_to_string(
    const o::parsed_argument &parsed_argument
)
{
    return std::format(
        "  arg: type: {}\n"
        "    original: {}\n"
        "              {}\n"
        "    modified: {}\n"
        "              {}\n"
        "  validity: {}\n"
        "  parsed? {}\n"
        "  option: {}\n"
        "  subcommand: {}\n"
        "  values: {}\n",
        o::to_string(parsed_argument.argument.arg_type),
        parsed_argument.argument.original.text,
        parsed_argument.argument.original.squiggle(),
        parsed_argument.argument.modified.text,
        parsed_argument.argument.modified.squiggle(),
        o::to_string(parsed_argument.valid),
        parsed_argument.is_parsed,
        parsed_argument.ref_option
            ? parsed_argument.ref_option->description
            : "(null)"s,
        parsed_argument.ref_subcommand
            ? parsed_argument.ref_subcommand->description
            : "(null)"s,
        sm::to_string(parsed_argument.values)
    );
}

/**
 *  @brief  Formatter for parsed argument.
 */
template<>
struct std::formatter<o::parsed_argument> {

    /**
     *  @brief  Parse format specifier
     *
     *  @tparam  parse_context  Parsing context type.
     *  @param   pc             Parsing context.
     *  @return  Iterator to end of format specifier.
     */
    template<typename parse_context>
    [[nodiscard]] inline constexpr auto parse(parse_context &pc)
    {
        auto it = pc.begin();
        if (it == pc.end())
        {
            return it;
        }

        // Add format specifiers?

        if (it != pc.end() && *it != '}')
        {
            throw std::format_error("Invalid format for o::parsed_argument");
        }

        return it;
    }

    /**
     *  @brief  Format the object and convert to string.
     *
     *  @tparam  format_context   Formatting context type.
     *  @param   parsed_argument  Object to convert to string.
     *  @param   fc               Formatting context.
     *  @return  Formatted string representing the object.
     */
    template<typename format_context>
    [[nodiscard]] inline constexpr auto format(
        const o::parsed_argument &parsed_argument, format_context &fc) const
    {
        return std::format_to(fc.out(), "\"{}\" (\"{}\")",
            parsed_argument.argument.original.text,
            parsed_argument.argument.modified.text);
    }
};

/**
 *  @brief  Compare all values of two parsed argument.
 *
 *  @param  a  First parsed argument.
 *  @param  b  Second parsed argument.
 *  @return  True if they are equal.
 */
[[nodiscard]] static inline constexpr auto operator== (
    const o::parsed_argument &a,
    const o::parsed_argument &b
)
{
    return a.argument.original.text == b.argument.original.text
        && a.argument.original.position == b.argument.original.position
        && a.argument.original.size == b.argument.original.size
        && a.argument.modified.text == b.argument.modified.text
        && a.argument.modified.position == b.argument.modified.position
        && a.argument.modified.size == b.argument.modified.size
        && a.argument.arg_type == b.argument.arg_type
        && a.valid == b.valid
        && a.is_parsed == b.is_parsed
        && a.ref_option == b.ref_option
        && a.ref_subcommand == b.ref_subcommand
        && a.values == b.values;
}

/**
 *  @brief  Helper to test AP with args, options and subcommands.
 *
 *  @param  args      Arguments to test against.
 *  @param  expected  Expected parsed results to compare with.
 *  @return  Number of errors.
 */
[[nodiscard, maybe_unused]] static inline constexpr auto ap_tester(
    const std::vector<std::string>                   &args,
    const std::vector<o::parsed_argument>            &expected,
    const std::vector<const o::option_template *>     options,
    const std::vector<const o::subcommand_template *> subcommands
) -> std::size_t
{
    CT_BEGIN;

    auto parsed = o::parse_arguments(args, options, subcommands);

    logln("args: {}",        sm::to_string(args));
    logln("parsed:\n{}\n",   sm::to_string(parsed, arg_to_string, "\n"));
    logln("expected:\n{}\n", sm::to_string(expected, arg_to_string, "\n"));

    CT_ASSERT_CTR(parsed, expected);

    CT_END;
}
