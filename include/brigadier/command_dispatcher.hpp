#pragma once

#include <brigadier/argument/suggestion_provider.hpp>
#include <brigadier/string_reader.hpp>
#include <brigadier/tree/advance_result.hpp>
#include <cassert>
#include <expected>
#include <string_view>
#include <type_traits>

namespace brigadier {
	template <typename Source, typename... Roots>
	requires(sizeof...(Roots) > 0)
	class command_dispatcher {
	public:
		using source_type = Source;
		using dispatch_result = std::expected<execution_result, parse_error>;

		[[nodiscard]] constexpr dispatch_result execute(const std::string_view input, Source& source) const {
			string_reader reader{ input };

			auto best = advance_result::no_match();
			if ((best.consider(Roots::advance(reader, source)) || ...)) {
				return best.execution_;
			}

			if (best.state_ == advance_state::no_match) {
				return std::unexpected(parse_error{ .message_ = "Unknown command" });
			}

			return std::unexpected(best.error_);
		}

		[[nodiscard]] constexpr suggestions suggest(const std::string_view input, Source& source) const {
			return this->suggest(input, input.size(), source);
		}

		[[nodiscard]] constexpr suggestions suggest(const std::string_view input, const size_t cursor, Source& source) const {
			assert(cursor <= input.size());

			suggestions_builder builder{ input.substr(0, cursor) };
			static_cast<void>((Roots::suggest(source, builder) || ...));
			return builder.build();
		}

		[[nodiscard]] static constexpr size_t size() noexcept {
			return sizeof...(Roots);
		}
	};

	template <typename Source, typename... Roots>
	[[nodiscard]] constexpr auto make_dispatcher(Roots&&...) noexcept {
		return command_dispatcher<Source, std::remove_cvref_t<Roots>...>{};
	}
}
