#pragma once

#include <brigadier/execution_result.hpp>
#include <concepts>
#include <functional>
#include <utility>

namespace brigadier {
	struct no_command { };

	template <typename Type>
	concept is_executable = !std::same_as<Type, no_command>;

	template <typename Fn, typename S, typename... Args>
	concept valid_command =
		std::invocable<Fn, S&, Args...> &&
		std::same_as<std::invoke_result_t<Fn, S&, Args...>, execution_result>;

	template <auto E>
	class command {
	public:
		template <typename Source, typename... Args>
		requires valid_command<decltype(E), Source, Args&&...>
		static constexpr execution_result run(Source& source, Args&&... args) {
			return std::invoke(E, source, std::forward<Args>(args)...);
		}
	};
}
