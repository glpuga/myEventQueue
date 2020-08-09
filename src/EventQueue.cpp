/**
 * Copyright 2020, Gerardo Puga
 * Simple event queue
 * */

// Standard library headers
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>

// Project headers
#include <event_queue/EventQueue.hpp>

namespace glpuga {

namespace tools {

EventQueue::EventQueue() {
  forwarder_thread_ = std::thread([this]() { runforwarder(); });
}

EventQueue::~EventQueue() {
  halting_ = true;
  cv_.notify_all();
  forwarder_thread_.join();
}

void EventQueue::postEvent() {
  pending_event_ = true;
  cv_.notify_all();
}

EventQueue::CallbackHandler EventQueue::subscribeToEvent(const CallbackFunction &callback) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto handler = std::make_shared<CallbackFunction>(callback);
  subscribers_.emplace_back(std::weak_ptr<CallbackFunction>(handler));
  return handler;
}

void EventQueue::runforwarder() {
  std::unique_lock<std::mutex> lock{mutex_};
  while (!halting_) {
    cv_.wait_for(lock, max_sleep_interval_, [this]() { return static_cast<bool>(pending_event_); });
    if (pending_event_) {
      pending_event_ = false;
      triggerSubscribers();
    }
  }
}

void EventQueue::triggerSubscribers() {
  subscribers_.remove_if([](const EventQueue::WeakCallbackHandler &handler) { return handler.expired(); });

  auto caller = [](const EventQueue::WeakCallbackHandler &weak_handler) {
    auto handler = weak_handler.lock();
    if (handler) {
      (*handler)();
    }
  };

  std::for_each(subscribers_.begin(), subscribers_.end(), caller);
}

}  // namespace tools

}  // namespace glpuga
