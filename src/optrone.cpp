/**
 *  @author  Anstro Pleuton (https://github.com/anstropleuton)
 *  @brief   Implementations for non-inline functions from
 *           @c optrone.hpp .
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

#include "optrone.hpp"

#include <print>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std::string_literals;

namespace o  = optrone;
namespace sm = alcelin::sm;
namespace cu = alcelin::cu;

using namespace alcelin::sm_operators;

/**
 *  @brief  Throw if options/switches are null.
 *
 *  @param  options  Options.
 *  @exception  std::invalid_argument  Thrown if options are null.
 */
static inline constexpr auto options_sanity_checker(
    const std::vector<const o::option_template *> &options
)
{
    for (std::size_t i = 0; i < options.size(); i++)
    {
        auto &option = options[i];
        if (!option)
        {
            throw std::invalid_argument(
                std::format("Option cannot be null (at index: {})", i)
            );
        }

        if (!option->parameters.empty())
        {
            for (std::size_t j = 0; j < option->parameters.size() - 1; j++)
            {
                if (o::is_parameter_variadic(option->parameters[j])
                 != o::variadicity::not_variadic)
                {
                    throw std::invalid_argument(
                        std::format("Option's non-last parameter cannot be "
                            "variadic (at index: {}, parameter index: {})", i,
                            j)
                    );
                }
            }
        }

        if (option->defaults_from_back.size() > option->parameters.size())
        {
            throw std::invalid_argument(
                std::format("Option cannot have more default values than "
                    "parameters (defaults: {}, parameters: {}, at index: {})",
                    option->defaults_from_back.size(),
                    option->parameters.size(), i)
            );
        }

        if (!option->parameters.empty()
         && !option->defaults_from_back.empty()
         && o::is_parameter_variadic(option->parameters.back())
         != o::variadicity::not_variadic)
        {
            throw std::invalid_argument(
                std::format("Option cannot have default values when last "
                    "parameter is variadic (at index: {})", i)
            );
        }
    }
}

/**
 *  @brief  Throw if subcommands or nested subcommands are null.
 *
 *  @param  subcommands  Subcommands.
 *  @param  level        Nesting level (optional).
 *  @exception  std::invalid_argument  Thrown if subcommands or nested
 *                                     subcommands are null.
 */
static inline constexpr auto subcommands_sanity_checker(
    const std::vector<const o::subcommand_template *> &subcommands,
    std::vector<std::size_t>                           nesting_indices
) -> void
{
    for (std::size_t i = 0; i < subcommands.size(); i++)
    {
        nesting_indices.emplace_back(i);
        auto &subcommand = subcommands[i];
        if (!subcommand)
        {
            throw std::invalid_argument(std::format(
                "Subcommand cannot be null (nesting indices: {})",
                sm::to_string(nesting_indices)
            ));
        }

        if (!subcommand->parameters.empty())
        {
            for (std::size_t j = 0; j < subcommand->parameters.size() - 1; j++)
            {
                if (o::is_parameter_variadic(subcommand->parameters[j])
                 != o::variadicity::not_variadic)
                {
                    throw std::invalid_argument(
                        std::format("Subcommand's non-last parameter cannot be "
                            "variadic (nesting indices: {}, parameter index: "
                            "{})", sm::to_string(nesting_indices), j)
                    );
                }
            }
        }

        if (subcommand->defaults_from_back.size()
          > subcommand->parameters.size())
        {
            throw std::invalid_argument(
                std::format("Subcommand cannot have more default values than "
                    "parameters (defaults: {}, parameters: {}, nesting indices:"
                    " {})", subcommand->defaults_from_back.size(),
                    subcommand->parameters.size(), i)
            );
        }

        if (!subcommand->parameters.empty()
         && !subcommand->defaults_from_back.empty()
         && o::is_parameter_variadic(subcommand->parameters.back())
         != o::variadicity::not_variadic)
        {
            throw std::invalid_argument(
                std::format("Subcommand cannot have default values when last "
                    "parameter is variadic (nesting indices: {})",
                    sm::to_string(nesting_indices))
            );
        }

        if (!subcommand->parameters.empty()
         && !subcommand->subcommands.empty()
         && o::is_parameter_variadic(subcommand->parameters.back())
         != o::variadicity::not_variadic)
        {
            throw std::invalid_argument(
                std::format("Subcommands cannot have nested subcommands when "
                    "last parameter is variadic (nesting indices: {})",
                    sm::to_string(nesting_indices))
            );
        }

        options_sanity_checker(subcommand->subcommand_options);
        subcommands_sanity_checker(subcommand->subcommands, nesting_indices);
        nesting_indices.pop_back();
    }
}

