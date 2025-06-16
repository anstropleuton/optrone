/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file contains template structure for defining a valid
/// command-line options and subcommands that the parser will expect as
/// arguments.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace optrone {

/// A template for defining a command-line option.
/// @note Certain features are "Mutually exclusive", meaning they cannot be used
/// together. Those are:
/// - Default values for parameters.
/// - Variadic parameters.
struct option_template {
    /// Description of the option for help message display.
    std::string description;

    /// Short names that refers to this option, such as `'v'` for `-v` or `/V`.
    /// @note Must be lowercase.
    std::vector<char> short_names;

    /// Long names that refers to this option, such as `"version"` for
    /// `--version` or `/VERSION`.
    /// @note Must be lowercase, and greater-than or equal to 2 characters.
    std::vector<std::string> long_names;

    /// Parameter names that this option takes as additional values, such as
    /// `"level"` for `-v=<level>` or `/V:<level>`.
    std::vector<std::string> params;

    /// Default values (RIGHT-ANCHORED) for the parameters if they are not specified.
    /// @note Right-anchored: the default values correspond to the last N parameters in order.
    /// @note This is a mutually-exclusive feature.
    std::vector<std::string> defaults;

    /// If true, the option can take any number of parameters after the last
    /// parameter.
    /// @note This is a mutually-exclusive feature.
    bool variadic = false;
};

/// A template for defining a command-line subcommand (the "positional argument").
/// @note Certain features are "Mutually exclusive", meaning they cannot be used
/// together. Those are:
/// - Default values for parameters.
/// - Nested subcommands.
/// - Variadic parameters.
struct subcommand_template {
    /// Description of the subcommand for help message display.
    std::string description;

    /// Names that refer to this subcommand, such as `"get"` for `program get`.
    /// @note Must be lowercase.
    std::vector<std::string> names;

    /// Parameter names that this subcommand takes as additional values. such as
    /// `"rate"` for `program set <rate>`.
    std::vector<std::string> params;

    /// Default values (RIGHT-ANCHORED) for the parameters if they are not specified.
    /// @note Right-anchored: the default values correspond to the last N parameters in order.
    /// @note This is a mutually-exclusive feature.
    std::vector<std::string> defaults;

    /// If true, the subcommand can take any number of parameters after the last
    /// parameter.
    /// @note This is a mutually-exclusive feature.
    bool variadic = false;

    /// Nested options for this subcommand.
    /// @note This is a mutually-exclusive feature.
    std::vector<std::shared_ptr<option_template>> nested_options;

    /// Nested subcommands for this subcommand.
    std::vector<std::shared_ptr<subcommand_template>> nested_subcommands;
};

} // namespace optrone
