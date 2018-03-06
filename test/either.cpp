#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <portable_concurrency/bits/alias_namespace.h>
#include <portable_concurrency/bits/either.h>

namespace portable_concurrency {
namespace test {
namespace {

static_assert(
  !std::is_copy_constructible<detail::either<detail::monostate, int, std::string>>::value,
  "either should not be copyable"
);
static_assert(
  !std::is_copy_assignable<detail::either<detail::monostate, int, std::string>>::value,
  "either should not be copyable"
);

TEST(Either, default_constructed_is_empty) {
  detail::either<detail::monostate, int, std::string> either;
  EXPECT_TRUE(either.empty());
}

TEST(Either, first_value_constructed_has_correct_state) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<1>{}, 42};
  EXPECT_EQ(either.state(), 1u);
}

TEST(Either, second_value_constructed_is_not_empty) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<2>{}, "42"};
  EXPECT_EQ(either.state(), 2u);
}

TEST(Either, moved_from_is_empty) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<2>{}, "42"};
  detail::either<detail::monostate, int, std::string> either2 = std::move(either);
  EXPECT_TRUE(either.empty());
}

TEST(Either, moved_to_same_state_is_first_for_first_src) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<1>{}, 42};
  detail::either<detail::monostate, int, std::string> either2 = std::move(either);
  EXPECT_EQ(either2.state(), 1u);
}

TEST(Either, moved_to_state_is_second_for_second_src) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<2>{}, "42"};
  detail::either<detail::monostate, int, std::string> either2 = std::move(either);
  EXPECT_EQ(either2.state(), 2u);
}

TEST(Either, moved_to_is_empty_for_empty_src) {
  detail::either<detail::monostate, int, std::string> either;
  detail::either<detail::monostate, int, std::string> either2 = std::move(either);
  EXPECT_TRUE(either2.empty());
}

TEST(Either, empty_then_emplaced_with_first_is_first) {
  detail::either<detail::monostate, int, std::string> either;
  either.emplace(detail::in_place_index_t<1>{}, 42);
  EXPECT_EQ(either.state(), 1u);
}

TEST(Either, empty_then_emplaced_with_second_is_second) {
  detail::either<detail::monostate, int, std::string> either;
  either.emplace(detail::in_place_index_t<1>{}, 42);
  EXPECT_EQ(either.state(), 1u);
}

TEST(Either, switched_from_first_to_second_is_second) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<1>{}, 324};
  either.emplace(detail::in_place_index_t<2>{}, "42");
  EXPECT_EQ(either.state(), 2u);
}

TEST(Either, switched_from_secons_to_first_is_first) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<2>{}, "324"};
  either.emplace(detail::in_place_index_t<1>{}, 42);
  EXPECT_EQ(either.state(), 1u);
}

TEST(Either, get_value_on_cunstructed_as_first) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<1>{}, 42};
  EXPECT_EQ(either.get(detail::in_place_index_t<1>{}), 42);
}

TEST(Either, get_value_on_cunstructed_as_second) {
  detail::either<detail::monostate, int, std::string> either{detail::in_place_index_t<2>{}, "help"};
  EXPECT_EQ(either.get(detail::in_place_index_t<2>{}), "help");
}

TEST(Either, get_value_on_emplaced_with_first) {
  detail::either<detail::monostate, int, std::string> either;
  either.emplace(detail::in_place_index_t<1>{}, 42);
  EXPECT_EQ(either.get(detail::in_place_index_t<1>{}), 42);
}

TEST(Either, get_first_value_after_move) {
  detail::either<detail::monostate, int, std::string> either_old{detail::in_place_index_t<1>{}, 42};
  detail::either<detail::monostate, int, std::string> either_new = std::move(either_old);
  EXPECT_EQ(either_new.get(detail::in_place_index_t<1>{}), 42);
}

TEST(Either, get_second_value_after_move) {
  detail::either<detail::monostate, int, std::string> either_old{detail::in_place_index_t<2>{}, "voodo"};
  detail::either<detail::monostate, int, std::string> either_new = std::move(either_old);
  EXPECT_EQ(either_new.get(detail::in_place_index_t<2>{}), "voodo");
}

struct EitherCleanup: ::testing::Test {
  using sptr_t = std::shared_ptr<int>;
  std::shared_ptr<int> sval = std::make_shared<int>(100500);
  std::weak_ptr<int> wval = sval;
};

TEST_F(EitherCleanup, switch_from_first_to_second_destroys_old_value) {
  detail::either<detail::monostate, sptr_t, std::string> either{detail::in_place_index_t<1>{}, std::move(sval)};
  either.emplace(detail::in_place_index_t<2>{}, "Hello");
  EXPECT_TRUE(wval.expired());
}

TEST_F(EitherCleanup, switch_from_second_to_first_destroys_old_value) {
  detail::either<detail::monostate, std::string, sptr_t> either{detail::in_place_index_t<2>{}, std::move(sval)};
  either.emplace(detail::in_place_index_t<1>{}, "Hello");
  EXPECT_TRUE(wval.expired());
}

TEST_F(EitherCleanup, destructor_destroys_stored_first) {
  {
    detail::either<detail::monostate, sptr_t, std::string> either{detail::in_place_index_t<1>{}, std::move(sval)};
  }
  EXPECT_TRUE(wval.expired());
}

TEST_F(EitherCleanup, destructor_destroys_stored_second) {
  {
    detail::either<detail::monostate, std::string, sptr_t> either{detail::in_place_index_t<2>{}, std::move(sval)};
  }
  EXPECT_TRUE(wval.expired());
}

} // anonymous namespace
} // namespace test
} // namespace portable_concurrency
