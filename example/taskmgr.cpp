/// @file
///
/// @authors   Anstro Pleuton <https://github.com/anstropleuton>
/// @copyright Copyright (c) 2025 Anstro Pleuton
///
/// This example file showcases general usage example for Optrone. This program
/// implements a CLI task-manager. It can be used to create and manage tasks.
///
/// Each task in the saved tasks.txt is a line to represent the task.
/// Each line in the saved tasks.txt is in the following format:
///
/// ```
/// <text>;<0/1 for done>;<priority>;<number of notes>;<number of tags>[;semicolon-separated notes][;semicolon-separated tags]
/// ```
///
/// Where text inside < and > are required and text inside [ and ] are optional.
///
/// This project is licensed under the terms of MIT License.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <format>
#include <fstream>
#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <print>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "optrone/error.hpp"
#include "optrone/help.hpp"
#include "optrone/parser.hpp"
#include "optrone/template.hpp"

/// A single task of the task manager.
struct task {
    std::string                     text;             ///< Task's description.
    bool                            done     = false; ///< Whether the task is done.
    std::size_t                     priority = 0;     ///< Priority of the task. Higher the number, higher the priority.
    std::vector<std::string>        notes;            ///< Notes for the task.
    std::unordered_set<std::string> tags;             ///< Tags of the task.
};

/// Check if two unordered sets has a common element.
template <typename set_a, typename set_b>
bool has_any_intersection(const set_a &a, const set_b &b)
{
    if (a.size() > b.size())
        return has_any_intersection(b, a); // Iterate over smaller set

    for (const auto &x : a)
    {
        if (b.contains(x))
            return true;
    }
    return false;
}

/// Construct set of indices from list of values.
std::unordered_set<std::size_t> get_indices(const std::vector<std::string> &values)
{
    // Obtain indices as a set in two steps:
    // 1. Convert values to ints.
    // 2. Convert vector to set.

    // clang-format off
    return values
        | std::views::transform([](const std::string &value) { return std::stoul(value); })
        | std::ranges::to<std::unordered_set>();
    // clang-format on
}

/// Filter out indices from a list.
template <typename type>
std::vector<type> filter_out(const std::vector<type> &values, const std::unordered_set<std::size_t> &indices)
{
    // Remove tasks with those indices in four steps:
    // 1. Obtain tasks and index of each tasks
    // 2. Filter tasks to exclude tasks with specific indices.
    // 3. Extract only tasks, leave index behind
    // 4. Assign this back to the tasks.

    // clang-format off
    return values | std::views::enumerate
        | std::views::filter([&](const auto &pair) { auto [index, _] = pair; return !indices.contains(index); })
        | std::views::transform([](const auto &pair) { auto [_, value] = pair; return value; })
        | std::ranges::to<std::vector>();
    // clang-format on
}

/// Match glob pattern with all the elements in the list.
/// @return Matched element indices.
std::vector<std::size_t> glob_match(std::string_view glob, const std::vector<std::string> list)
{
    // Convert glob to regex
    std::string pattern(glob);

    pattern = std::regex_replace(pattern, std::regex(R"(([\$\^\(\)\[\]\{\}\|\\]))"), R"(\$1)"); // Sanitize
    pattern = std::regex_replace(pattern, std::regex(R"([^\\]{0-1}(\*))"), ".*");               // Replace * with .*
    pattern = std::regex_replace(pattern, std::regex(R"([^\\]{0-1}(\?))"), ".");                // Replace ? with .
    pattern = "^" + pattern + "$";                                                              // Match full glob

    std::regex               regex(pattern);
    std::vector<std::size_t> matches;
    for (std::size_t i = 0; i < list.size(); i++)
    {
        if (std::regex_match(list[i], regex))
        {
            matches.emplace_back(i);
        }
    }

    return matches;
}

