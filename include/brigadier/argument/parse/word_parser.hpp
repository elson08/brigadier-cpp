#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/string_reader.hpp>

#include <string_view>

namespace brigadier {
	template <>
	struct parser<std::string_view> {
		[[nodiscard]] static constexpr std::expected<std::string_view, parse_error> parse(string_reader& reader) {
			const auto start = reader.cursor();
			const auto value = reader.read_word();
			if (value.empty()) {
				return std::unexpected(parse_error{
					.cursor_ = start,
					.message_ = "Expected a word",
				});
			}

			return value;
		}
	};
}
