# Optrone Argument Parser
[![Uses C++23](https://img.shields.io/badge/C++-23-blue.svg)](https://en.cppreference.com/w/cpp)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](license.md)
[![Latest Version v0.0.1](https://img.shields.io/badge/Latest-v0.0.1-yellow.svg)](https://github.com/anstropleuton/optrone/releases/latest)

**Optrone** is a lightweight argument parser written in modern C++. It has **no dependencies\*, simple usage and is powerful**.

Optrone provides a **minimal yet expressive** way to predefine templates to parse command-line options or subcommands. It can even parse **Microsoft-style** arguments, not just limited to **POSIX-style** arguments.

Optrone can be used in any programs that requires a **consistent, moderately-complex and flexible** command-line argument handling, including support for options, subcommands, parameters, default values for parameters, nested options and nested subcommands.

Optrone **does not insist** that the command-line arguments needs to come from a command line, meaning you can indeed fetch the arguments from a **config-file**, split them and feed them to Optrone, thanks to its modular design.

\*: it does have a dependency for testing, but it is not enabled by default when Fluxins is added as a subdirectory.

# Prerequisite

- A C++ compiler that supports [C++23](https://en.cppreference.com/w/cpp/compiler_support/23)
- CMake 3.15 or higher
- [doctest](https://github.com/doctest/doctest) (for testing)

# Building

- Clone the repository
```bash
git clone https://github.com/anstropleuton/optrone.git
```
- Create a build directory
```bash
mkdir build
cd build
```
- Build the project
```bash
cmake ..
cmake --build . --config Release
```
- Install the project (optional; UNIX-like systems)
```bash
sudo cmake --install . --config Release
```

# Quick-Start Example

If you are ready to dive into the APIs, add your project as a subdirectory in your CMakeLists.txt:
```cmake
add_subdirectory(optrone)
```

If your project does not use CMake for your project, you can use CMake to build Optrone as a static library, set up include paths, and link the library to your project.

This example showcases how to set up basic and a few advanced templates and use them to parse the command-line arguments.

```cpp
// This is just a quick-start example. Also see the other examples from the
// example directory to learn more.

// Supports parsing POSIX-style (--long, -s, --long=value)
// and Microsoft-style (/LONG, /S, /LONG:VALUE) arguments.

#include <cstdlib>
#include <exception>
#include <memory>
#include <print>
#include <string>
#include <vector>

#include "optrone/optrone.hpp"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

// Setting up templates

// Basic option with short and long names
auto help_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Show help message.",
    .short_names = { 'h' },
    .long_names  = { "help" },
});

// Option with parameter and default value
auto verbose_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Set verbosity level (quiet, info, debug, trace).",
    .short_names = { 'v' },
    .long_names  = { "verbose" },
    .params      = { "level" },
    .defaults    = { "info" },
});

// Subcommand with parameter
auto get_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Get a value from a key.",
    .names       = { "get" },
    .params      = { "key" },
});

// Subcommand with two parameters and one default (right-anchored)
auto set_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Set a value of a key.",
    .names       = { "set" },
    .params      = { "key", "value" },
    .defaults    = { "0" },
});

// Option that is nested in a subcommand
auto list_sort_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Sort listed keys in order (none, ascending, descending).",
    .short_names = { 's' },
    .long_names  = { "sort" },
    .params      = { "key" },
    .defaults    = { "ascending" },
});

// Option with variadic args that is nested in a subcommand
auto list_filter_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Filter by globbing keys.",
    .short_names = { 'f' },
    .long_names  = { "filter" },
    .params      = { "glob" },
    .variadic    = true,
});

// Subcommand with nested option
auto list_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description    = "List all the keys and values.",
    .names          = { "list" },
    .nested_options = { list_filter_option },
});

// Templates

std::vector options = {
    help_option,
    verbose_option,
};

std::vector subcommands = {
    get_subcommand,
    set_subcommand,
    list_subcommand,
};

// Functions to handle each template to keep things organized

/// Handle `--help` option.
void handle_help_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::print("{}", optrone::format_saec(optrone::get_help_message(options, subcommands)));
    std::exit(0);
}

/// Handle `--verbose` option.
void handle_verbose_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string verbosity_level = arg.values[0]; // It is guaranteed to have values for all parameters

    // ...
}

/// Handle `get` subcommand.
void handle_get_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string key = arg.values[0];

    // ...
}

/// Handle `set` subcommand.
void handle_set_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string key   = arg.values[0];
    std::string value = arg.values[1];

    // ...
}

/// Handle `list --sort` option.
void handle_list_sort_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string key = arg.values[0];

    // ...
}

/// Handle `list --filter` option.
void handle_list_filter_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    auto globs = arg.values[0]; // Variadic parameters

    // ...
}

/// Handle `list` subcommand.
void handle_list_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    // Check for nested options
    while (i < args.size())
    {
        const optrone::parsed_argument &next_arg = args[i]; // Don't increment when seeking ahead

        if (auto option = next_arg.ref_option.lock(); option == list_sort_option)
        {
            handle_list_sort_option(args, i);
        }
        else if (option == list_filter_option)
        {
            handle_list_filter_option(args, i);
        }
        else
        {
            break;
        }
    }

    // ...
}

/// Main function
int main(int argc, char *argv[])
{
    // Parsing
    std::vector<optrone::parsed_argument> args;
    try
    {
        args = optrone::parse_arguments(std::vector<std::string>(argv + 1, argv + argc), options, subcommands);
    }
    catch (const optrone::argument_error &error)
    {
        // Error parsing due to invalid command-line argument
        std::print("{}", error.formatted_string); // Display error with preview
        return 1;
    }
    catch (const std::exception &error)
    {
        // Error parsing due to misconfigured templates?
        std::print("{}", error.what());
        return 1;
    }

    // Handle arguments...
    for (std::size_t i = 0; i < args.size();) // Incrementing happens in the handler functions
    {
        if (auto option = args[i].ref_option.lock(); option == help_option)
        {
            handle_help_option(args, i);
        }
        else if (option == verbose_option)
        {
            handle_verbose_option(args, i);
        }

        // Handle additional options...

        else if (auto subcommand = args[i].ref_subcommand.lock(); subcommand == get_subcommand)
        {
            handle_get_subcommand(args, i);
        }
        else if (subcommand == set_subcommand)
        {
            handle_set_subcommand(args, i);
        }
        else if (subcommand == list_subcommand)
        {
            handle_list_subcommand(args, i);
        }

        // Handle additional subcommands...

        else
        {
            break;
        }
    }
}
```

For other usage examples, including customizing functions or operators, see the examples in the [example](example) directory.

# Features

**Parsing features**:
- POSIX-style options:
  - Long option: `--option`.
  - Short option: `-s` (`-abc` will be split up into `-a`, `-b` and `-c`).
  - Parameters: `--option value` (or split with first `=`: `--option=value`).
- Microsoft-style options:
  - Long options: `/OPTION`.
  - Short option: `/S` (no splitting here).
  - Parameters: `/OPTION VALUE` (or split with first `:`: `/OPTION:VALUE`).
- Subcommands:
  - `./program subcommand`.
  - Parameters: `./program subcommand value`.
  - Nested options or nested subcommands: `./program subcommand nested-subcommand --option-for-subcommand`.
- Default values for parameters (right-anchored).
- Variadic parameters: `--option arg1 arg2 ...` (zero or more).
- Help-message generation: Provides the complete and fairly customizable help-message generation for all the provided templates, including nested templates.

**Other features**:
- **Error Reporting**: Parsing arguments can throw `argument_error` exception which contains information about the error, along with location of the error within a [reconstructed command-line](#reconstructed-command-line), such as invalid option or insufficient parameters provided, etc.

# Anti-features

- Parameter type specification or validation: **Not supported**. These are the responsibilities of the business logic, not parsing arguments.
- Generate documentations: **Not available**. Help message generation is provided, though it is only focused on the templates, not the overall application documentation.
- Shell auto-completions: **Not planned**. Auto-completions are, well, specific to shells. You could try using the 

# Limitations

There are several limitations put in place to guarantee deterministic behavior. These are:

- Long names **cannot be less than 2 characters long**, meaning it is not possible to parse arguments such as `--a`.
- Name comparisons are all done **case-insensitively**, e.g., no distinction between `-v` and `-V`.
- Names may not contain `:` or `=`, as they are used to split arguments.
  - Additionally, short names cannot be a single hyphen (`'-'`) as the parser will treat it as a long-name option.

# Future plans/considerations/TODOs

- Upgrade the error reporting be non-blocking, i.e., it will collect all the errors and emit them at once, as opposed to throwing at the first instance of an error.
- Allow global parameters, default values and variadicity setting. This allows program commands itself to act as a template: `./program value-1 value-2 ...` rather than `./program --option value-1 value-2 ...` or `./program subcommand value-1 value-2 ...`.
- Relax validation (?)
  - Is it good idea to throw for unsuspecting but invalid templates (e.g., uppercase letters being used in names), or
  - Is it good idea to internally modify them for a less-agressive validation?
- Easier "parse the parsed result" code.
  - Organize results in the nested manner to allow easier parsing of the parsed arguments.
  - Better yet, allow hooks or callbacks for parsing.

# General Information

## Mutually-Exclusive Features

Certain features in Optrone templates are mutually-exclusive, such as default values for parameters vs. variadic parameters. Mutually-exclusive implies that **you cannot use those features together**, e.g., you cannot provide **default values for parameters** and also enable **variadic parameters**.

For options, there are two mutually-exclusive features: **Default Values for Parameters** and **Variadic Parameters**.

For subcommands, there are three mutually-exclusive features: **Default Values for Parameters**, **Nested Subcommands** and **Variadic Parameters**.

## Default Values for Parameters

Default values for parameters are specified in **right-anchored manner**, meaning the values for default apples to **last N parameters, in order**. This can be best described with an example.

Suppose an option takes 5 values:  
`id`, `usernamae`, `password`, `encrypted`, `private-key`.  
To specify default values for last 2 parameters, in order, we would have default values such as:  
`no`, `this-is-a-private-key-tell-nobody-shhh`.  
Here, the `no` is the default value for the `encrypted` parameter and `this-is-a-private-key-tell-nobody-shhh` is the default value for `private-key` parameter.

## Reconstructed Command-Line

When reporting errors, Optrone internally tries to **reconstruct the command-line** from the list of arguments parsed as tokens. The reconstructed command-line is **different from the original input**, as the shell does perform some operations on the command-line before sending it to the program, such as **removing the quotation marks**, **evaluating globs (e.g., \*.txt)**, etc.

Note that even the reconstructed command-line **differs from the arguments** (`argc` and `argv`) provided from the main function, as they are constructed from the parsed tokens, e.g., arguments in the form `-abc` will appear as `-a -b -c` in the reconstructed command-line.

# More Examples

There are a few examples that I have created to demonstrate Optrone.
- [taskmgr.cpp](example/taskmgr.cpp): Demonstrates **general usage** of Optrone. This example implements a simple CLI Task Manager.

# Contributing

Feel free to contribute to this project by code (see [contributing.md](contributing.md)), by suggesting your ideas, or even by reporting a bug.

# Credits

Thanks to [jothepro](https://github.com/jothepro) for the stylesheet [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) for Doxygen.

Thanks to Nuno Pinheiro (can't find link) for the background Elarun. But I did find a [KDE store link](https://store.kde.org/p/1162360/) to the background image. I think you could consider that. If KDE developers are here, I would appreciate your help to find a proper link to the author.

Thanks to [patorjk](https://www.patorjk.com) for [Text to ASCII Art Generator](https://www.patorjk.com/software/taag).

# License

Copyright (c) 2025 Anstro Pleuton.

This project is licensed under the terms of MIT License. See [license.md](license.md) for more info.
