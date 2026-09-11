#pragma once

#include <brigadier/fixed_string.hpp>
#include <brigadier/string_reader.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace brigadier {
	using suggestions = std::vector<std::string>;

	class suggestions_builder {
	public:
		explicit constexpr suggestions_builder(const std::string_view input) noexcept
			: reader_(input) { }

		[[nodiscard]] constexpr std::string_view input() const noexcept {
			return this->reader_.input();
		}

		[[nodiscard]] constexpr size_t start() const noexcept {
			return this->reader_.cursor();
		}

		[[nodiscard]] constexpr std::string_view remaining() const noexcept {
			return this->reader_.remaining();
		}

		[[nodiscard]] constexpr string_reader& reader() noexcept {
			return this->reader_;
		}

		[[nodiscard]] constexpr const string_reader& reader() const noexcept {
			return this->reader_;
		}

		constexpr suggestions_builder& suggest(const std::string_view value) {
			this->suggestions_.emplace_back(value);
			return *this;
		}

		template <size_t Size>
		constexpr suggestions_builder& suggest(const fixed_string<Size>& value) {
			return this->suggest(value.view());
		}

		constexpr suggestions_builder& starts_with(const std::string_view value) {
			if (value.starts_with(this->reader_.peek_word())) {
				this->suggest(value);
			}
			return *this;
		}

		template <size_t Size>
		constexpr suggestions_builder& starts_with(const fixed_string<Size>& value) {
			return this->starts_with(value.view());
		}

		constexpr suggestions_builder& containing(const std::string_view value) {
			if (value.contains(this->reader_.peek_word())) {
				this->suggest(value);
			}
			return *this;
		}

		template <size_t Size>
		constexpr suggestions_builder& containing(const fixed_string<Size>& value) {
			return this->containing(value.view());
		}

		[[nodiscard]] constexpr suggestions build() noexcept {
			return std::move(this->suggestions_);
		}

	private:
		string_reader reader_;
		suggestions suggestions_;
	};
}
