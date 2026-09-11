#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/argument/suggestion_provider.hpp>
#include <brigadier/string_reader.hpp>
#include <array>
#include <span>
#include <string_view>
#include <ranges>

namespace brigadier {
	inline constexpr auto true_values = std::to_array<std::string_view>({
		"true", "1", "on", "yes", "y", "t"
	});

	inline constexpr auto false_values = std::to_array<std::string_view>({
		"false", "0", "off", "no", "n", "f"
	});

	template <>
	struct parser<bool> {
		[[nodiscard]] static constexpr std::expected<bool, parse_error> parse(string_reader& reader) {
			const auto start = reader.cursor();
			const auto value = reader.read_word();

			for (const auto& val : true_values) {
				if (value == val) {
					return true;
				}
			}

			for (const auto& val : false_values) {
				if (value == val) {
					return false;
				}
			}

			return std::unexpected(parse_error{
				.cursor_ = start,
				.message_ = "Expected true or false",
			});
		}
	};

	template <>
	struct suggestion_provider<bool> {
		static constexpr auto options = std::array{
			std::span<const std::string_view>(true_values),
			std::span<const std::string_view>(false_values)
		} | std::views::join;

		template <typename Source, typename... Args>
		static constexpr void suggest(const Source&, suggestions_builder& builder, const Args&...) {
			for (const auto& option : options) {
				builder.starts_with(option);
			}
		}
	};
}