/// Read all the tasks from a file.
std::vector<task> read_tasks(const std::string &filename)
{
    std::ifstream ifile(filename);
    if (!ifile)
    {
        return {};
    }

    std::vector<task> tasks;

    std::string line;
    while (std::getline(ifile, line))
    {
        auto tokens = line | std::views::split(';') | std::ranges::to<std::vector<std::string>>();
        if (tokens.size() < 5)
        {
            throw std::runtime_error("Invalid tokens");
        }

        task task;
        task.text     = tokens[0];
        task.done     = std::stoi(tokens[1]) != 0;
        task.priority = std::stoul(tokens[2]);

        std::size_t notes_count = std::stoul(tokens[3]);
        std::size_t tags_count  = std::stoul(tokens[4]);

        if (tokens.size() != notes_count + tags_count + 5)
        {
            throw std::runtime_error("Invalid tokens");
        }

        task.notes.insert(task.notes.begin(), tokens.begin() + 5, tokens.end() - tags_count);
        task.tags.insert(tokens.begin() + 5 + notes_count, tokens.end());
        tasks.emplace_back(task);
    }

    return tasks;
}

/// Write all the tasks to a file.
void write_tasks(const std::string &filename, const std::vector<task> &tasks)
{
    std::ofstream ofile(filename);
    if (!ofile)
    {
        return;
    }

    for (const task &task : tasks)
    {
        std::string line = std::format("{};{};{};{};{}", task.text, task.done ? 1 : 0, task.priority, task.notes.size(), task.tags.size());
        if (!task.notes.empty()) line += ";" + (std::views::join_with(task.notes, std::string(";")) | std::ranges::to<std::string>());
        if (!task.tags.empty()) line += ";" + (std::views::join_with(task.tags, std::string(";")) | std::ranges::to<std::string>());
        ofile << line << std::endl;
    }
}

// Setting up templates

// --help
auto help_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Show help message.",
    .short_names = { 'h' },
    .long_names  = { "help" },
});

// --version
auto version_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Show version information.",
    .short_names = { 'v' },
    .long_names  = { "version" },
});

// --file
auto file_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "File for the list of tasks to save and load.",
    .short_names = { 'f' },
    .long_names  = { "file" },
    .params      = { "filename" },
    .defaults    = { "tasks.txt" },
});

// add
auto add_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Add a task to the tasks list.",
    .names       = { "add" },
    .params      = { "text" },
});

// remove
auto remove_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Remove task(s) from the tasks list.",
    .names       = { "remove" },
    .params      = { "task index" },
    .variadic    = true,
});

// auto-remove
auto auto_remove_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Automatically remove completed tasks.",
    .names       = { "auto-remove", "remove-done" },
});

// list --include-notes
auto list_include_notes_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Sort tasks with notes included",
    .short_names = { 'n' },
    .long_names  = { "include-notes" },
});

// list --filter
auto list_filter_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Filter task by tags",
    .short_names = { 'f' },
    .long_names  = { "filter" },
    .params      = { "tags" },
    .variadic    = true,
});

// list --sort
auto list_sort_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Sort tasks in specific order (index, priority, completion, ascending, descending, notes, tags)",
    .short_names = { 's' },
    .long_names  = { "sort" },
    .params      = { "key" },
    .defaults    = { "priority" },
});

// list
auto list_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description    = "List task(s) from the tasks list.",
    .names          = { "list" },
    .nested_options = { list_include_notes_option, list_filter_option, list_sort_option },
});

// done
auto done_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Mark task(s) as done.",
    .names       = { "done" },
    .params      = { "task index" },
    .variadic    = true,
});

// undo
auto undo_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Unmark task(s) as done.",
    .names       = { "undo" },
    .params      = { "task index" },
    .variadic    = true,
});

// edit text
auto edit_text_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Edit task's text.",
    .names       = { "text" },
    .params      = { "task index", "text" },
});

