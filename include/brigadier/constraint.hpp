#pragma once

#include <concepts>
#include <expected>
#include <functional>
#include <string>
#include <utility>

namespace brigadier {
	using constraint_result = std::expected<void, std::string>;

	template <auto Executable>
	class constraint {
	public:
		template <typename... Args>
		requires std::invocable<decltype(Executable), Args...>
		[[nodiscard]] static constexpr constraint_result validate(Args&&... args) {
			return std::invoke(Executable, std::forward<Args>(args)...);
		}
	};

	using no_constraint = constraint<[]<typename... Args>(Args&&...) -> constraint_result {
		return {};
	}>;
}
