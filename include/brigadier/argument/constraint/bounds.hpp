#pragma once

#include <expected>
#include <format>
#include <string>

namespace brigadier {
	template <typename T, T Min, T Max>
	struct bounds {
		[[nodiscard]] static constexpr std::expected<void, std::string> validate(const T& value) {
			if (value >= Min && value <= Max) {
				return {};
			}

			return std::unexpected(std::format("Expected a value between {} and {}", Min, Max));
		}
	};
}