/**
 *  @brief  Check if argument matches the long names of the options/switches.
 *
 *  @param  options     Options.
 *  @param  long_name   Long name to match.
 *  @param  switch_ins  Whether to match Microsoft-style switches case
 *                      insensitively (optional).
 *  @return  Nullable pointer to matched option/switch.
 */
[[nodiscard]] static inline constexpr auto match_long_names(
    const std::vector<const o::option_template *> &options,
    std::string                                    long_name,
    bool                                           switch_ins = false
) -> const o::option_template *
{
    for (auto &option : options)
    {
        for (auto &option_long_name : option->long_names)
        {
            if (option_long_name == long_name ||
                (switch_ins && sm::is_equal_ins(option_long_name, long_name)))
            {
                return option;
            }
        }
    }

    return nullptr;
}

/**
 *  @brief  Check if argument matches the short names of the options/switches.
 *
 *  @param  options     Options
 *  @param  short_name  Short name to match.
 *  @param  switch_ins  Whether to match Microsoft-style switches case
 *                      insensitively (optional).
 *  @return  Nullable pointer to matched option/switch.
 */
[[nodiscard]] static inline constexpr auto match_short_names(
    const std::vector<const o::option_template *> &options,
    char                                           short_name,
    bool                                           switch_ins = false
) -> const o::option_template *
{
    for (auto &option : options)
    {
        for (auto &option_short_name : option->short_names)
        {
            if (option_short_name == short_name ||
                (switch_ins && sm::is_equal_ins(option_short_name, short_name)))
            {
                return option;
            }
        }
    }

    return nullptr;
}

/**
 *  @brief  Check if argument matches options/switches.
 *
 *  @param  arg         Argument to match options/switches.
 *  @param  arg_type    Argument type.
 *  @param  options     Options.
 *  @param  switch_ins  Whether to match Microsoft-style switches case
 *                      insensitively (optional).
 *  @return  Nullable pointer to matched option/switch.
 */
[[nodiscard]] static inline constexpr auto match_option(
    std::string                                   &arg,
    o::argument_type                               arg_type,
    const std::vector<const o::option_template *> &options,
    bool                                           switch_ins
) -> const o::option_template *
{
    if (arg_type == o::argument_type::long_option)
    {
        auto long_name = arg.substr(2);
        return match_long_names(options, long_name);
    }
    else if (arg_type == o::argument_type::short_option)
    {
        auto short_name = arg[1];
        return match_short_names(options, short_name);
    }
    // Match both long names and short names for Microsoft style argument
    // depending on the size of the argument
    else if (arg_type == o::argument_type::microsoft_switch)
    {
        if (arg.size() == 2)
        {
            auto short_name = arg[1];
            auto match      = match_short_names(options, short_name,
                switch_ins);
            if (match)
            {
                return match;
            }
        }

        auto long_name = arg.substr(1);
        auto match     = match_long_names(options, long_name, switch_ins);
        if (match)
        {
            return match;
        }
    }

    return nullptr;
}

/**
 *  @brief  Check if argument matches subordinate command.
 *
 *  @param  arg                  Argument.
 *  @param  current_subcommands  Currently handling nested subcommands.
 *  @return  Nullable pointer to matched subcommand.
 */
[[nodiscard]] static inline constexpr auto match_subcommand(
    std::string                                       arg,
    const std::vector<const o::subcommand_template *> current_subcommands
) -> const o::subcommand_template *
{
    for (auto &subs : current_subcommands)
    {
        for (auto &name : subs->names)
        {
            if (arg == name)
            {
                return subs;
            }
        }
    }

    return nullptr;
}

/**
 *  @brief  Collect arguments that are not option or switch, to use as values.
 *
 *  @param  i              Current argument index.
 *  @param  meta_args      All the arguments.
 *  @param  parameters     Parameters to collect.
 *  @param  default_args   Default values for unprovided parameters.
 *  @return  Collected values.
 */
