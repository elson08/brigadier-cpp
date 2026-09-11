#pragma once

#include <expected>
#include <string>

namespace brigadier {
	template <typename T>
	struct positive {
		[[nodiscard]] static constexpr std::expected<void, std::string> validate(const T& value) {
			if (value > T{}) {
				return {};
			}

			return std::unexpected("Expected a positive value");
		}
	};
}
