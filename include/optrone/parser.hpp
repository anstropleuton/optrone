/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides definitions for parser-related classes such as
/// parsed argument result type and parse function.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "optrone/error.hpp"
#include "optrone/template.hpp"

namespace optrone {

/// A single argument from the command-line.
struct token {
    /// The type of a token.
    enum class token_type {
        regular,      ///< Regular argument.
        short_option, ///< Argument starts with `-`.
        long_option,  ///< Argument starts with `--`.
        switch_option ///< Argument starts with `/`.
    };

    std::string value;                      ///< Token value.
    token_type  type = token_type::regular; ///< Type of the token.
    text_range  range;                      ///< Range within the command line.
};

/// Arguments parsed from command-line along with any parameters or default
/// values.
struct parsed_argument {
    std::weak_ptr<option_template>     ref_option;     ///< Option associated with this argument.
    std::weak_ptr<subcommand_template> ref_subcommand; ///< Subcommand associated with this argument.
    std::vector<std::string>           values;         ///< Values for parameters (including defaults).
};

/// Tokenize the arguments.
std::vector<token> tokenize(const std::vector<std::string> &args);

/// Reconstruct the command-line from tokens.
std::string construct_command_line(const std::vector<token> &tokens);

/// Throws if templates are invalid.
/// @see parse_arguments for list of exceptions.
void validate_templates(
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands);

/// Parse all the provided command-line arguments.
///
/// @exception std::invalid_argument Thrown in the following cases:
/// - No names are specified in a template.
/// - Names are not lowercase.
/// - Names contains `=` or `:`.
/// - Names starts with `-` or `/`.
/// - Long name is less than 2 characters.
/// - Number of default values exceed number of declared parameters.
/// - Mutually exclusive features are used together.
/// @exception argument_error Thrown in the following cases:
/// - Command-line argument points to option or subcommand that does not exist.
/// - Too few values provided for parameters.
std::vector<parsed_argument> parse_arguments(
    const std::vector<std::string>                   &args,
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands);

} // namespace optrone