[[nodiscard]] static inline constexpr auto collect_values(
    std::size_t                        &i,
    const std::vector<o::meta_argument> meta_args,
    const std::vector<std::string>     &parameters,
    const std::vector<std::string>     &default_args,
    o::validity                        &valid
)
{
    std::vector<std::string> collected_values = {};

    auto is_variadic = o::variadicity::not_variadic;
    if (!parameters.empty())
    {
        is_variadic = o::is_parameter_variadic(parameters.back());
    }

    // Refactor this code?  I added variadic feature after I made this
    valid         = o::validity::valid;
    std::size_t j = 0;

    if (is_variadic == o::variadicity::not_variadic)
    {
        for (; j < parameters.size() && i + j + 1 < meta_args.size(); j++)
        {
            auto &meta_arg = meta_args[i + j + 1];
            if (meta_arg.arg_type != o::argument_type::regular_argument
             && meta_arg.arg_type != o::argument_type::single_hyphen)
            {
                break;
            }

            collected_values.emplace_back(meta_arg.modified.text);
        }

        auto defaults_provided = j - parameters.size() + default_args.size();
        auto first = defaults_provided;
        auto last  = default_args.size();
        if (first < last)
        {
            auto defaults    = cu::subordinate(default_args, first, last);
            collected_values = cu::combine(collected_values, defaults);
        }

        // Validity check
        if (collected_values.size() != parameters.size())
        {
            valid = o::validity::not_enough_values;
        }
    }
    else
    {
        for (; i + j + 1 < meta_args.size(); j++)
        {
            auto &meta_arg = meta_args[i + j + 1];
            if (meta_arg.arg_type != o::argument_type::regular_argument
             && meta_arg.arg_type != o::argument_type::single_hyphen)
            {
                break;
            }

            collected_values.emplace_back(meta_arg.modified.text);
        }

        // Validity check
        switch (is_variadic)
        {
            case o::variadicity::not_variadic:
                break;
            case o::variadicity::zero_or_more:
                break;
            case o::variadicity::one_or_more:
                if (collected_values.size() < 1)
                {
                    valid = o::validity::not_enough_values;
                }
                break;
        }
    }

    i += j;
    return collected_values;
}

/**
 *  @brief  Parse command line arguments.
 *
 *  @see  Detailed Description of namespace @c ap.
 *
 *  @param  args         All excluding the first (usually program name) command
 *                       line arguments.
 *  @param  options      All options/switches.
 *  @param  subcommands  All subcommands.
 *  @param  switch_ins   Whether to match Microsoft-style switches case
 *                       insensitively (optional).
 *  @return  Parsed argument information.
 *
 *  @exception  std::invalid_argument  Thrown in the following cases:
 *   - When a pointer is null.
 *   - When there are more @c defaults_from_back than @c parameters.
 *   - When @c parameters is variadic, and default values are provided.
 *   - When @c parameters is variadic, and subcommands are provided.
 *   - When a non-last parameter is variadic.
 *
 *  @note  Do not pass dynamically allocated memory directly as @c options or
 *         @c subcommands.  This should point to user managed memory.
 */