// edit priority
auto edit_priority_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Edit task's priority.",
    .names       = { "priority" },
    .params      = { "task index", "priority" },
    .defaults    = { "0" }, // Zero priority if none provided
});

// edit
auto edit_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description        = "Edit the task.",
    .names              = { "edit" },
    .nested_subcommands = { edit_text_subcommand, edit_priority_subcommand },
});

// notes add
auto notes_add_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Add note(s) to the task.",
    .names       = { "add" },
    .params      = { "task index", "notes" },
    .variadic    = true,
});

// notes remove
auto notes_remove_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Remove note(s) from the task.",
    .names       = { "remove" },
    .params      = { "task index", "note index" },
    .variadic    = true,
});

// notes list --sort
auto notes_list_sort_option = std::make_shared<optrone::option_template>(optrone::option_template{
    .description = "Sort notes in specific order (index, ascending, descending)",
    .short_names = { 's' },
    .long_names  = { "sort" },
    .params      = { "key" },
    .defaults    = { "ascending" },
});

// notes list
auto notes_list_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description    = "List notes from the task(s).",
    .names          = { "list" },
    .params         = { "task index" },
    .variadic       = true,
    .nested_options = { notes_list_sort_option },
});

// notes
auto notes_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description        = "Edit notes of the task",
    .names              = { "notes" },
    .nested_subcommands = { notes_add_subcommand, notes_remove_subcommand, notes_list_subcommand }
});

// tags add
auto tags_add_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Add tag(s) to the task.",
    .names       = { "add" },
    .params      = { "task index", "tags" },
    .variadic    = true,
});

// tags remove
auto tags_remove_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "Remove tag(s) from the task.",
    .names       = { "remove" },
    .params      = { "task index", "tags" },
    .variadic    = true,
});

// tags list
auto tags_list_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description = "List tags from the task(s).",
    .names       = { "list" },
    .params      = { "task index" },
    .variadic    = true,
});

// tags
auto tags_subcommand = std::make_shared<optrone::subcommand_template>(optrone::subcommand_template{
    .description        = "Edit tags of the task",
    .names              = { "tags" },
    .nested_subcommands = { tags_add_subcommand, tags_remove_subcommand, tags_list_subcommand }
});

// Templates

std::vector options = {
    help_option,
    version_option,
    file_option,
};

std::vector subcommands = {
    add_subcommand,
    remove_subcommand,
    auto_remove_subcommand,
    list_subcommand,
    done_subcommand,
    undo_subcommand,
    edit_subcommand,
    notes_subcommand,
    tags_subcommand
};

// Globals

std::vector<task> tasks;
std::string       tasks_file   = "tasks.txt";
std::string       program_name = "./optrone_usage_example";

// Subcommand-specific globals

bool                                                                                                list_include_notes = false;
std::unordered_set<std::string>                                                                     list_filter_tags;
std::function<bool(const std::tuple<long, task> &a, const std::tuple<long, task> &b)>               list_sort_compare;
std::function<bool(const std::tuple<long, std::string> &a, const std::tuple<long, std::string> &b)> notes_list_sort_compare;

/// Handle `--help` option.
void handle_help_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::print("{}", optrone::format_saec(optrone::get_help_message(options, subcommands)));
    std::exit(0);
}

/// Handle `--version` option.
void handle_version_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::println("Optrone Usage Example (the \"Task Manager\")");
    std::println("Version 1.0.0");
    std::println("Copyright (c) 2025 Anstro Pleuton.");
    std::println("This project is licensed under the terms of MIT License.");
    std::exit(0);
}

/// Handle `--file` option.
void handle_file_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    tasks_file = arg.values[0];
}

/// Handle `add` subcommand.
void handle_add_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    tasks.emplace_back(arg.values[0]);
}

/// Handle `remove` subcommand.
void handle_remove_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    tasks = filter_out(tasks, get_indices(arg.values));
}

