#pragma once

#include <brigadier/argument/suggestions_builder.hpp>
#include <brigadier/tree/command_node.hpp>
#include <utility>

namespace brigadier {
	template <fixed_string Name, typename Command, typename Constraint, typename... Children>
	class literal_node : public command_node<literal_node<Name, Command, Constraint, Children...>, Name, Command, Constraint, Children...> {
	public:
		template <typename NewCommand, typename NewConstraint, typename... NewChildren>
		using rebind = literal_node<Name, NewCommand, NewConstraint, NewChildren...>;
		using base_type = command_node<literal_node, Name, Command, Constraint, Children...>;

		static constexpr auto usage_string = Name;

		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr bool suggest(Source& source, suggestions_builder& builder, Args&&... args) {
			if (!base_type::available(source, args...)) {
				return false;
			}

			auto& reader = builder.reader();
			reader.skip_whitespace();
			if (!reader.can_read()) {
				builder.suggest(Name);
				return false;
			}

			const auto value = reader.peek_word();
			if (value != Name.view()) {
				builder.starts_with(Name);
				return false;
			}

			static_cast<void>(reader.read_word());
			if (reader.can_read()) {
				static_cast<void>((Children::suggest(source, builder, args...) || ...));
				return true;
			}

			builder.suggest(Name);
			return true;
		}

		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr advance_result advance(string_reader reader, Source& source, Args&&... args) {
			reader.skip_whitespace();
			const auto start = reader.cursor();
			if (reader.read_word() != Name.view()) {
				return advance_result::no_match(start);
			}

			return base_type::continue_parsing(reader, source, std::forward<Args>(args)...);
		}
	};

	template <fixed_string Name>
	[[nodiscard]] constexpr auto literal() noexcept {
		return literal_node<Name, no_command, no_constraint>{};
	}
}