[[nodiscard]] auto o::parse_arguments(
    const std::vector<std::string>                 &args,
    const std::vector<const option_template *>     &options,
    const std::vector<const subcommand_template *> &subcommands,
    bool                                            switch_ins
) -> std::vector<parsed_argument>
{
    options_sanity_checker(options);
    subcommands_sanity_checker(subcommands, {});

    // Split with '=' or ':' based on argument
    std::vector<meta_argument> meta_args_1 = {};
    std::size_t mod_i_1 = 0;
    for (; mod_i_1 < args.size(); mod_i_1++)
    {
        auto         &arg      = args[mod_i_1];
        argument_type arg_type = get_argument_type(arg);
        auto          pos      = std::string::npos;

        if (arg_type == argument_type::double_hyphen
         || arg_type == argument_type::unknown)
        {
            break;
        }

        if (arg_type == argument_type::long_option
         || arg_type == argument_type::short_option)
        {
            pos = arg.find_first_of('=');
        }

        if (arg_type == argument_type::microsoft_switch)
        {
            pos = arg.find_first_of(':');
        }

        if (pos == std::string::npos)
        {
            meta_args_1.emplace_back(
                squiggled_text(arg, 0, arg.size()),
                squiggled_text(arg, 0, arg.size()),
                arg_type
            );
            continue;
        }

        auto first  = arg.substr(0, pos);
        auto second = arg.substr(pos + 1);
        meta_args_1.emplace_back(
            squiggled_text(arg,   0, pos),
            squiggled_text(first, 0, first.size()),
            arg_type
        );
        meta_args_1.emplace_back(
            squiggled_text(arg,    pos + 1, second.size()),
            squiggled_text(second, 0,       second.size()),
            argument_type::regular_argument
        );
    }

    // Unparsed
    for (; mod_i_1 < args.size(); mod_i_1++)
    {
        auto &arg = args[mod_i_1];
        meta_args_1.emplace_back(
            squiggled_text(arg, 0, arg.size()),
            squiggled_text(arg, 0, arg.size()),
            argument_type::unknown
        );
    }

    // Split short options of "-abc" to "-a", "-b", "-c"
    std::vector<meta_argument> meta_args_2 = {};
    std::size_t mod_i_2 = 0;
    for (; mod_i_2 < meta_args_1.size(); mod_i_2++)
    {
        auto &meta_arg = meta_args_1[mod_i_2];

        if (meta_arg.arg_type == argument_type::double_hyphen
         || meta_arg.arg_type == argument_type::unknown)
        {
            break;
        }

        if (meta_arg.arg_type != argument_type::short_option)
        {
            meta_args_2.emplace_back(meta_arg);
            continue;
        }

        for (std::size_t i = 1; i < meta_arg.modified.text.size(); i++)
        {
            meta_args_2.emplace_back(
                squiggled_text(meta_arg.original.text,
                    meta_arg.original.position + i, 1),
                squiggled_text("-"s + meta_arg.modified.text[i], 1, 1),
                meta_arg.arg_type
            );
        }
    }

    // Unparsed
    for (; mod_i_2 < args.size(); mod_i_2++)
    {
        auto &arg = args[mod_i_2];
        meta_args_2.emplace_back(
            squiggled_text(arg, 0, arg.size()),
            squiggled_text(arg, 0, arg.size()),
            argument_type::unknown
        );
    }

    // Adjust pos and size for long names and Microsoft-style names
    std::vector<meta_argument> meta_args_3 = {};
    std::size_t mod_i_3 = 0;
    for (; mod_i_3 < meta_args_2.size(); mod_i_3++)
    {
        auto &meta_arg = meta_args_2[mod_i_3];

        if (meta_arg.arg_type == argument_type::double_hyphen
         || meta_arg.arg_type == argument_type::unknown)
        {
            break;
        }

        if (meta_arg.arg_type == argument_type::long_option)
        {
            meta_args_3.emplace_back(
                squiggled_text(meta_arg.original.text, 2,
                    meta_arg.modified.text.size() - 2),
                squiggled_text(meta_arg.modified.text, 2,
                    meta_arg.modified.text.size() - 2),
                meta_arg.arg_type
            );
        }
        else if (meta_arg.arg_type == argument_type::microsoft_switch)
        {
            meta_args_3.emplace_back(
                squiggled_text(meta_arg.original.text, 1,
                    meta_arg.modified.text.size() - 1),
                squiggled_text(meta_arg.modified.text, 1,
                    meta_arg.modified.text.size() - 1),
                meta_arg.arg_type
            );
        }
        else
        {
            meta_args_3.emplace_back(meta_arg);
        }
    }

    // Unparsed
    for (; mod_i_3 < args.size(); mod_i_3++)
    {
        auto &arg = args[mod_i_3];
        meta_args_3.emplace_back(
            squiggled_text(arg, 0, arg.size()),
            squiggled_text(arg, 0, arg.size()),
            argument_type::unknown
        );
    }

    // Latest modification
    auto meta_args = meta_args_3;

    // Nesting subcommands is a thing
    const subcommand_template *current_subcommand = nullptr;

    // Finally, we parse! (with the 90% of parsing code made being above...)
    std::vector<parsed_argument> result = {};
    std::size_t i = 0;
    for (; i < meta_args.size(); i++)
    {
        auto &meta_arg = meta_args[i];

        if (meta_arg.arg_type == argument_type::double_hyphen
         || meta_arg.arg_type == argument_type::unknown)
        {
            break;
        }
        // First check for subcommand
        else if (meta_arg.arg_type == argument_type::regular_argument)
        {
            const subcommand_template *matched_subcommand = nullptr;
            if (current_subcommand)
            {
                matched_subcommand = match_subcommand(meta_arg.modified.text,
                    current_subcommand->subcommands);
            }

            if (!matched_subcommand)
            {
                current_subcommand = nullptr;
                matched_subcommand = match_subcommand(meta_arg.modified.text,
                    subcommands);
            }

            if (!matched_subcommand)
            {
                parsed_argument parsed_arg = {
                    meta_arg, validity::unrecognized_subcommand, true, nullptr,
                    nullptr,  {}
                };

                result.emplace_back(parsed_arg);
                continue;
            }

            validity valid          = validity::valid;
            auto     collected_args = collect_values(i, meta_args,
                matched_subcommand->parameters,
                matched_subcommand->defaults_from_back, valid);

            parsed_argument parsed_arg = {
                meta_arg, valid, true, nullptr, matched_subcommand,
                collected_args
            };

            result.emplace_back(parsed_arg);
            current_subcommand = matched_subcommand;
        }
        else if (meta_arg.arg_type == argument_type::long_option
              || meta_arg.arg_type == argument_type::short_option
              || meta_arg.arg_type == argument_type::microsoft_switch)
        {
            // Then for options/switches within a subcommand if subcommand is
            // available
            const option_template *matched_option = nullptr;
            if (current_subcommand)
            {
                matched_option = match_option(meta_arg.modified.text,
                    meta_arg.arg_type, current_subcommand->subcommand_options,
                    switch_ins);
            }

            // Then for global options/switches
            if (!matched_option)
            {
                matched_option = match_option(meta_arg.modified.text,
                    meta_arg.arg_type, options, switch_ins);
            }

            if (!matched_option)
            {
                parsed_argument parsed_arg = {
                    meta_arg, validity::unrecognized_option, true, nullptr,
                    nullptr,  {}
                };

                result.emplace_back(parsed_arg);
                continue;
            }

            validity valid = validity::valid;

            auto collected_args = collect_values(i, meta_args,
                matched_option->parameters,
                matched_option->defaults_from_back, valid);

            parsed_argument parsed_arg = {
                meta_arg, valid, true, matched_option, nullptr, collected_args
            };

            result.emplace_back(parsed_arg);
        }
        else
        {
            parsed_argument parsed_arg = {
                meta_arg, validity::unknown, true, nullptr, nullptr, {}
            };

            result.emplace_back(parsed_arg);
        }
    }

    // Add all the unparsed arguments (as valid)
    for (; i < meta_args.size(); i++)
    {
        auto &meta_arg = meta_args[i];

        parsed_argument parsed_arg = {
            meta_arg, validity::valid, false, nullptr, nullptr, {}
        };

        result.emplace_back(parsed_arg);
    }
    return result;
}

