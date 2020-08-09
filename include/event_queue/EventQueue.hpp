/**
 * Copyright 2020, Gerardo Puga
 * Simple event queue
 * */
#pragma once

// Standard library headers
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

namespace glpuga {

namespace tools {

class EventQueue {
 public:
  using SharedPtr = std::shared_ptr<EventQueue>;
  using CallbackFunction = std::function<void()>;
  using CallbackHandler = std::shared_ptr<CallbackFunction>;

  /** @brief Constructor */
  EventQueue();

  /** @brief Destructor */
  ~EventQueue();

  /** Disabled copy constructor/assignment. These operations don't really make sense for this object. */
  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;
  EventQueue(EventQueue &&) = delete;
  EventQueue &operator=(EventQueue &&) = delete;

  /** @brief Broadcast an event to all subscribers */
  void postEvent();

  /** @brief Subscribes to an event and returns a handler
   *  @returns A CallbackHandler object. To unsubscribe destroy the object
   *           pointed by the handler. */
  CallbackHandler subscribeToEvent(const CallbackFunction &callback);

 private:
  using WeakCallbackHandler = std::weak_ptr<CallbackFunction>;
  const std::chrono::milliseconds max_sleep_interval_{100};
  std::atomic_bool pending_event_{false};
  std::atomic_bool halting_{false};
  std::condition_variable cv_;
  std::mutex mutex_;
  std::thread forwarder_thread_;
  std::list<WeakCallbackHandler> subscribers_;

  void runforwarder();
  void triggerSubscribers();
};

}  // namespace tools

}  // namespace glpuga
