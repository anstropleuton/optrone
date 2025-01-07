# Anstro Pleuton's Coding Style Guidelines
There is a lot of things here, mostly because I unnecessarily aim for perfection. No pressure to follow every single one of them, but you can keep them in mind.

The guidelines starts with a basic general generic regular simple coding style that everyone may have used at some point and slowly transitions to the Anstro Pleuton's way of programming, where each previous guidelines are applied to the proceeding guidelines.

## Naming Conventions
A few things in common
- Don't abbreviate names that are public APIs. Acronyms are allowed in any case.
- Don't use single letter names unless it is index of the `for` loop or is related to Mathematics.

Let's get to specific naming conventions:

### Naming Variables
Use `snake_case`, and try to keep them short yet descriptive.
```cpp
int users_count;
```

### Naming Constants
Same rules as [Naming Variables](#naming-variables)'s.
```cpp
const int max_users;
```

### Naming Classes
Same rules as [Naming Variables](#naming-variables)'s, but prefer keeping them small.
```cpp
class user
{
    // ...
};
```

### Naming Enumerators
Same rules as [Naming Classes](#naming-classes)'s.
```cpp
enum user_type
{
    // ...
};
```

### Naming Functions
Same rules as [Naming Variables](#naming-variables)'s.
```cpp
user create_user(...);
```

### Naming Namespaces
Same rules as [Naming Classes](#naming-classes)'s, except prefer keeping it even more short. An acronym should be good, but abbreviations are also allowed.
```cpp
namespace nsp
{
    // ...
}
```

### Naming Template Parameters
Same rules as [Naming Classes](#naming-classes)'s.

### Naming Files
Same rules as [Naming Variables](#naming-variables)'s. Also, use specificity-first style for naming files with multiple words, like `animated_icon_texture.gif` instead of `texture_icon_animated.gif`.
```
$ tree
.
├── res
│   ├── nirovan_logo.png
│   ├── nirovan_logo_dark.png
│   └── nirovan_logo_light.png
└── include
    └── alce_library.hpp
    └── plons_library.hpp
```

## Formatting
You may be better off using Uncrustify with the provided config [uncrustify.cfg](uncrustify.cfg) to format your code. If you don't have access to Uncrustify for whatever reason and would like to manually format the code, here it is:

### Indentation
Use 4 spaces for indentation, not tabs. And, do not indent namespace contents.
```cpp
namespace nsp
{
int value;

void set_value()
{
    value = 21;
}
}
```

### Brace Style
Use Allman style braces for code (control flow, function, lambda, etc.).
```cpp
void function()
{
    // Code

    // or
    if (condition)
    {
        // Code
    }
}
```
And K&R style braces for declarations (classes, structures, enum, namespace, etc.).
```cpp
class user_details {
    // Declarations
};
```

### Spacing
The average Joe style, except put space in between braces too.
```cpp
int user_id = rand() % 255;
```

### Line Limit
Limit lines to not exceed 80 characters. Why so old? Am I still running a computer with a terminal that is 80 characters long? No. I use split screen and only have 1920x1080 resolution monitor. Still, why? I use split view to edit files side-by-side, or at least view from one file and write in other.

### Function Wrap
Split the function parameters if the function signature is exceeding 80 characters.
```cpp
std::string get_users_in_details(
    bool username,
    bool password,
    bool registration_date,
    bool last_login_date,
    bool raw_details
)
{
    // ...
}
```

## Comments
Self-explanatory, I hope.

### Single-line Comments
Place single-line comment (`//`) above or after the code.
```cpp
// It is named this way for a very particular reason
int yet_another_user_variable;

int user_2; // Not so much for this one
```

### Multi-line Comments
Use multi-line comments (`/**/`) with one space before asterisk and two space after it.
```cpp
/*
 *  Don't use it for single line message and inside code though
 */
```

### Documentation Comments
Use Doxygen-style comments for documentation.
```cpp
/**
 *  @brief  Function description.
 *  @param  a  First parameter.
 *  @param  b  Second parameter.
 *  @return  Return description.
 */
```

### Punctuations
Use periods to end the sentence in a document comment, and not in regular comment.
```cpp
/*
 *  This comment ends with a period.
 */

// This comment does not end with a period though
```
Use two space in comment after `.`, `!` and `?` punctuations.
```cpp
/*
 *  This is a sentence.  This is another sentence after period!  Is this a
 *  life-long question?  And the end.
 */
```

### File Comment
Have a documentation comment on the first line of the file describing the file.
`header.hpp`:
```cpp
/**
 *  @author  Your Name (your.mail@email.provider)
 *  @brief   This is a header file.
 *
 *  @copyright  Copyright (C) 2024 I own this code to death
 *
 *  SUPER FANCY ART (OPTIONAL).
 *
 *  Detailed description.
 *
 *  License info.
 *
 *  Credits.
 */
```

## Coding Standards
The style part ends there, now comes the standard/functionality part.

### Initialization
- Use this for initializing types that have literals (i.e., `int`, `std::string`, etc.):
```cpp
data_type variable_name = <value>;
```
- Use this if the type has a constructor with parameters:
```cpp
data_type variable_name(<parameters>);
```
- Use this if the constructor accepts initializer list:
```cpp
data_type variable_name = { <list> };
```

### Range-based Loops
Use `auto &` for range-based `for` loops.
```cpp
for (auto &value : values) { /* code */ }
```

### Variable Declaration
Use `auto` for declaring variables which is initialized from an expression. But do not use it for fundamental types (`int`, `float`, etc., including aliases like `std::size_t` or `std::uint64_t`).
```cpp
auto users = get_inactive_users();
```

### Function Definition
Use `auto` as return type to automatically deduce type, or use trailing function declaration if automatic return type deduction is inapplicable (i.e., is not an in-line function).
```cpp
auto get_active_users()
{
    return get_filtered_users(filter::active);
}
```

### `[[nodiscard]]`, `inline` and `constexpr` Functions
Use `[[nodiscard]]` attribute for almost all functions, except for the ones whose return value can freely be ignored. Use `inline` and `constexpr` together for a function whose definition is inside a header file.
```cpp
[[nodiscard]] inline constexpr auto add(int a, int b) -> int
{
    return a + b;
}
```

### Lambdas
Always use reference capture for lambda.
```cpp
auto lambda = [&]() { /* code */ };
```

### Static Functions
Mark a function `static` if it is defined in a source file with no declaration in header file. This will make the function private to that source file.
```cpp
static inline constexpr auto helper() -> void;
```

### Use `struct` instead of `class`
I don't like data hiding. I truly believe that user should have complete access to the memory that they have initialized. I want to provide much flexibility, not restrict them.

Note: I will still be calling them "Class" rather than "Structure".
```cpp
struct {
    // YEAH everything public!
};
```

### Class Organization
When writing a class, Organize members in this order:
- Data members
- Constructors
- Destructor
- Function members
- Friend operators (Operators declared as `friend`)
- Modifying operators (Operators not declared as `friend` and does modify members)
- Conversion operators
```cpp
struct password {

    // Data members
    std::string content;
    bool raw;

    // Constructors
    password() = default;
    password(std::string content, bool raw) : content(content), raw(raw) {};

    // Member functions
    [[nodiscard]] inline constexpr auto cook() // Inline function
    {
        raw = false;
        return cook(content);
    }

    auto cook_deeply() -> std::string; // Can't be inline

    // Friend operators
    [[nodiscard]] friend inline constexpr auto operator+(
        const password &a,
        const password &b
    )
    {
        return password(a.content + b.content, a.raw || b.raw);
    }

    // Modifying operators
    inline constexpr auto operator+=(const password &other) -> password &
    {
        *this = *this + other;
        return *this;
    }

    // Conversion operators
    [[nodiscard]] inline constexpr operator std::string()
    {
        return content;
    }

    [[nodiscard]] inline constexpr operator bool()
    {
        return raw;
    }
};
```

### Constructors
Prefer making a type aggregate unless you really need constructors.

### Enumerators
Include a `max` member in enumerators intended for use as indexes, and an `unknown` member intended to serve as uninitialized/default.

### Enum Classes
Always use `enum class` instead of `enum` for better type safety, with the only exception being when the `enum` is inside of a struct.

### Enum to String
Always create an overload of `to_string` for every `enum class` to enable convenient text formatting of the enum values, enhancing readability and usability in code.
```cpp
enum class enumerator {
    unknown,
    member_1,
    member_2,
    // ...
    max
};

[[nodiscard]] inline constexpr auto to_string(enumerator enumerator)
{
    using namespace std::string_literals;
    switch (enumerator)
    {
        case enumerator::unknown: return "unknown"s;
        case enumerator::member_1: return "member_1"s;
        case enumerator::member_2: return "member_2"s;
        // ...
        case enumerator::max: return "max"s;
    }
    return ""s;
}
```

### Concepts
Prefer using defined concepts over `typename` for template parameters.
```cpp
template<std::contiguous_iterator iter>
// stuff
```