/**
 *  @brief  Abstract helper to add name to the option_line.
 *
 *  @tparam  container          Container of compatible type.
 *  @tparam  get_name_t         Function type to get the name from element.
 *  @param   ctr                Container of elements to get name from.
 *  @param   separator          Separator.
 *  @param   wrap_pad           Padding for wrapped line.
 *  @param   wrap_width         Max width to wrap at.
 *  @param   current_line       Current option line (local variable).
 *  @param   option_lines       All the option lines (local variable).
 *  @param   get_name           Function to get the name from element.
 */
template<cu::cu_compatible container, typename get_name_t>
static inline constexpr auto add_names(
    const container                 &ctr,
    o::styled_text                   separator,
    o::styled_padding                wrap_pad,
    std::size_t                      wrap_width,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines,
    get_name_t                       get_name
)
{
    for (std::size_t i = 0; i < std::size(ctr); i++)
    {
        auto &element = *(std::begin(ctr) + i);

        o::measured_string name = {};

        // Separator
        if (i != 0)
        {
            name = separator.m_str();
        }

        name += get_name(i, element);

        // Wrap
        if (current_line.size + name.size > wrap_width)
        {
            option_lines.emplace_back(current_line);
            current_line = wrap_pad.m_str();
        }

        current_line += name;
    }
}

