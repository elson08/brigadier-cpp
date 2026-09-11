#pragma once

#include <algorithm>
#include <brigadier/constraint.hpp>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <format>
#include <limits>

namespace brigadier {
	template <std::floating_point T, std::size_t Places>
	struct decimal_places {
		static constexpr T scale_ = [] {
			T scale = 1;
			for (std::size_t i = 0; i < Places; ++i) {
				scale *= 10;
			}
			return scale;
		}();

		[[nodiscard]] static constexpr constraint_result validate(const T value) {
			const auto scaled = value * scale_;
			const auto tolerance = std::numeric_limits<T>::epsilon() * std::max(T{ 1 }, std::abs(scaled)) * T{ 4 };
			if (!std::isfinite(value) || !std::isfinite(scaled) || std::abs(scaled - std::round(scaled)) > tolerance) {
				return std::unexpected(std::format("Expected at most {} decimal places", Places));
			}

			return {};
		}
	};
}