/// Handle `auto-remove` subcommand.
void handle_auto_remove_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    tasks = tasks | std::views::filter([](const task &task) { return !task.done; }) | std::ranges::to<std::vector>();
}

/// Handle `list --filter` option.
void handle_list_filter_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    list_filter_tags = std::unordered_set(arg.values.begin(), arg.values.end());
}

/// Handle `list --include-notes` option.
void handle_list_include_notes_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    list_include_notes = true;
}

/// Handle `list --sort` option.
void handle_list_sort_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string sorter = arg.values[0];

    if (sorter == "index")
    {
        // Do nothing
    }
    else if (sorter == "priority")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return task_a.priority > task_b.priority;
        };
    }
    else if (sorter == "completion")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return task_a.done > task_b.done;
        };
    }
    else if (sorter == "ascending")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return !task_a.text.empty() && !task_b.text.empty() && task_a.text[0] < task_b.text[0];
        };
    }
    else if (sorter == "descending")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return !task_a.text.empty() && !task_b.text.empty() && task_a.text[0] > task_b.text[0];
        };
    }
    else if (sorter == "notes")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return task_a.notes.size() > task_b.notes.size();
        };
    }
    else if (sorter == "tags")
    {
        list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, task_a] = a;
            auto [index_b, task_b] = b;
            return task_a.tags.size() > task_b.tags.size();
        };
    }
    else
    {
        std::println("Invalid sorter for `list --sort`.");
        std::println("Try `{} --help` for more information.", program_name);
        std::exit(1);
    }
}

/// Handle `list` subcommand.
void handle_list_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    bool include_notes = false;

    // Check for nested options
    while (i < args.size())
    {
        const optrone::parsed_argument &next_arg = args[i];

        if (auto option = next_arg.ref_option.lock(); option == list_filter_option)
        {
            handle_list_filter_option(args, i);
        }
        else if (option == list_include_notes_option)
        {
            handle_list_include_notes_option(args, i);
        }
        else if (option == list_sort_option)
        {
            handle_list_sort_option(args, i);
        }
        else
        {
            break;
        }
    }

    // Filter tasks by tags

    // clang-format off
    auto list_tasks = read_tasks(tasks_file)
        | std::views::enumerate
        | std::views::filter([&](const auto &pair) { auto [_, task] = pair; return list_filter_tags.empty() || has_any_intersection(task.tags, list_filter_tags); })
        | std::ranges::to<std::vector>();
    // clang-format on

    // Sort the tasks
    if (list_sort_compare)
    {
        std::ranges::sort(list_tasks, list_sort_compare);
    }

    // Print the tasks
    for (const auto &tuple : list_tasks)
    {
        auto [index, task] = tuple;

        // clang-format off
        std::println("{}. [{}] (P{}): {} {}", index, task.done ? "x" : " ", task.priority, task.text,
                     std::views::transform(task.tags, [](const std::string &str) { return "[" + str + "]"; })
                     | std::views::join
                     | std::ranges::to<std::string>());
        // clang-format on

        if (include_notes)
        {
            for (const std::string &note : task.notes)
            {
                std::println("  -> {}", note);
            }
        }
    }
}

/// Handle `done` subcommand.
void handle_done_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    for (const std::string &value : arg.values)
    {
        std::size_t index    = std::stoul(value);
        tasks.at(index).done = true;
    }
}

/// Handle `undo` subcommand.
void handle_undo_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    for (const std::string &value : arg.values)
    {
        std::size_t index    = std::stoul(value);
        tasks.at(index).done = false;
    }
}

/// Handle `edit text` subcommand.
void handle_edit_text_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t index    = std::stoul(arg.values[0]);
    tasks.at(index).text = arg.values[1];
}

/// Handle `edit priority` subcommand.
void handle_edit_priority_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t index        = std::stoul(arg.values[0]);
    tasks.at(index).priority = std::stoul(arg.values[1]);
}

