#include <brigadier/brigadier.hpp>
#include <gtest/gtest.h>

using namespace brigadier;

struct Source {
	bool permitted{};
};

struct custom_value { };

struct contextual_value { };

template <>
struct brigadier::parser<custom_value> {
	[[nodiscard]] static constexpr std::expected<custom_value, parse_error> parse(string_reader&) noexcept {
		return custom_value{};
	}
};

template <>
struct brigadier::parser<contextual_value> {
	[[nodiscard]] static constexpr std::expected<contextual_value, parse_error> parse(string_reader& reader) {
		static_cast<void>(reader.read_word());
		return contextual_value{};
	}
};

template <>
struct brigadier::suggestion_provider<contextual_value> {
	template <typename T>
	static constexpr void suggest(const T&, suggestions_builder& builder, const int count) {
		if (count == 2) {
			builder.starts_with("two");
		}
	}
};

template <typename Parser>
[[nodiscard]] constexpr auto parse(const std::string_view input) {
	string_reader reader{ input };
	return Parser::parse(reader);
}

[[nodiscard]] static constexpr constraint_result require(const bool condition, std::string error) {
	return condition ? constraint_result{} : std::unexpected(std::move(error));
}

static constexpr auto make_broadcast_command() {
	return literal<"broadcast">()
		.must<[](const auto& source) -> constraint_result {
			return require(source.permitted, "Missing permission");
		}>()
		.then(
			argument<"message", greedy_text>()
				.executes<[](auto&, std::string_view) {
					return execution_result::success;
				}>()
		);
}

TEST(CommandDispatcher, ExecutesAndReportsConstraintErrors) {
	constexpr auto dispatcher = make_dispatcher<Source>(make_broadcast_command());

	Source source{
		.permitted = true
	};

	EXPECT_EQ(dispatcher.execute("broadcast hello guys", source), execution_result::success);

	source.permitted = false;
	const auto rejected = dispatcher.execute("broadcast hello", source);
	ASSERT_FALSE(rejected);
	EXPECT_EQ(rejected.error().message_, "Missing permission");
}

TEST(CommandDispatcher, BacktracksChildNodes) {
	constexpr auto dispatcher = make_dispatcher<Source>(
		literal<"mode">()
			.then(
				literal<"fast">()
			)
			.then(
				literal<"slow">()
					.executes<[](Source&) -> execution_result {
						return execution_result::success;
					}>()
			)
	);

	Source source{};
	EXPECT_EQ(dispatcher.execute("mode slow", source), execution_result::success);
}

TEST(IntegralParser, ParsesAndValidatesInput) {
	EXPECT_EQ(parse<parser<int>>("-5"), -5);
	EXPECT_FALSE(parse<parser<int>>("12x"));

	const auto negative = parse<argument_type<int, positive<int>>>("-5");
	ASSERT_FALSE(negative);
	EXPECT_EQ(negative.error().message_, "Expected a positive value");

	const auto outside = parse<argument_type<int, bounds<int, 1, 10>>>("11");
	ASSERT_FALSE(outside);
	EXPECT_EQ(outside.error().message_, "Expected a value between 1 and 10");
}

TEST(FloatingPointParser, ParsesAndValidatesInput) {
	EXPECT_DOUBLE_EQ(parse<parser<double>>("3.5").value(), 3.5);
	EXPECT_DOUBLE_EQ(parse<parser<double>>("-1.25e2").value(), -125.0);

	const auto malformed = parse<parser<double>>("1.2x");
	ASSERT_FALSE(malformed);
	EXPECT_EQ(malformed.error().message_, "Expected a floating point number");

	EXPECT_DOUBLE_EQ((parse<argument_type<double, bounds<double, 0.0, 1.0>>>("0.5").value()), 0.5);
	const auto outside = parse<argument_type<double, bounds<double, 0.0, 1.0>>>("1.5");
	ASSERT_FALSE(outside);
	EXPECT_EQ(outside.error().message_, "Expected a value between 0 and 1");
}

