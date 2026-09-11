# brigadier-cpp

`brigadier-cpp` is a header-only C++23 command parser and dispatcher inspired by [Mojang's Brigadier](https://github.com/Mojang/brigadier), with command trees that can be constructed at compile time.

## Usage

```cpp
#include <brigadier/brigadier.hpp>
#include <string_view>

using namespace brigadier;

// Your command source type
struct player {
    bool muted_{};

    void broadcast_message(std::string_view message) {
        // ...
    }
};

constexpr auto make_broadcast_command() {
    return literal<"broadcast">()
        .must<[](const player& sender) -> constraint_result {
            if (sender.muted_) {
                return std::unexpected("Muted players cannot broadcast");
            }

            return {};
        }>()
        .then(
			argument<"message", greedy_text>()
                .executes<[](player& source, std::string_view message) -> execution_result {
                    source.broadcast_message(message);
                    return execution_result::success;
                }>()
        );
}

inline constexpr auto dispatcher = make_dispatcher<player>(make_broadcast_command());
```

Execute input at runtime:

```cpp
player source{ .muted_ = false };
const auto result = dispatcher.execute("broadcast Hello, World!", source);
if (!result) {
    // Parsing or a constraint failed.
    const auto& error = result.error();
}
```

## Arguments

Define a custom parser by specializing `parser` for its underlying type:
```cpp
template <>
struct brigadier::parser<custom_type> {
	using parse_result = std::expected<custom_type, brigadier::parse_error>;
	[[nodiscard]] static constexpr parse_result parse(brigadier::string_reader& reader);
};
```

Use it by passing the underlying type to `argument`:

```cpp
argument<"value", custom_type>()
```

### Suggestions

Pass the text entered so far to the dispatcher:

```cpp
auto suggestions = dispatcher.suggest("whis", source);
```

Argument nodes get their suggestions from `suggestion_provider<T>`. Specialize it for a custom type when it has known values:

```cpp
template <>
struct brigadier::suggestion_provider<custom_type> {
	template <typename Source>
	static void suggest(const Source& source, brigadier::suggestions_builder& builder) {
		// Call builder.suggest(value) for matching values.
	}
};
```

## Building

Build this repository and run its tests with:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Installing

Install the header-only library and its CMake package with:

```sh
cmake -S . -B build -DBRIGADIER_BUILD_TESTS=OFF
cmake --install build --prefix <install-prefix>
```

Consume an installed package with:

```cmake
find_package(brigadier 0.1 CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE brigadier::brigadier)
```
