#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace brigadier {
	template <size_t N>
	struct fixed_string {
		char value_[N + 1]{};

		constexpr fixed_string() noexcept = default;

		constexpr fixed_string(const char (&input)[N + 1]) noexcept {
			std::copy_n(input, N, this->value_);
		}

		[[nodiscard]] constexpr const char* data() const noexcept {
			return this->value_;
		}

		[[nodiscard]] constexpr const char* c_str() const noexcept {
			return this->value_;
		}

		[[nodiscard]] constexpr size_t size() const noexcept {
			return N;
		}

		[[nodiscard]] constexpr bool empty() const noexcept {
			return this->size() == 0;
		}

		[[nodiscard]] constexpr char operator[](const size_t index) const noexcept {
			return this->value_[index];
		}

		[[nodiscard]] constexpr std::string_view view() const noexcept {
			return { this->data(), this->size() };
		}

		[[nodiscard]] constexpr operator std::string_view() const noexcept {
			return this->view();
		}

		template <size_t M>
		[[nodiscard]] constexpr bool operator==(const fixed_string<M>& other) const noexcept {
			return this->view() == other.view();
		}

		template <size_t M>
		[[nodiscard]] constexpr auto operator+(const fixed_string<M>& other) const noexcept {
			fixed_string<N + M> result;
			std::copy_n(this->value_, N, result.value_);
			std::copy_n(other.value_, M, result.value_ + N);
			return result;
		}
	};

	template <size_t N>
	fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

	template <fixed_string Value>
	[[nodiscard]] consteval auto operator""_fs() noexcept {
		return Value;
	}
}
