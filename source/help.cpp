/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source provides implementation for help message display.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "optrone/error.hpp"
#include "optrone/help.hpp"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

// Sanity proofing
using option_ptr     = std::shared_ptr<optrone::option_template>;
using option_vec     = std::vector<option_ptr>;
using subcommand_ptr = std::shared_ptr<optrone::subcommand_template>;
using subcommand_vec = std::vector<subcommand_ptr>;

/// Convert string to uppercase.
static std::string str_to_upper(std::string_view str)
{
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), toupper);
    return result;
}

/// Word-wrap a string.
static std::vector<std::string> wrap_string(std::string_view string, std::size_t width)
{
    static const std::string newlines = "\n\r\v";
    static const std::string delims   = " \t\r\n\f\v\b";
    std::vector<std::string> lines;

    while (string.size() > width)
    {
        std::string_view substr = string.substr(0, width);
        std::size_t      pos    = substr.find_first_of(newlines);

        if (pos == (std::size_t) -1)
        {
            pos = substr.find_last_of(delims);
        }

        bool delim_found = true;
        if (pos == (std::size_t) -1)
        {
            pos         = width;
            delim_found = false;
        }

        lines.emplace_back(std::string(string.substr(0, pos)));

        // Exclude newline if delimiter was found
        string = string.substr(std::min(pos + delim_found, string.size()));
    }

    if (!string.empty())
    {
        lines.emplace_back(std::string(string));
    }

    return lines;
}

/// Get a string with `n` space characters.
static std::string spaces(std::size_t n)
{
    return std::string(n, ' ');
}

/// Style a string by add style before the value and appending a style reset saec.
static std::string style(std::string_view value, std::string_view saec)
{
    if (saec.empty())
    {
        return std::string(value);
    }

    return std::string(saec) + std::string(value) + "$0";
}

/// Style a string by add style before the value and appending a style reset saec.
static std::string style(char value, std::string_view saec)
{
    return style(std::string(1, value), saec);
}

/// Build a list of long names in the format
/// `-a, -b, ...`
/// or
/// `/A, /B, ...` (if Microsoft style).
/// @note It does not include indentation or styling.
static std::string build_short_names(option_ptr option, optrone::help_customizer customizer)
{
    std::ostringstream oss;

    bool first = true;
    for (char short_name : option->short_names)
    {
        if (first)
            first = false;
        else
            oss << ", ";

        if (customizer.microsoft_style)
            oss << "/" << style(std::toupper(short_name), customizer.template_saec);
        else
            oss << "-" << style(short_name, customizer.template_saec);
    }

    return oss.str();
}

/// Build a list of long names in the format
/// `--name-1, --name-2, ...`
/// or
/// `/NAME-1, /NAME-2, ...` (if Microsoft style).
/// @note It does not include indentation or styling.
static std::string build_long_names(option_ptr option, optrone::help_customizer customizer)
{
    std::ostringstream oss;

    bool first = true;
    for (const std::string &long_name : option->long_names)
    {
        if (first)
            first = false;
        else
            oss << ", ";

        if (customizer.microsoft_style)
            oss << "/" << style(str_to_upper(long_name), customizer.template_saec);
        else
            oss << "--" << style(long_name, customizer.template_saec);
    }

    return oss.str();
}

/// Build a list of subcommand names in the format
/// `subc-1, subc-2, ...`
/// @note It does not include indentation or styling.
static std::string build_subcommand_names(subcommand_ptr subcommand, optrone::help_customizer customizer)
{
    std::ostringstream oss;

    bool first = true;
    for (const std::string &name : subcommand->names)
    {
        if (first)
            first = false;
        else
            oss << ", ";

        oss << style(name, customizer.template_saec);
    }

    return oss.str();
}

/// Build a list of parameters in the format
/// `<param-1> <param-2> ... [default-1] [default-2]`
/// or
/// `<PARAM-1> <PARAM-2> ... [DEFAULT-1] [DEFAULT-2]` (if Microsoft style).
static std::string build_params(const std::vector<std::string> &params, const std::vector<std::string> &defaults, bool variadic, optrone::help_customizer customizer)
{
    std::ostringstream oss;

    bool first = true;
    for (std::size_t i = 0; i < params.size(); i++)
    {
        if (first)
            first = false;
        else
            oss << " ";

        // Assuming validation was performed
        std::size_t default_index = i - (params.size() - defaults.size());
        bool        is_optional   = default_index < defaults.size();

        if (is_optional)
            oss << "[";
        else
            oss << "<";

        oss << style(params[i], customizer.template_saec);

        if (is_optional && !defaults[default_index].empty())
        {
            oss << "=" << style(defaults[default_index], customizer.template_saec);
        }

        if (is_optional)
            oss << "]";
        else
            oss << ">";
    }

    if (variadic)
    {
        if (!params.empty())
            oss << " ";
        oss << "...";
    }

    return oss.str();
}

