#pragma once

#include <brigadier/command.hpp>
#include <brigadier/constraint.hpp>
#include <brigadier/fixed_string.hpp>
#include <brigadier/string_reader.hpp>
#include <brigadier/tree/advance_result.hpp>
#include <type_traits>
#include <utility>

namespace brigadier {
	template <typename Derived, fixed_string Name, typename Command, typename Constraint, typename... Children>
	class command_node {
	public:
		static constexpr auto name = Name;
		using command_type = Command;
		using requirement_type = Constraint;

		template <auto Requirement>
		[[nodiscard]] constexpr auto must() const noexcept {
			return typename Derived::template rebind<Command, constraint<Requirement>, Children...>{};
		}

		template <typename Requirement>
		[[nodiscard]] constexpr auto must() const noexcept {
			return typename Derived::template rebind<Command, constraint<Requirement{}>, Children...>{};
		}

		template <auto Executable>
		[[nodiscard]] constexpr auto executes() const noexcept {
			return typename Derived::template rebind<command<Executable>, Constraint, Children...>{};
		}

		template <typename Executable>
		[[nodiscard]] constexpr auto executes() const noexcept {
			return typename Derived::template rebind<command<Executable{}>, Constraint, Children...>{};
		}

		template <typename Child>
		[[nodiscard]] constexpr auto then(Child&&) const noexcept {
			return typename Derived::template rebind<Command, Constraint, Children..., std::remove_cvref_t<Child>>{};
		}

		[[nodiscard]] static constexpr size_t children() noexcept {
			return sizeof...(Children);
		}

		[[nodiscard]] static constexpr bool has_children() noexcept {
			return sizeof...(Children) > 0;
		}

	protected:
		template <typename... Args>
		[[nodiscard]] static constexpr std::expected<void, std::string> available(Args&&... args) {
			return Constraint::validate(std::forward<Args>(args)...);
		}

		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr advance_result continue_parsing(string_reader& reader, Source& source, Args&&... args) {
			if (auto result = available(source, args...); !result) {
				return advance_result::rejected(reader.cursor(), result.error());
			}

			reader.skip_whitespace();
			if (!reader.can_read()) {
				if constexpr (is_executable<Command>) {
					return advance_result::executed(Command::run(source, std::forward<Args>(args)...));
				} else {
					return advance_result::rejected(reader.cursor(), "Incomplete command");
				}
			}

			return advance_children(reader, source, std::forward<Args>(args)...);
		}

	private:
		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr advance_result advance_children(string_reader& reader, Source& source, Args&&... args) {
			if constexpr (!has_children()) {
				return advance_result::rejected(reader.cursor(), "Unexpected argument");
			} else {
				auto best = advance_result::no_match(reader.cursor());
				if ((best.consider(Children::advance(reader, source, std::forward<Args>(args)...)) || ...)) {
					return best;
				}

				if (best.state_ == advance_state::no_match) {
					return advance_result::rejected(reader.cursor(), "Unexpected argument");
				}

				return best;
			}
		}
	};
}
