#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/constraint.hpp>
#include <utility>

namespace brigadier {
	template <typename T, typename... Constraints>
	class argument_type {
	public:
		using underlying_type = T;
		using parser_type = parser<underlying_type>;
		using parse_result = decltype(parser_type::parse(std::declval<string_reader&>()));
		using value_type = typename parse_result::value_type;

		[[nodiscard]] static constexpr parse_result parse(string_reader& reader) {
			const auto start = reader.cursor();
			auto parsed = parser_type::parse(reader);

			if (!parsed) {
				return parsed;
			}

			if constexpr (sizeof...(Constraints) > 0) {
				if (auto result = validate_constraints(parsed.value()); !result) {
					return std::unexpected(parse_error{
						.cursor_ = start,
						.message_ = result.error(),
					});
				}
			}

			return parsed;
		}

	private:
		static constexpr constraint_result validate_constraints(const value_type& value) {
			constraint_result result;
			static_cast<void>(((result = Constraints::validate(value), !result) || ...));
			return result;
		}
	};
}
