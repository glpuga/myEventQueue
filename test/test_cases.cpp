/**
 * Copyright 2020, Gerardo Puga
 * Simple event queue
 * */

// gtest and gmock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Standard library
#include <chrono>
#include <memory>
#include <thread>

// Project
#include <event_queue/EventQueue.hpp>

namespace glpuga {

namespace tools {

namespace test {

namespace {

using ::testing::Test;
using ::testing::InSequence;

class SubscriberInterface {
 public:
  SubscriberInterface() = default;

  SubscriberInterface(const SubscriberInterface&) = delete;
  SubscriberInterface& operator=(const SubscriberInterface&) = delete;
  SubscriberInterface(SubscriberInterface&&) = delete;
  SubscriberInterface& operator=(SubscriberInterface&&) = delete;

  virtual ~SubscriberInterface() = default;
  virtual void callback() = 0;
};

class SubscriberMock : public SubscriberInterface {
 public:
  MOCK_METHOD0(callback, void());
};

class QueueEventTest : public Test {
 public:
  const std::chrono::milliseconds wait_interval_{500};
};

TEST_F(QueueEventTest, ConstructionTest) {
  // Test creation and destruction work as expected, even when
  // done in quick succession.
  std::unique_ptr<EventQueue> uut;
  uut = std::make_unique<EventQueue>();
  uut = nullptr;
}

TEST_F(QueueEventTest, TestsThatAnEventsCanBePosted) {
  // Test that uut does not crash when posting events
  EventQueue uut;
  std::this_thread::sleep_for(wait_interval_);
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);
}

TEST_F(QueueEventTest, TestsThatWeCanSubscribeToEvent) {
  // Test that subscribing returns a valid handle
  EventQueue uut;
  SubscriberMock subscriber_1;
  SubscriberMock subscriber_2;

  // we won't post an event, so no callback should be called
  EXPECT_CALL(subscriber_1, callback()).Times(0);
  EXPECT_CALL(subscriber_2, callback()).Times(0);

  // subscribe clients to the event
  auto handle_1 = uut.subscribeToEvent([&subscriber_1]() { subscriber_1.callback(); });
  auto handle_2 = uut.subscribeToEvent([&subscriber_2]() { subscriber_2.callback(); });

  // we should have got two valid handles, one for each subscriber
  ASSERT_TRUE(handle_1 != nullptr);
  ASSERT_TRUE(handle_2 != nullptr);
  ASSERT_TRUE(handle_1 != handle_2);
}

TEST_F(QueueEventTest, TestsPostedEventsGetReceived) {
  // Test that an event posted gets sent to all subscribers
  EventQueue uut;
  SubscriberMock subscriber_1;
  SubscriberMock subscriber_2;

  EXPECT_CALL(subscriber_1, callback()).Times(1);
  EXPECT_CALL(subscriber_2, callback()).Times(1);

  auto handle_1 = uut.subscribeToEvent([&subscriber_1]() { subscriber_1.callback(); });
  auto handle_2 = uut.subscribeToEvent([&subscriber_2]() { subscriber_2.callback(); });

  uut.postEvent();

  // Wait some time to let the forwarder thread wake up and do its job
  std::this_thread::sleep_for(wait_interval_);
}

TEST_F(QueueEventTest, TestsClientsCanHopOnAndHopOffAtAnyTime) {
  // Tests that posted events only get sent to client that have not destroyed
  // their handles
  EventQueue uut;
  SubscriberMock subscriber_1;
  SubscriberMock subscriber_2;
  SubscriberMock subscriber_3;
  SubscriberMock subscriber_4;

  InSequence aux;
  // Here an event gets posted
  EXPECT_CALL(subscriber_1, callback()).Times(1);
  // Here an event gets posted
  EXPECT_CALL(subscriber_1, callback()).Times(1);
  EXPECT_CALL(subscriber_2, callback()).Times(1);
  // Here an event gets posted
  EXPECT_CALL(subscriber_1, callback()).Times(1);
  EXPECT_CALL(subscriber_2, callback()).Times(1);
  EXPECT_CALL(subscriber_3, callback()).Times(1);
  // Here an event gets posted
  EXPECT_CALL(subscriber_1, callback()).Times(1);
  EXPECT_CALL(subscriber_2, callback()).Times(1);
  EXPECT_CALL(subscriber_4, callback()).Times(1);
  // Here an event gets posted
  EXPECT_CALL(subscriber_1, callback()).Times(1);
  EXPECT_CALL(subscriber_4, callback()).Times(1);
  // Here an event gets posted

  // Create the first subcriber
  auto handle_1 = uut.subscribeToEvent([&subscriber_1]() { subscriber_1.callback(); });
  // Post first event
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);

  // subscribe the second client
  auto handle_2 = uut.subscribeToEvent([&subscriber_2]() { subscriber_2.callback(); });
  // Post second event
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);

  // subscribe the third client
  auto handle_3 = uut.subscribeToEvent([&subscriber_3]() { subscriber_3.callback(); });
  // Post third event
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);

  // unsubscribe the third client, create the fourth
  handle_3 = nullptr;
  auto handle_4 = uut.subscribeToEvent([&subscriber_4]() { subscriber_4.callback(); });
  // Post fourth event
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);

  // unsubscribe the second client
  handle_2 = nullptr;
  // Post fifth event
  uut.postEvent();
  std::this_thread::sleep_for(wait_interval_);

  // unsubscribe the fourth and first clients
  handle_4 = nullptr;
  handle_1 = nullptr;
  // Post sixth event
  uut.postEvent();
  // Wait some time to let the forwarder thread wake up and do its job
  std::this_thread::sleep_for(wait_interval_);
}

}  // namespace

}  // namespace test

}  // namespace tools

}  // namespace glpuga
