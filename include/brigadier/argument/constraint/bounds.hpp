#pragma once

#include <brigadier/constraint.hpp>
#include <format>

namespace brigadier {
	template <typename T, T Min, T Max>
	struct bounds {
		[[nodiscard]] static constexpr constraint_result validate(const T& value) {
			if (value >= Min && value <= Max) {
				return {};
			}

			return std::unexpected(std::format("Expected a value between {} and {}", Min, Max));
		}
	};
}
