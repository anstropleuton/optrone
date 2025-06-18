/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source provides implementation for the main argument parser.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "optrone/error.hpp"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

// Sanity proofing
using option_ptr     = std::shared_ptr<optrone::option_template>;
using option_vec     = std::vector<option_ptr>;
using subcommand_ptr = std::shared_ptr<optrone::subcommand_template>;
using subcommand_vec = std::vector<subcommand_ptr>;

/// Determine the token type from the argument.
static optrone::token::token_type determine_type(std::string_view value)
{
    if (value.starts_with("/")) return optrone::token::token_type::switch_option;
    if (value.starts_with("--")) return optrone::token::token_type::long_option;
    if (value.starts_with("-")) return optrone::token::token_type::short_option;
    return optrone::token::token_type::regular;
}

std::vector<optrone::token> optrone::tokenize(const std::vector<std::string> &args)
{
    std::vector<token> tokens;
    tokens.resize(args.size());
    for (std::size_t i = 0; i < args.size(); i++)
    {
        tokens[i] = { args[i], determine_type(args[i]) };
    }

    // 1. Split at `=` or `:` based on whether it is an option or a switch.
    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        token      &tok = tokens[i];
        std::size_t pos = (std::size_t) -1;
        if (tok.type == token::token_type::long_option ||
            tok.type == token::token_type::short_option)
        {
            pos = tok.value.find('=');
        }
        else if (tok.type == token::token_type::switch_option)
        {
            pos = tok.value.find(':');
        }

        if (pos != (std::size_t) -1)
        {
            token first  = { tok.value.substr(0, pos), tok.type };
            token second = { tok.value.substr(pos + 1), token::token_type::regular }; // Treat as regular arg
            tok          = first;
            tokens.emplace(tokens.begin() + i + 1, second);
            i++; // Skip processing second, as we know it is fixed to be regular
        }
    }

    // 2. Split `-abc` as three tokens: `-a`, `-b` and `-c`.
    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        token &tok = tokens[i];
        if (tok.type == token::token_type::short_option && tok.value.size() > 2)
        {
            std::string characters = tok.value.substr(2);
            tok.value              = std::string("-") + tok.value[1];

            for (std::size_t j = 0; j < characters.size(); j++)
            {
                tokens.emplace(tokens.begin() + i + j + 1, std::string("-") + characters[j], token::token_type::short_option);
            }

            i += characters.size(); // Skip processing the already split args.
        }
    }

    // Adjust text range of each token
    std::size_t prev = 0;
    for (token &tok : tokens)
    {
        tok.range.begin   = prev;
        tok.range.pointer = tok.range.begin;
        tok.range.length  = tok.value.size();
        prev += tok.range.length + 1;
    }

    return tokens;
}

std::string optrone::construct_command_line(const std::vector<token> &tokens)
{
    std::string command_line = "";
    for (const token &tok : tokens)
    {
        command_line += " " + tok.value;
    }

    if (!command_line.empty())
    {
        command_line = command_line.substr(1);
    }

    return command_line;
}