/// Handle `edit` subcommand.
void handle_edit_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    i++; // Skip 'edit'

    if (i >= args.size())
    {
        std::println("Missing subcommand for `edit`.");
        std::println("Usage: {} edit <subcommand> [arg]...", program_name);
        std::println("Try `{} --help` for more information.", program_name);
        std::exit(1);
    }

    const optrone::parsed_argument &arg = args[i]; // Don't skip subcommand

    if (auto subcommand = arg.ref_subcommand.lock(); subcommand == edit_text_subcommand)
    {
        handle_edit_text_subcommand(args, i);
    }
    else if (subcommand == edit_priority_subcommand)
    {
        handle_edit_priority_subcommand(args, i);
    }
}

/// Handle `notes add` subcommand.
void handle_notes_add_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t index = std::stoul(arg.values[0]);
    tasks.at(index).notes.insert(tasks.at(index).notes.end(), arg.values.begin() + 1, arg.values.end());
}

/// Handle `notes remove` subcommand.
void handle_notes_remove_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t task_index     = std::stoul(arg.values[0]);
    auto        note_indices   = get_indices(std::vector(arg.values.begin() + 1, arg.values.end())); // Exclude first value (task index)
    tasks.at(task_index).notes = filter_out(tasks.at(task_index).notes, note_indices);
}

/// Handle `notes list --sort` option.
void handle_notes_list_sort_option(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::string sorter = arg.values[0];

    if (sorter == "index")
        ; // Do nothing
    else if (sorter == "ascending")
        notes_list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, string_a] = a;
            auto [index_b, string_b] = b;
            return !string_a.empty() && !string_b.empty() && string_a[0] < string_b[0];
        };
    else if (sorter == "descending")
        notes_list_sort_compare = [](const auto &a, const auto &b) {
            auto [index_a, string_a] = a;
            auto [index_b, string_b] = b;
            return !string_a.empty() && !string_b.empty() && string_a[0] > string_b[0];
        };
    else
    {
        std::println("Invalid sorter for `notes list --sort`.");
        std::println("Try `{} --help` for more information.", program_name);
        std::exit(1);
    }
}

/// Handle `notes list` subcommand.
void handle_notes_list_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    // Check for nested options
    while (i < args.size())
    {
        const optrone::parsed_argument &next_arg = args[i];

        if (auto option = next_arg.ref_option.lock(); option == notes_list_sort_option)
        {
            handle_notes_list_sort_option(args, i);
        }
        else
        {
            break;
        }
    }

    // Print notes for each task indices provided
    for (const std::string &value : arg.values)
    {
        std::size_t task_index = std::stoul(value);
        auto        list_notes = tasks.at(task_index).notes | std::views::enumerate | std::ranges::to<std::vector>();

        // Sort the notes
        if (notes_list_sort_compare)
        {
            std::ranges::sort(list_notes, notes_list_sort_compare);
        }

        // Print the notes list for each task
        std::println("Task {}: {}", task_index, tasks.at(task_index).text);
        for (const auto &tuple : list_notes)
        {
            auto [note_index, note] = tuple;
            std::println("-> {}: {}", note_index, note);
        }
    }
}

/// Handle `notes` subcommand.
void handle_notes_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    i++; // Skip 'notes'

    if (i >= args.size())
    {
        std::println("Missing subcommand for `notes`.");
        std::println("Usage: {} notes <subcommand> [arg]...", program_name);
        std::println("Try `{} --help` for more information.", program_name);
        std::exit(1);
    }

    const optrone::parsed_argument &arg = args[i]; // Don't skip subcommand

    // Handle subcommands
    if (auto subcommand = arg.ref_subcommand.lock(); subcommand == notes_add_subcommand)
    {
        handle_notes_add_subcommand(args, i);
    }
    else if (subcommand == notes_remove_subcommand)
    {
        handle_notes_remove_subcommand(args, i);
    }
    else if (subcommand == notes_list_subcommand)
    {
        handle_notes_list_subcommand(args, i);
    }
}

