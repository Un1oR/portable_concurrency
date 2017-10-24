#include <gtest/gtest.h>

#include <portable_concurrency/future>

namespace portable_concurrency {
namespace test {
namespace {

TEST(PromiseCancelation, no_object_held_by_promise_after_future_destruction) {
  auto val = std::make_shared<int>(42);
  std::weak_ptr<int> weak = val;
  pc::promise<std::shared_ptr<int>> promise;
  promise.set_value(std::move(val));
  promise.get_future();
  EXPECT_FALSE(weak.lock());
}

TEST(PromiseCanceleation, set_value_do_nothing_on_canceled_promise) {
  pc::promise<int> promise;
  promise.get_future();
  EXPECT_NO_THROW(promise.set_value(42));
}

TEST(PromiseCanceleation, set_exception_do_nothing_on_canceled_promise) {
  pc::promise<int> promise;
  promise.get_future();
  EXPECT_NO_THROW(promise.set_exception(std::make_exception_ptr(std::runtime_error{"Ooups"})));
}

TEST(PackagedTaskCancelation, function_object_is_destroyed_on_cancel) {
  auto val = std::make_shared<int>(42);
  std::weak_ptr<int> weak = val;
  pc::packaged_task<void()> task{[val = std::move(val)] {}};
  task.get_future();
  EXPECT_FALSE(weak.lock());
}

TEST(PackagedTaskCancelation, canceled_task_remain_valid) {
  pc::packaged_task<void()> task{[] {}};
  task.get_future();
  EXPECT_TRUE(task.valid());
}

TEST(PackagedTaskCancelation, run_canceled_task_do_not_throw) {
  pc::packaged_task<void()> task{[] {}};
  task.get_future();
  EXPECT_NO_THROW(task());
}

TEST(PackagedTaskCancelation, run_canceled_task_do_not_execute_stored_function) {
  bool executed = false;
  pc::packaged_task<void()> task{[&executed] {executed = true;}};
  task.get_future();
  task();
  EXPECT_FALSE(executed);
}

} // namespace
} // namespace test
} // namespace portable_concurrency
