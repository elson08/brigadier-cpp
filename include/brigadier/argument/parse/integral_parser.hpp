#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/string_reader.hpp>

#include <charconv>
#include <concepts>
#include <type_traits>

namespace brigadier {
	template <std::integral Type>
	requires(!std::same_as<std::remove_cv_t<Type>, bool>)
	struct parser<Type> {
		[[nodiscard]] static constexpr std::expected<Type, parse_error> parse(string_reader& reader) {
			const auto start = reader.cursor();
			const auto token = reader.read_word();

			Type value{};
			const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), value);
			if (error != std::errc{} || end != token.data() + token.size()) {
				return std::unexpected(parse_error{
					.cursor_ = start,
					.message_ = "Expected an integer",
				});
			}

			return value;
		}
	};
}