TEST(FloatingPointConstraint, LimitsDecimalPlaces) {
	EXPECT_DOUBLE_EQ((parse<argument_type<double, decimal_places<double, 2>>>("1.23").value()), 1.23);
	const auto rejected = parse<argument_type<double, decimal_places<double, 2>>>("1.234");
	ASSERT_FALSE(rejected);
	EXPECT_EQ(rejected.error().message_, "Expected at most 2 decimal places");
	EXPECT_TRUE((decimal_places<double, 2>::validate(123456789.12)));
	EXPECT_FALSE((decimal_places<double, 2>::validate(std::numeric_limits<double>::infinity())));
	EXPECT_FALSE((decimal_places<double, 2>::validate(std::numeric_limits<double>::quiet_NaN())));
}

TEST(TextParsers, ParseWordAndGreedyText) {
	string_reader word_reader{ "hello world" };
	string_reader text_reader{ "hello world" };

	EXPECT_EQ(parser<std::string_view>::parse(word_reader), "hello");
	EXPECT_EQ(word_reader.remaining(), " world");
	EXPECT_EQ(parser<greedy_text>::parse(text_reader), "hello world");
	EXPECT_FALSE(text_reader.can_read());
}

TEST(SuggestionsBuilder, FiltersSuggestions) {
	suggestions_builder builder{ "set tr" };
	builder.reader().set_cursor(4);
	builder.starts_with("true"_fs).starts_with("false").containing("patrol"_fs);

	EXPECT_EQ(builder.input(), "set tr");
	EXPECT_EQ(builder.start(), 4);
	EXPECT_EQ(builder.remaining(), "tr");
	EXPECT_EQ(builder.build(), (suggestions{ "true", "patrol" }));
}

TEST(CommandDispatcher, SuggestsLiteralsAndParserValues) {
	constexpr auto booleans = make_dispatcher<Source>(
		literal<"set">()
			.then(
				argument<"enabled", bool>()
			)
	);

	constexpr auto literals = make_dispatcher<Source>(
		literal<"start">(),
		literal<"stop">()
	);

	Source source{};

	EXPECT_EQ(booleans.suggest("set t", source), (suggestions{ "true", "t" }));
	EXPECT_EQ(literals.suggest("st", source), (suggestions{ "start", "stop" }));
	EXPECT_EQ(literals.suggest("start", source), suggestions{ "start" });
}

TEST(CommandDispatcher, AppliesConstraintsToSuggestions) {
	constexpr auto dispatcher = make_dispatcher<Source>(
		literal<"set">()
			.then(
				argument<"enabled", bool>()
					.must<[](const auto&, const bool enabled) {
						return require(!enabled, "Not allowed");
					}>()
			)
	);

	Source source{};

	EXPECT_TRUE(dispatcher.suggest("set t", source).empty());
	EXPECT_EQ(dispatcher.suggest("set f", source), (suggestions{ "false", "f" }));
}

TEST(CommandDispatcher, PassesParsedArgumentsToSuggestions) {
	constexpr auto contextual = make_dispatcher<Source>(
		literal<"pick">()
			.then(
				argument<"count", int>()
					.then(
						argument<"value", contextual_value>()
					)
			)
	);

	constexpr auto constrained = make_dispatcher<Source>(
		literal<"apply">()
			.then(
				argument<"count", int>()
					.then(
						literal<"allowed">()
							.must<[](const auto&, const int count) {
								return require(count == 2, "Wrong count");
							}>()
					)
			)
	);

	Source source{};

	EXPECT_EQ(contextual.suggest("pick 2 t", source), suggestions{ "two" });
	EXPECT_EQ(constrained.suggest("apply 2 a", source), suggestions{ "allowed" });
	EXPECT_TRUE(constrained.suggest("apply 3 a", source).empty());
}
