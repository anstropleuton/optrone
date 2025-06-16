/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This source provides implementation for misc. stuff in Optrone.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "optrone/error.hpp"

std::string optrone::format_saec(std::string_view string, bool unformat)
{
    static const std::unordered_map<char, std::string> saecs = {
        { '0', "\x1b[0m"  },
        { '*', "\x1b[1m"  },
        { '_', "\x1b[4m"  },
        { 'k', "\x1b[30m" },
        { 'r', "\x1b[31m" },
        { 'g', "\x1b[32m" },
        { 'y', "\x1b[33m" },
        { 'b', "\x1b[34m" },
        { 'm', "\x1b[35m" },
        { 'c', "\x1b[36m" },
        { 'w', "\x1b[37m" },
        { 'K', "\x1b[90m" },
        { 'R', "\x1b[91m" },
        { 'G', "\x1b[92m" },
        { 'Y', "\x1b[93m" },
        { 'B', "\x1b[94m" },
        { 'M', "\x1b[95m" },
        { 'C', "\x1b[96m" },
        { 'W', "\x1b[97m" },
    };

    std::string result;

    for (std::size_t i = 0; i < string.size(); i++)
    {
        // Normal character
        if (string[i] != '$')
        {
            result += string[i];
            continue;
        }

        // Dollar at the end
        if (i + 1 >= string.size())
        {
            result += string[i];
            continue;
        }

        char next = string[i + 1];

        // Escape SAEC
        if (next == '$')
        {
            result += '$';
            i++;
            continue;
        }

        // Unknown SAEC
        if (!saecs.contains(next))
        {
            result += '$';
            result += next;
            i++;
            continue;
        }

        // Replace SAEC
        if (!unformat) result += saecs.at(next);
        i++;
    }

    return result;
}

std::string optrone::sanitize_saec(std::string_view string)
{
    return std::regex_replace(std::string(string), std::regex("\\$"), "$$");
}

std::vector<std::pair<std::size_t, std::size_t>> optrone::get_lines(std::string_view string)
{
    std::vector<std::pair<std::size_t, std::size_t>> line_infos;
    {
        std::size_t pos = 0;
        while (pos <= string.size())
        {
            std::size_t eol = string.find('\n', pos);

            if (eol == std::string_view::npos) eol = string.size();

            line_infos.emplace_back(pos, eol - pos);
            pos = eol + 1; // +1 to skip '\n'
        }
    }

    return line_infos;
}

std::pair<std::size_t, std::size_t> optrone::get_line_row_col(const std::vector<std::pair<std::size_t, std::size_t>> &lines, std::size_t pos)
{
    for (std::size_t i = 0; i < lines.size(); i++)
    {
        std::size_t begin  = lines[i].first;
        std::size_t length = lines[i].second;
        std::size_t end    = begin + length;

        if (pos >= begin && pos < end)
        {
            std::size_t col = pos - begin;
            return { i, col };
        }
    }

    throw std::out_of_range("Position " + std::to_string(pos) + " is out of range");
}

std::string optrone::preview_range(std::string_view string, text_range range, int padding, preview_customizer customizer)
{
    std::size_t end = range.begin + range.length;

    // Build lines info
    auto line_infos = get_lines(string);

    // Width of line numbers
    std::size_t ln_width = std::to_string(line_infos.size()).size();
    std::string ln_spaces(ln_width, ' ');
    std::string indent_spaces(padding, ' ');

    std::ostringstream oss;

    for (std::size_t i = 0; i < line_infos.size(); i++)
    {
        auto [line_begin, line_length] = line_infos[i];
        std::size_t      line_end      = line_begin + line_length;
        std::string_view line          = string.substr(line_begin, line_length);

        if (line_end <= range.begin || line_begin >= end)
        {
            // No overlap
            continue;
        }

        // Content line
        // [indent][line number][separator][prefix/marked/suffix text]

        // Indent
        oss << indent_spaces;

        // Line number and separator
        if (customizer.enable_line_number)
        {
            oss << std::setw(ln_width) << (i + 1) << customizer.ln_separator;
        }

        // Marked text regions
        std::size_t mark_begin = std::max(0zu, range.begin - line_begin);
        std::size_t mark_end   = std::min(line_length, end - line_begin);

        // Prefix (normal text before range)
        if (mark_begin > 0)
        {
            oss << customizer.normal_text_style << line.substr(0, mark_begin) << "$0";
        }

        // Marked (text within range)
        oss << customizer.marked_text_style << line.substr(mark_begin, mark_end - mark_begin) << "$0";

        // Suffix (normal text after range)
        if (mark_end < line_length)
        {
            oss << customizer.normal_text_style << line.substr(mark_end) << "$0";
        }

        oss << std::endl;

        // Marker line
        // [indent][spaces of line number][separator][space up to marker][marker]

        // Indent
        oss << indent_spaces;

        // Spaces of line number and separator
        if (customizer.enable_line_number)
        {
            oss << std::string(ln_width, ' ') << customizer.ln_separator;
        }

        // Space up to marker
        oss << std::string(mark_begin, ' ');

        // Marker
        oss << customizer.marker_style;

        for (std::size_t j = mark_begin; j < mark_end; j++)
        {
            if (range.pointer >= line_begin && range.pointer < line_end && j == range.pointer - line_begin)
                oss << "^";
            else if (j == mark_begin)
                oss << "<";
            else if (j == mark_end - 1)
                oss << ">";
            else
                oss << "~";
        }

        oss << "$0" << std::endl;
    }

    return oss.str();
}

optrone::argument_error::argument_error(std::string_view message, std::string_view cmd_line, text_range range)
try
    : message(message), cmd_line(cmd_line), range(range)
{
    auto line_infos             = get_lines(cmd_line);
    auto [begin_row, begin_col] = get_line_row_col(line_infos, range.begin);
    auto [end_row, end_col]     = get_line_row_col(line_infos, range.begin + range.length - 1);

    std::ostringstream oss;
    oss << (begin_row + 1) << ":" << begin_col << "-" << (end_row + 1) << ":" << end_col << ": " << message << std::endl
        << preview_range(cmd_line, range);
    string = oss.str();

    formatted_string   = format_saec(string);
    unformatted_string = format_saec(string, true);
}
catch (const std::exception &e)
{
}