/**
 *  @brief  Helper to add long names to option_line.
 *
 *  @param  option             Option.
 *  @param  format             POSIX-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_long_names(
    const o::option_template        &option,
    const o::posix_help_format      &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(option.long_names, format.long_name_separator,
        format.pad_long_names, format.option_n_subcommand_width, current_line,
        option_lines, [&](std::size_t, const std::string &long_name)
    {
        return o::measured_string(format.long_name_prefix_style, "--")
             + o::measured_string(format.long_name_style, long_name);
    });
}

/**
 *  @brief  Helper to add short names to option_line.
 *
 *  @param  option             Option.
 *  @param  format             POSIX-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_short_names(
    const o::option_template        &option,
    const o::posix_help_format      &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(option.short_names, format.short_name_separator,
        format.pad_short_names, format.option_n_subcommand_width, current_line,
        option_lines, [&](std::size_t, char short_name)
    {
        return o::measured_string(format.short_name_prefix_style, "-")
             + o::measured_string(format.short_name_style,
            std::string(1, short_name));
    });
}

/**
 *  @brief  Helper to add Microsoft-style switch names (long and short option
 *          names) to option_line.
 *
 *  @param  option             Option.
 *  @param  format             Microsoft-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_microsoft_switches(
    const o::option_template        &option,
    const o::microsoft_help_format  &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    auto add_microsoft_switch_long_names = [&]()
    {
        add_names(option.long_names, format.switch_separator,
            format.pad_switch, format.switch_n_subcommand_width, current_line,
            option_lines, [&](std::size_t, const std::string &long_name)
        {
            return o::measured_string(format.switch_prefix_style, "/")
                 + o::measured_string(format.switch_style, long_name);
        });
    };

    auto add_microsoft_switch_short_names = [&]()
    {
        add_names(option.short_names, format.switch_separator,
            format.pad_switch, format.switch_n_subcommand_width, current_line,
            option_lines, [&](std::size_t, char short_name)
        {
            return o::measured_string(format.switch_prefix_style, "/")
                 + o::measured_string(format.switch_style,
                std::string(1, short_name));
        });
    };

    // Whichever first
    if (format.long_names_first)
    {
        add_microsoft_switch_long_names();
        current_line += format.switch_separator.m_str();
        add_microsoft_switch_short_names();
    }
    else
    {
        add_microsoft_switch_short_names();
        current_line += format.switch_separator.m_str();
        add_microsoft_switch_long_names();
    }
}

/**
 *  @brief  Helper to add subcommand names to option_line.
 *
 *  @param  subcommand         Subcommand.
 *  @param  format             POSIX-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_subcommand_names(
    const o::subcommand_template    &subcommand,
    const o::posix_help_format      &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(subcommand.names, format.subcommand_separator,
        format.pad_subcommand, format.option_n_subcommand_width, current_line,
        option_lines, [&](std::size_t, const std::string &name)
    {
        return o::measured_string(format.subcommand_style, name);
    });
}

/**
 *  @brief  Helper to add subcommand names to option_line.
 *
 *  @param  subcommand         Subcommand.
 *  @param  format             Microsoft-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_subcommand_names(
    const o::subcommand_template    &subcommand,
    const o::microsoft_help_format  &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(subcommand.names, format.subcommand_separator,
        format.pad_subcommand, format.switch_n_subcommand_width, current_line,
        option_lines, [&](std::size_t, const std::string &name)
    {
        return o::measured_string(format.subcommand_style, name);
    });
}

/**
 *  @brief  Abstract helper to get formatted parameter name.
 *
 *  @param  i                      Current parameter index.
 *  @param  parameter              Current parameter.
 *  @param  parameters_count       Number of parameters.
 *  @param  defaults_count         Number of default values.
 *  @param  optional_enclose       Encloser for optional parameter.
 *  @param  mandatory_enclose      Encloser for mandatory parameter.
 *  @param  prefix_first           Prefix for first parameter.
 *  @param  prefix                 Prefix for rest of the parameters.
 *  @param  enclose_before_prefix  Whether to enclose before appending prefix.
 *  @return  Formatted parameter name.
 */
[[nodiscard]] static inline constexpr auto parameter_name_adder(
    std::size_t         i,
    const std::string  &parameter,
    std::size_t         parameters_count,
    std::size_t         defaults_count,
    o::styled_enclosure optional_enclose,
    o::styled_enclosure mandatory_enclose,
    o::styled_text      prefix_first,
    o::styled_text      prefix,
    bool                enclose_before_prefix
)
{
    bool is_optional = parameters_count - defaults_count < i + 1;

    o::styled_enclosure encloser = {};
    if (is_optional) encloser = optional_enclose;
    else encloser = mandatory_enclose;

    o::styled_text actual_prefix = {};
    if (i == 0) actual_prefix = prefix_first;
    else actual_prefix = prefix;

    if (enclose_before_prefix)
    {
        return o::measured_string(prefix.style, prefix.value)
             + o::measured_string(encloser.prefix.style, encloser.prefix.value)
             + o::measured_string(encloser.value_style, parameter)
             + o::measured_string(encloser.suffix.style,
            encloser.suffix.value);
    }
    else
    {
        return o::measured_string(encloser.prefix.style, encloser.prefix.value)
             + o::measured_string(prefix.style, prefix.value)
             + o::measured_string(encloser.value_style, parameter)
             + o::measured_string(encloser.suffix.style,
            encloser.suffix.value);
    }
}

