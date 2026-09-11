#pragma once

#include <brigadier/constraint.hpp>

namespace brigadier {
	template <typename T>
	struct positive {
		[[nodiscard]] static constexpr constraint_result validate(const T& value) {
			if (value > T{}) {
				return {};
			}

			return std::unexpected("Expected a positive value");
		}
	};
}