/// Handle `tags add` subcommand.
void handle_tags_add_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t index = std::stoul(arg.values[0]);
    tasks.at(index).tags.insert(arg.values.begin() + 1, arg.values.end());
}

/// Handle `tags remove` subcommand.
void handle_tags_remove_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    std::size_t task_index = std::stoul(arg.values[0]);

    // List of tags to remove
    std::unordered_set tags_to_remove(arg.values.begin() + 1, arg.values.end()); // Exclude first value (task index)

    // Construct new tags list by taking the set difference
    std::unordered_set<std::string> new_tags;
    std::ranges::set_difference(tasks.at(task_index).tags, tags_to_remove, std::inserter(new_tags, new_tags.begin()));

    // Assign new tags list back
    tasks.at(task_index).tags = new_tags;
}

/// Handle `tags list` subcommand.
void handle_tags_list_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    const optrone::parsed_argument &arg = args[i++];

    for (const std::string &value : arg.values)
    {
        std::size_t task_index = std::stoul(value);

        std::println("Task {}: {}", task_index, tasks.at(task_index).text);
        for (const std::string &tag : tasks.at(task_index).tags)
        {
            std::println("-> {}", tag);
        }
    }
}

/// Handle `tags` subcommand.
void handle_tags_subcommand(const std::vector<optrone::parsed_argument> &args, std::size_t &i)
{
    i++; // Skip 'tags'

    if (i >= args.size())
    {
        std::println("Missing subcommand for `tags`.");
        std::println("Usage: {} tags <subcommand> [arg]...", program_name);
        std::println("Try `{} --help` for more information.", program_name);
        std::exit(1);
    }

    const optrone::parsed_argument &arg = args[i]; // Don't skip subcommand

    // Handle subcommands
    if (auto subcommand = arg.ref_subcommand.lock(); subcommand == tags_add_subcommand)
    {
        handle_tags_add_subcommand(args, i);
    }
    else if (subcommand == tags_remove_subcommand)
    {
        handle_tags_remove_subcommand(args, i);
    }
    else if (subcommand == tags_list_subcommand)
    {
        handle_tags_list_subcommand(args, i);
    }
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

    if (argc >= 1) program_name = argv[0];

    if (args.empty())
    {
        std::println("Usage: {} [option]... <command> [arg]...", program_name);
        std::println("Try `{} --help` for more information.", program_name);
        return 1;
    }

    // Parsing the parsed args

    // Iterating by index to handle nesting
    for (std::size_t i = 0; i < args.size();) // Incrementing is done in handlers
    {
        if (auto option = args[i].ref_option.lock(); option == help_option)
        {
            handle_help_option(args, i);
        }
        else if (option == version_option)
        {
            handle_version_option(args, i);
        }
        else if (option == file_option)
        {
            handle_file_option(args, i);
        }

        else if (auto subcommand = args[i].ref_subcommand.lock(); subcommand == add_subcommand)
        {
            handle_add_subcommand(args, i);
        }
        else if (subcommand == remove_subcommand)
        {
            handle_remove_subcommand(args, i);
        }
        else if (subcommand == auto_remove_subcommand)
        {
            handle_auto_remove_subcommand(args, i);
        }
        else if (subcommand == list_subcommand)
        {
            handle_list_subcommand(args, i);
        }
        else if (subcommand == done_subcommand)
        {
            handle_done_subcommand(args, i);
        }
        else if (subcommand == undo_subcommand)
        {
            handle_undo_subcommand(args, i);
        }
        else if (subcommand == edit_subcommand)
        {
            handle_edit_subcommand(args, i);
        }
        else if (subcommand == notes_subcommand)
        {
            handle_notes_subcommand(args, i);
        }
        else if (subcommand == tags_subcommand)
        {
            handle_tags_subcommand(args, i);
        }

        else
        {
            break;
        }
    }
}