static std::string str_to_lower(std::string_view str)
{
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

/// Validates an option, throws if invalid.
static void validate_option(option_ptr option)
{
    if (option->long_names.empty() && option->short_names.empty())
    {
        throw std::invalid_argument("No short names or long names specified");
    }

    for (const std::string &long_name : option->long_names)
    {
        if (long_name.size() < 2)
        {
            throw std::invalid_argument("Long name cannot be less than 2 characters");
        }
        if (long_name != str_to_lower(long_name))
        {
            throw std::invalid_argument("Long name must be lowercase");
        }
        if (long_name.contains(':') || long_name.contains('='))
        {
            throw std::invalid_argument("Long name cannot contain '=' or ':'");
        }
        if (long_name.starts_with('-') || long_name.starts_with('/'))
        {
            throw std::invalid_argument("Long name cannot start with '-' or '/'");
        }
    }

    for (char short_name : option->short_names)
    {
        if (std::isupper(short_name))
        {
            throw std::invalid_argument("Short name must be lowercase");
        }
        if (short_name == '-' || short_name == '/' || short_name == '=' || short_name == ':')
        {
            throw std::invalid_argument("Short name cannot be '-', '/', '=' or ':'");
        }
    }

    if (option->defaults.size() > option->params.size())
    {
        throw std::invalid_argument("Option cannot have more number of default values than declared parameters");
    }

    if (!option->defaults.empty() && option->variadic)
    {
        throw std::invalid_argument("Option cannot have default values and variadic parameters");
    }
}

/// Validates subcommand, nested options and nested subcommands, throws if invalid.
static void validate_subcommand(subcommand_ptr subcommand)
{
    if (subcommand->names.empty())
    {
        throw std::invalid_argument("No short names or long names specified");
    }

    for (const std::string &name : subcommand->names)
    {
        if (name != str_to_lower(name))
        {
            throw std::invalid_argument("Subcommand name must be lowercase");
        }
        if (name.contains(':') || name.contains('='))
        {
            throw std::invalid_argument("Subcommand name cannot contain '=' or ':'");
        }
        if (name.starts_with('-') || name.starts_with('/'))
        {
            throw std::invalid_argument("Subcommand name cannot start with '-' or '/'");
        }
    }

    if (subcommand->defaults.size() > subcommand->params.size())
    {
        throw std::invalid_argument("Subcommand cannot have more number of default values than declared parameters");
    }

    if (!subcommand->defaults.empty() && subcommand->variadic)
    {
        throw std::invalid_argument("Subcommand cannot have default values and variadic parameters");
    }

    if (!subcommand->nested_subcommands.empty() && subcommand->variadic)
    {
        throw std::invalid_argument("Subcommand cannot have nested subcommands and variadic parameters");
    }

    if (!subcommand->defaults.empty() && !subcommand->nested_subcommands.empty())
    {
        throw std::invalid_argument("Subcommand cannot have default values and nested subcommands");
    }

    for (option_ptr option : subcommand->nested_options)
    {
        validate_option(option);
    }

    for (subcommand_ptr subcommand : subcommand->nested_subcommands)
    {
        validate_subcommand(subcommand);
    }
}

void optrone::validate_templates(
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands)
{
    for (option_ptr option : options)
    {
        validate_option(option);
    }

    for (subcommand_ptr subcommand : subcommands)
    {
        validate_subcommand(subcommand);
    }
}

/// Find long name from the templates.
static option_ptr find_long_name(
    std::string_view long_name,
    option_vec       options)
{
    std::string lower_name = str_to_lower(long_name);
    for (option_ptr option : options)
    {
        for (const std::string &option_long_name : option->long_names)
        {
            if (lower_name == option_long_name)
            {
                return option;
            }
        }
    }

    return nullptr;
}

/// Find short name from the templates.
static option_ptr find_short_name(
    char       short_name,
    option_vec options)
{
    char lower_name = std::tolower(short_name);
    for (option_ptr option : options)
    {
        for (char option_short_name : option->short_names)
        {
            if (lower_name == option_short_name)
            {
                return option;
            }
        }
    }

    return nullptr;
}

/// Find long or short name from the templates.
static option_ptr find_option(
    std::string_view           name,
    optrone::token::token_type type,
    option_vec                 options)
{
    switch (type)
    {
        case optrone::token::token_type::long_option:
            return find_long_name(name.substr(2), options);
        case optrone::token::token_type::short_option:
            return find_short_name(name[1], options);
        case optrone::token::token_type::switch_option:
            if (name.size() == 2)
                return find_short_name(name[1], options);
            else
                return find_long_name(name.substr(1), options);
        default:
            break;
    }

    return nullptr;
}

/// Find subcommand name from the templates.
static subcommand_ptr find_subcommand_name(
    std::string_view name,
    subcommand_vec   subcommands)
{
    std::string lower_name = str_to_lower(name);
    for (subcommand_ptr subcommand : subcommands)
    {
        for (const std::string &subcommand_name : subcommand->names)
        {
            if (lower_name == subcommand_name)
            {
                return subcommand;
            }
        }

        auto result = find_subcommand_name(name, subcommand->nested_subcommands);

        if (result)
        {
            return result;
        }
    }

    return nullptr;
}

/// Collect values for parameters.
static std::vector<std::string> collect_values(
    std::size_t                       &index,
    const std::vector<optrone::token> &tokens,
    const std::vector<std::string>    &params,
    const std::vector<std::string>    &defaults,
    bool                               variadic)
{
    std::vector<std::string> values;

    std::size_t count = 0;
    for (; count < params.size() && index < tokens.size(); index++, count++)
    {
        if (tokens[index].type != optrone::token::token_type::regular)
        {
            break;
        }

        values.emplace_back(tokens[index].value);
    }

    // WTF? (add default values, since they are right-anchored we get this dirty arithmetics)
    std::size_t first = count - params.size() + defaults.size();
    std::size_t last  = defaults.size();
    if (first < last)
    {
        auto to_add = std::vector(defaults.begin() + first, defaults.begin() + last);
        for (const std::string &add : to_add)
        {
            values.emplace_back(add);
        }
    }

    // Variadic arguments, add until regular tokens
    if (variadic)
    {
        for (; index < tokens.size(); index++)
        {
            if (tokens[index].type != optrone::token::token_type::regular)
            {
                break;
            }

            values.emplace_back(tokens[index].value);
        }
    }

    return values;
}

std::vector<optrone::parsed_argument> optrone::parse_arguments(
    const std::vector<std::string>                   &args,
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands)
{
    validate_templates(options, subcommands);

    std::vector<token> tokens   = tokenize(args);
    std::string        cmd_line = construct_command_line(tokens);

    std::shared_ptr<subcommand_template> nested = nullptr; // Currently nested subcommand to match for, or match global if not found

    // Parse all tokens
    std::vector<parsed_argument> result;
    for (std::size_t index = 0; index < tokens.size();)
    {
        token tok = tokens[index];

        if (tok.type == token::token_type::regular)
        {
            std::shared_ptr<subcommand_template> matched = nullptr;

            if (nested)
            {
                matched = find_subcommand_name(tok.value, { nested });
            }

            if (!matched)
            {
                nested  = nullptr;
                matched = find_subcommand_name(tok.value, subcommands);
            }

            if (!matched)
            {
                throw argument_error("Unrecognized subcommand", cmd_line, tok.range);
            }

            index++;
            auto values = collect_values(index, tokens, matched->params, matched->defaults, matched->variadic);
            if (values.size() < matched->params.size())
            {
                throw argument_error("Too vew values provided for parameters", cmd_line, tok.range);
            }

            result.push_back({ {}, matched, values });
            nested = matched; // Find for nested subcommands
        }
        else if (tok.type == token::token_type::long_option ||
                 tok.type == token::token_type::short_option ||
                 tok.type == token::token_type::switch_option)
        {
            std::shared_ptr<option_template> matched = nullptr;

            if (nested)
            {
                matched = find_option(tok.value, tok.type, nested->nested_options);
            }

            if (!matched)
            {
                matched = find_option(tok.value, tok.type, options);
            }

            if (!matched)
            {
                throw argument_error("Unrecognized option", cmd_line, tok.range);
            }

            index++;
            auto values = collect_values(index, tokens, matched->params, matched->defaults, matched->variadic);
            if (values.size() < matched->params.size())
            {
                throw argument_error("Too vew values provided for parameters", cmd_line, tok.range);
            }

            result.push_back({ matched, {}, values });
        }
        else
            index++;
    }

    return result;
}
