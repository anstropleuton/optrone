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
#include <string_view>
#include <vector>

#include "optrone/template.hpp"

namespace optrone {

/// Customize the help message display of a template.
struct help_customizer {
    int short_names_indent = 2;  ///< Indentation for short names.
    int long_names_indent  = 6;  ///< Indentation for long names.
    int subcommand_indent  = 4;  ///< Indentation for subcommands.
    int description_indent = 40; ///< Indentation for description.
    int description_width  = 40; ///< Maximum width of description before word-wrapping.

    std::string_view template_saec;    ///< Style the templates (options and subcommands).
    std::string_view description_saec; ///< Style the description text.

    bool microsoft_style = false; ///< If true, displays options using /O or /OPTION style.
};

/// Obtain multi-line help message from the list of subcommands.
///
/// The help message is created in the below format with the default
/// customization:
///
/// An option:
/// ```
///   -a, --option <param> [param=default]  Description of the option.
/// ```
///
/// An obnoxious option:
/// ```
///   -a, -b, -c, -d, -e, -f, -g, -h, -i, -j, -k, --option-1, --option-2 <param-1> <param-2> [param-3=default] [param-4=default]
///                                         Lorem ipsum dolor sit amet, consectetur
///                                         adipiscing elit, sed do eiusmod tempor
///                                         incididunt ut labore et dolore magna
///                                         aliqua. Ut enim ad minim veniam, quis
///                                         nostrud exercitation ullamco laboris
///                                         nisi ut aliquip ex ea commodo consequat.
/// ```
///
/// ```
///   /A, /OPTION <PARAM> [PARAM=DEFAULT]   Ut enim ad minim veniam, quis.
/// ```
///
/// A subcommand:
/// ```
///     subc-1, subc-2 <param> [param=default]
///                                         Nostrud exercitation ullamco.
/// ```
///
/// A subcommand with nested options and subcommands:
/// ```
///     subc-1                              Laboris nisi ut aliquip ex ea.
///
/// subc-1:
///   -a, --option-1                        Commodo consequat. Duis aute.
///   -b, --option-2                        Irure dolor in reprehenderit.
///     subc-2                              In voluptate velit esse cillum.
///     subc-3                              Dolore eu fugiat nulla pariatur.
///
/// subc-2:
///   -c, --option-3                        Excepteur sint occaecat.
///   -d, --option-4                        Cupidatat non proident, sunt in.
///
/// subc-3:
///   -e, --option-5                        Culpa qui officia deserunt.
///   -f, --option-6                        Mollit anim id est laborum.
/// ```
std::string get_help_message(
    std::vector<std::shared_ptr<option_template>>     options,
    std::vector<std::shared_ptr<subcommand_template>> subcommands,
    help_customizer                                   customizer = help_customizer());

} // namespace optrone
