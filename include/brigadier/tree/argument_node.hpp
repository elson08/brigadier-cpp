#pragma once

#include <brigadier/argument/argument.hpp>
#include <brigadier/argument/suggestion_provider.hpp>
#include <brigadier/tree/command_node.hpp>

#include <utility>

namespace brigadier {
	template <fixed_string Name, typename Argument, typename Command, typename Constraint, typename... Children>
	class argument_node : public command_node<argument_node<Name, Argument, Command, Constraint, Children...>, Name, Command, Constraint, Children...> {
	public:
		template <typename NewCommand, typename NewConstraint, typename... NewChildren>
		using rebind = argument_node<Name, Argument, NewCommand, NewConstraint, NewChildren...>;

		using base_type = command_node<argument_node, Name, Command, Constraint, Children...>;
		using argument_type = Argument;
		using underlying_type = typename argument_type::underlying_type;
		using parser_type = typename argument_type::parser_type;

		static constexpr auto usage_string = "<"_fs + Name + ">"_fs;

		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr bool suggest(Source& source, suggestions_builder& builder, Args&&... args) {
			auto& reader = builder.reader();
			reader.skip_whitespace();

			auto candidate_reader = reader;
			auto parsed = argument_type::parse(candidate_reader);
			if (parsed && !base_type::available(source, args..., *parsed)) {
				return false;
			}

			const auto single_word = reader.remaining().size() <= reader.peek_word().size();
			const auto extends_to_end = !parsed || !candidate_reader.can_read();
			if (single_word || (multi_word_suggestions<underlying_type> && extends_to_end)) {
				suggestion_provider<underlying_type>::suggest(source, builder, args...);
				return false;
			}

			if (!parsed) {
				return false;
			}

			reader = candidate_reader;
			static_cast<void>((Children::suggest(source, builder, args..., *parsed) || ...));
			return true;
		}

		template <typename Source, typename... Args>
		[[nodiscard]] static constexpr advance_result advance(string_reader reader, Source& source, Args&&... args) {
			reader.skip_whitespace();
			auto parsed = argument_type::parse(reader);
			if (!parsed) {
				return advance_result::rejected(parsed.error());
			}

			return base_type::continue_parsing(reader, source, std::forward<Args>(args)..., std::move(*parsed));
		}
	};

	template <fixed_string Name, typename Type, typename... Constraints>
	[[nodiscard]] constexpr auto argument() noexcept {
		return argument_node<Name, argument_type<Type, Constraints...>, no_command, no_constraint>{};
	}
}
