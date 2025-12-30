/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This header file provides error reporting mechanism as well as a way to
/// specify location of certain constructs in the command-line argument, such
/// as the name of a template.
///
/// This project is licensed under the terms of MIT License.

#pragma once

#include <cstddef>
#include <exception>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace optrone {

/// Formats the Shorthand ANSI Escape Code (AEC) (SAEC) and replaces them with
/// actual AECs.
///
/// The SAEC is a concise way to represent a few common AECs. It starts with a
/// dollar sigh followed by a single character that represents the AEC, such as
/// `$r` for Red (`"\x1b[31m"`), `$*` for Bold (`"\x1b[1m"`) or `$0` for
/// Reset (`"\x1b[0m"`).
///
/// Here is the full list:
/// - `$0` - Reset          (`"\x1b[0m"`).
/// - `$*` - Bold           (`"\x1b[1m"`).
/// - `$_` - Underline      (`"\x1b[4m"`).
/// - `$k` - Black          (`"\x1b[30m"`).
/// - `$r` - Red            (`"\x1b[31m"`).
/// - `$g` - Green          (`"\x1b[32m"`).
/// - `$y` - Yellow         (`"\x1b[33m"`).
/// - `$b` - Blue           (`"\x1b[34m"`).
/// - `$m` - Magenta        (`"\x1b[35m"`).
/// - `$c` - Cyan           (`"\x1b[36m"`).
/// - `$w` - White          (`"\x1b[37m"`).
/// - `$K` - Bright Black   (`"\x1b[90m"`).
/// - `$R` - Bright Red     (`"\x1b[91m"`).
/// - `$G` - Bright Green   (`"\x1b[92m"`).
/// - `$Y` - Bright Yellow  (`"\x1b[93m"`).
/// - `$B` - Bright Blue    (`"\x1b[94m"`).
/// - `$M` - Bright Magenta (`"\x1b[95m"`).
/// - `$C` - Bright Cyan    (`"\x1b[96m"`).
/// - `$W` - Bright White   (`"\x1b[97m"`).
///
/// To preserve the dollar sign, add another dollar sign before it to escape it.
///
/// @param unformat Don't replace with real AEC, just remove the SAEC.
std::string format_saec(std::string_view string, bool unformat = false);

/// Sanitizes the string so it will not be formatted from the SAEC formatter.
/// (escapes each dollar sign with another dollar sign).
std::string sanitize_saec(std::string_view string);

/// Get a list of lines from the string.
///
/// Each element is a pair of two integers that point to begin of the line
/// and the length of the string.
std::vector<std::pair<std::size_t, std::size_t>> get_lines(std::string_view string);

/// Get a line's row index and column index (both 0-based).
std::pair<std::size_t, std::size_t> get_line_row_col(const std::vector<std::pair<std::size_t, std::size_t>> &lines, std::size_t pos);

/// Customize the text range preview.
struct preview_customizer {
    std::string_view begin_marker   = "<";   ///< Mark the beginning of the range.
    std::string_view end_marker     = ">";   ///< Mark the end of the range.
    std::string_view pointer_marker = "^";   ///< Mark the pointer.
    std::string_view underline      = "~";   ///< Underline for the text within the range.
    std::string_view ln_separator   = " | "; ///< Separator to separate line numbers.

    std::string_view marker_style      = ""; ///< Style the marker using SAEC.
    std::string_view normal_text_style = ""; ///< Style the normal text (text that isn't in range) using SAEC..
    std::string_view marked_text_style = ""; ///< Style the marked text (text that is in range) using SAEC..

    bool enable_line_number = true; ///< Enable or disable line number (also toggles line number separator).
};

/// A way to point at specific part of a text.
struct text_range {
    std::size_t begin   = 0; ///< Beginning of the range.
    std::size_t length  = 0; ///< Length of the range.
    std::size_t pointer = 0; ///< Pointer within the text (relative) that is the important part.
};

/// Preview the text with a given range.
///
/// The preview text is in the following format:
/// ```
/// 123 | A line that is being previewed given a range.
///     |                <~~~~~^~~~~~~~>
/// ```
/// Here, the phrase "being previewed" is in the range and is marked. The
/// pointer points to thet character 'p'.
///
/// It can also span multiple lines, in which case the preview text will
/// look like:
/// ```
/// 122 | Lorem ipsum dolor sit amet. Consectetur adipiscing elit. Sed do
///     |                             <~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// 123 | eiusmod tempor incididunt ut labore et dolore magna aliqua, ut enim ad
///     | ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~
/// 124 | minim veniam. Quis nostrud exercitation ullamco laboris.
///     | ~~~~~~~~~~~~>
/// ```
///
/// @note The text is unformatted and contains SAEC.
/// @see format_saec to format the text.
std::string preview_range(
    std::string_view   text,
    text_range         range,
    int                indent     = 0,
    preview_customizer customizer = preview_customizer());

/// Error when parsing command-line arguments, also contains details for error
/// and range of the error in the command line.
struct argument_error : std::exception {
    std::string message;  ///< The exception message.
    std::string cmd_line; ///< The command line constructed from the list of arguments (approx).
    text_range  range;    ///< Range in the args that caused the error.

    std::string message_with_preview; ///< Stores the message and preview (includes SAEC).

    /// Initializes the exception. Also creates the formatted message.
    argument_error(std::string_view message, std::string_view cmd_line, text_range range);

    /// Obtain the error message (unformatted).
    const char *what() const noexcept override
    {
        // Something went wrong, just return message (unformatted).
        if (message_with_preview.empty())
        {
            return message.c_str();
        }

        return message_with_preview.c_str();
    }
};

} // namespace optrone