/**
 *  @brief  Helper to add parameter names to option_line.
 *
 *  @param  option             Option.
 *  @param  format             POSIX-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_parameter_names(
    const o::option_template        &option,
    const o::posix_help_format      &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(option.parameters, format.parameter_separator,
        format.pad_parameters_wrapped, format.option_n_subcommand_width,
        current_line, option_lines,
        [&](std::size_t i, const std::string &parameter)
    {
        return parameter_name_adder(i, parameter, option.parameters.size(),
            option.defaults_from_back.size(), format.optional_parameter_enclose,
            format.mandatory_parameter_enclose, format.parameter_prefix_first,
            format.parameter_prefix, format.enclose_before_prefix);
    });
}

/**
 *  @brief  Helper to add parameter names to option_line.
 *
 *  @param  option             Option.
 *  @param  format             Microsoft-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_parameter_names(
    const o::option_template        &option,
    const o::microsoft_help_format  &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(option.parameters, format.parameter_separator,
        format.pad_parameters_wrapped, format.switch_n_subcommand_width,
        current_line, option_lines,
        [&](std::size_t i, const std::string &parameter)
    {
        return parameter_name_adder(i, parameter, option.parameters.size(),
            option.defaults_from_back.size(), format.optional_parameter_enclose,
            format.mandatory_parameter_enclose, format.parameter_prefix_first,
            format.parameter_prefix, format.enclose_before_prefix);
    });
}

/**
 *  @brief  Helper to add parameter names to option_line.
 *
 *  @param  subcommand         Subcommand.
 *  @param  format             POSIX-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_parameter_names(
    const o::subcommand_template    &subcommand,
    const o::posix_help_format      &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(subcommand.parameters, format.parameter_separator,
        format.pad_parameters_wrapped, format.option_n_subcommand_width,
        current_line, option_lines,
        [&](std::size_t i, const std::string &parameter)
    {
        return parameter_name_adder(i, parameter, subcommand.parameters.size(),
            subcommand.defaults_from_back.size(),
            format.optional_parameter_enclose,
            format.mandatory_parameter_enclose, format.parameter_prefix_first,
            format.parameter_prefix, format.enclose_before_prefix);
    });
}

/**
 *  @brief  Helper to add parameter names to option_line.
 *
 *  @param  subcommand         Subcommand.
 *  @param  format             Microsoft-style format.
 *  @param  current_line       Current option line (local variable).
 *  @param  option_lines       All the option lines (local variable).
 */
static inline constexpr auto add_parameter_names(
    const o::subcommand_template    &subcommand,
    const o::microsoft_help_format  &format,
    o::measured_string              &current_line,
    std::vector<o::measured_string> &option_lines
)
{
    add_names(subcommand.parameters, format.parameter_separator,
        format.pad_parameters_wrapped, format.switch_n_subcommand_width,
        current_line, option_lines,
        [&](std::size_t i, const std::string &parameter)
    {
        return parameter_name_adder(i, parameter, subcommand.parameters.size(),
            subcommand.defaults_from_back.size(),
            format.optional_parameter_enclose,
            format.mandatory_parameter_enclose, format.parameter_prefix_first,
            format.parameter_prefix, format.enclose_before_prefix);
    });
}

/**
 *  @brief  Helper to combine option_line and description text.
 *
 *  @param  desc              Description text.
 *  @param  desc_wrap_width   Width to wrap description at.
 *  @param  ons_width         All option/switch and subcommand's total width.
 *  @param  pad_desc          Padding for first line of description.
 *  @param  pad_desc_wrapped  Padding for wrapped lines of description.
 *  @param  option_lines      All the option lines (local variable).
 *  @return  @c std::vector<std::string> of lines after combining.
 */
static inline constexpr auto combine_option_description(
    std::string                      desc,
    std::size_t                      desc_wrap_width,
    std::size_t                      ons_width,
    o::styled_padding                pad_desc,
    o::styled_padding                pad_desc_wrapped,
    std::vector<o::measured_string> &option_lines
)
{
    std::vector<std::string> result = {};
    auto wrapped_desc = sm::word_wrap(desc, desc_wrap_width);

    bool offset_by_one = false;
    if (ons_width > pad_desc.width && !option_lines.empty())
    {
        offset_by_one = option_lines.size() - option_lines.back().size
                     <= pad_desc.width;
    }

    // Add description and option lines
    // Will continue to add option even if description lines are ended
    std::size_t i = 0;
    for (; i < option_lines.size(); i++)
    {
        std::string line = option_lines[i].string;

        std::size_t j = i - offset_by_one;
        if (j == 0)
        {
            line += pad_desc.str(line.size());
            line += wrapped_desc.front();
        }
        else if (j < wrapped_desc.size())
        {
            line += pad_desc_wrapped.str(line.size());
            line += wrapped_desc[j];
        }
        result.emplace_back(line);
    }

    // Add description if option lines ended before description lines
    for (std::size_t j = i - offset_by_one; j < wrapped_desc.size(); i++, j++)
    {
        std::string line = "";
        line += pad_desc_wrapped.str(line.size());
        line += wrapped_desc[j];
        result.emplace_back(line);
    }

    return result;
}