/// Builds all the lines for description by taking the template line's size for first line indentation.
/// @note Also guarantees a newline.
static std::string build_description(std::size_t template_line_length, std::string_view description, optrone::help_customizer customizer)
{
    std::ostringstream oss;

    auto wrapped_description = wrap_string(description, customizer.description_width);
    if (!wrapped_description.empty())
    {
        if (template_line_length <= customizer.description_indent)
        {
            oss << spaces(customizer.description_indent - template_line_length) << style(wrapped_description[0], customizer.description_saec) << "\n";
            wrapped_description.erase(wrapped_description.begin());
        }
        else
        {
            oss << "\n";
        }

        for (const std::string &line : wrapped_description)
        {
            oss << spaces(customizer.description_indent) << style(line, customizer.description_saec) << "\n";
        }
    }
    else
    {
        oss << "\n";
    }

    return oss.str();
}

/// Get help message of an option.
static std::string option_help_message(option_ptr option, optrone::help_customizer customizer)
{
    std::string short_names = build_short_names(option, customizer);
    std::string long_names  = build_long_names(option, customizer);
    std::string params      = build_params(option->params, option->defaults, option->variadic, customizer);

    std::ostringstream oss;

    // Templates
    if (!short_names.empty())
        oss << spaces(customizer.short_names_indent) << short_names;

    if (!long_names.empty())
    {
        if (short_names.empty())
            oss << spaces(customizer.long_names_indent);
        else
            oss << ", ";
        oss << long_names;
    }

    if (!params.empty())
        oss << " " << params;

    std::string template_line = oss.str();
    std::size_t template_size = optrone::format_saec(template_line, true).size(); // Remove SAEC and then calculate size

    return template_line + build_description(template_size, option->description, customizer);
}

/// Get help message of a subcommand.
/// @note Does not include nested options or subcommands
static std::string subcommand_help_message(subcommand_ptr subcommand, optrone::help_customizer customizer)
{
    std::string names  = build_subcommand_names(subcommand, customizer);
    std::string params = build_params(subcommand->params, subcommand->defaults, subcommand->variadic, customizer);

    std::ostringstream oss;

    oss << spaces(customizer.subcommand_indent) << names;

    if (!params.empty())
    {
        oss << " " << params;
    }

    std::string template_line = oss.str();
    std::size_t template_size = optrone::format_saec(template_line, true).size(); // Remove SAEC and then calculate size

    return template_line + build_description(template_size, subcommand->description, customizer);
}

/// Get help message of nested options and subcommands.
/// @note Does not include help message of the subcommand itself.
/// @param names_list List of subcommands names that lead to this nesting.
static std::string subcommand_help_message_nested(subcommand_ptr subcommand, optrone::help_customizer customizer, std::string names_list)
{
    std::string result;

    if (!subcommand->nested_options.empty() || !subcommand->nested_subcommands.empty())
    {
        // Add names that lead up to this nested subcommand.
        if (!names_list.empty())
        {
            names_list += " ";
        }
        names_list += subcommand->names[0];
        result += "\n" + names_list + ":\n";

        for (option_ptr option : subcommand->nested_options)
        {
            result += option_help_message(option, customizer);
        }

        if (!subcommand->nested_options.empty() && !subcommand->nested_subcommands.empty())
        {
            result += "\n";
        }

        for (subcommand_ptr subcommand : subcommand->nested_subcommands)
        {
            result += subcommand_help_message(subcommand, customizer);
        }

        for (subcommand_ptr subcommand : subcommand->nested_subcommands)
        {
            result += subcommand_help_message_nested(subcommand, customizer, names_list);
        }
    }

    return result;
}

std::string optrone::get_help_message(
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands,
    help_customizer                                   customizer)
{
    validate_templates(options, subcommands);

    std::string result;

    for (option_ptr option : options)
    {
        result += option_help_message(option, customizer);
    }

    if (!options.empty() && !subcommands.empty())
    {
        result += "\n";
    }

    for (subcommand_ptr subcommand : subcommands)
    {
        result += subcommand_help_message(subcommand, customizer);
    }

    for (subcommand_ptr subcommand : subcommands)
    {
        result += subcommand_help_message_nested(subcommand, customizer, "");
    }

    return result;
}
