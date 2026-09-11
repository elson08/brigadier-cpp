#pragma once

#include <brigadier/argument/parse/parser.hpp>
#include <brigadier/command.hpp>
#include <string>
#include <utility>

namespace brigadier {
	enum class advance_state {
		no_match,
		rejected,
		executed,
	};

	struct advance_result {
		advance_state state_{};
		parse_error error_{};
		execution_result execution_{ execution_result::success };

		[[nodiscard]] constexpr bool committed() const noexcept {
			return this->state_ == advance_state::executed;
		}

		constexpr bool consider(const advance_result& candidate) {
			if (candidate.committed()) {
				*this = candidate;
				return true;
			}

			if (candidate.state_ == advance_state::rejected && (this->state_ == advance_state::no_match || candidate.error_.cursor_ > this->error_.cursor_)) {
				*this = candidate;
			}

			return false;
		}

		[[nodiscard]] static constexpr advance_result no_match(const size_t cursor = 0) noexcept {
			return {
				.state_ = advance_state::no_match,
				.error_ = { .cursor_ = cursor }
			};
		}

		[[nodiscard]] static constexpr advance_result rejected(const size_t cursor, std::string message) noexcept {
			return {
				.state_ = advance_state::rejected,
				.error_ = { .cursor_ = cursor, .message_ = std::move(message) },
			};
		}

		[[nodiscard]] static constexpr advance_result rejected(parse_error error) noexcept {
			return {
				.state_ = advance_state::rejected,
				.error_ = std::move(error)
			};
		}

		[[nodiscard]] static constexpr advance_result executed(const execution_result result) noexcept {
			return {
				.state_ = advance_state::executed,
				.execution_ = result,
			};
		}
	};
}
