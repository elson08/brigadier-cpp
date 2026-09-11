#pragma once

#include <cassert>
#include <cstddef>
#include <string_view>

namespace brigadier {
	class string_reader {
	public:
		explicit constexpr string_reader(const std::string_view input) noexcept
			: input_(input) { }

		[[nodiscard]] constexpr std::string_view input() const noexcept {
			return this->input_;
		}

		[[nodiscard]] constexpr size_t cursor() const noexcept {
			return this->cursor_;
		}

		constexpr void set_cursor(const size_t cursor) noexcept {
			assert(cursor <= this->input_.size());
			this->cursor_ = cursor;
		}

		[[nodiscard]] constexpr size_t remaining_length() const noexcept {
			return this->input_.size() - this->cursor_;
		}

		[[nodiscard]] constexpr std::string_view read_text() const noexcept {
			return this->input_.substr(0, this->cursor_);
		}

		[[nodiscard]] constexpr std::string_view remaining() const noexcept {
			return this->input_.substr(this->cursor_);
		}

		[[nodiscard]] constexpr bool can_read(const size_t length = 1) const noexcept {
			return length <= this->remaining_length();
		}

		[[nodiscard]] constexpr char peek(const size_t offset = 0) const noexcept {
			assert(offset < this->remaining_length());
			return this->input_[this->cursor_ + offset];
		}

		[[nodiscard]] constexpr std::string_view peek_word() const noexcept {
			auto length = size_t{};
			while (length < this->remaining_length() && !is_whitespace(this->peek(length))) {
				++length;
			}
			return this->remaining().substr(0, length);
		}

		[[nodiscard]] constexpr char read() noexcept {
			assert(this->can_read());
			return this->input_[this->cursor_++];
		}

		[[nodiscard]] constexpr std::string_view read_word() noexcept {
			const auto start = this->cursor_;
			this->read_while([](const char character) {
				return !is_whitespace(character);
			});
			return this->input_.substr(start, this->cursor_ - start);
		}

		constexpr void skip() noexcept {
			assert(this->can_read());
			++this->cursor_;
		}

		template <typename Predicate>
		constexpr void read_while(Predicate predicate) {
			while (this->can_read() && predicate(this->peek())) {
				this->skip();
			}
		}

		constexpr void skip_whitespace() noexcept {
			this->read_while(is_whitespace);
		}

		[[nodiscard]] static constexpr bool is_whitespace(const char character) noexcept {
			return character == ' ' || character == '\t' || character == '\n' || character == '\r' || character == '\f' || character == '\v';
		}

	private:
		std::string_view input_;
		size_t cursor_{};
	};
}