/**
 *  @brief  Generate help message string from option and POSIX-style format.
 *
 *  @param  option  Option.
 *  @param  format  POSIX-style format.
 *  @return  @c std::vector<std::string> for each line.
 */
[[nodiscard]] auto o::get_help_message(
    const option_template &option,
    posix_help_format      format
) -> std::vector<std::string>
{
    std::vector<std::string>        result       = {};
    std::vector<o::measured_string> option_lines = {};
    o::measured_string current_line = {};

    // Whichever first
    if (format.long_names_first)
    {
        add_long_names(option, format, current_line, option_lines);

        current_line += o::measured_string(
            format.short_n_long_name_separator.style,
            format.short_n_long_name_separator.value);

        add_short_names(option, format, current_line, option_lines);
    }
    else
    {
        add_short_names(option, format, current_line, option_lines);

        current_line += o::measured_string(
            format.short_n_long_name_separator.style,
            format.short_n_long_name_separator.value);

        add_long_names(option, format, current_line, option_lines);
    }

    add_parameter_names(option, format, current_line, option_lines);

    result = combine_option_description(option.description,
        format.description_width, format.option_n_subcommand_width,
        format.pad_description, format.pad_description_wrapped, option_lines);

    return result;
}

/**
 *  @brief  Generate help message string from subcommand and POSIX-style format.
 *
 *  @param  subcommand  Subcommand.
 *  @param  format      POSIX-style format.
 *  @return  @c std::vector<std::string> for each line.
 */
[[nodiscard]] auto o::get_help_message(
    const subcommand_template &subcommand,
    posix_help_format          format
) -> std::vector<std::string>
{
    std::vector<std::string>        result       = {};
    std::vector<o::measured_string> option_lines = {};
    o::measured_string current_line = {};

    add_subcommand_names(subcommand, format, current_line, option_lines);

    add_parameter_names(subcommand, format, current_line, option_lines);

    result = combine_option_description(subcommand.description,
        format.description_width, format.option_n_subcommand_width,
        format.pad_description, format.pad_description_wrapped, option_lines);

    return result;
}

/**
 *  @brief  Generate help message string from option (switch) and
 *          Microsoft-style format.
 *
 *  @param  option  Option.
 *  @param  format  Microsoft-style format.
 *  @return  @c std::vector<std::string> for each line.
 */
[[nodiscard]] auto o::get_help_message(
    const option_template &option,
    microsoft_help_format  format
) -> std::vector<std::string>
{
    std::vector<std::string>        result       = {};
    std::vector<o::measured_string> option_lines = {};
    o::measured_string current_line = {};

    add_microsoft_switches(option, format, current_line, option_lines);

    add_parameter_names(option, format, current_line, option_lines);

    result = combine_option_description(option.description,
        format.description_width, format.switch_n_subcommand_width,
        format.pad_description, format.pad_description_wrapped, option_lines);

    return result;
}

/**
 *  @brief  Generate help message string from subcommand and Microsoft-style
 *          format.
 *
 *  @param  subcommand  Subcommand.
 *  @param  format      Microsoft-style format.
 *  @return  @c std::vector<std::string> for each line.
 */
[[nodiscard]] auto o::get_help_message(
    const subcommand_template &subcommand,
    microsoft_help_format      format
) -> std::vector<std::string>
{
    std::vector<std::string>        result       = {};
    std::vector<o::measured_string> option_lines = {};
    o::measured_string current_line = {};

    add_subcommand_names(subcommand, format, current_line, option_lines);

    add_parameter_names(subcommand, format, current_line, option_lines);

    result = combine_option_description(subcommand.description,
        format.description_width, format.switch_n_subcommand_width,
        format.pad_description, format.pad_description_wrapped, option_lines);

    return result;
}
