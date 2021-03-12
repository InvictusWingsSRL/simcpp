#ifndef __SIMCPP_H
#define __SIMCPP_H

#include "protothread.h"
#include <memory>
#include <queue>
#include <vector>

#define PROC_WAIT_FOR(event)                                                   \
  do {                                                                         \
    if ((event)->add_handler(shared_from_this())) {                            \
      PT_YIELD();                                                              \
    }                                                                          \
  } while (0)

using std::priority_queue;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

class Event;

class Process;

class Simulation : public std::enable_shared_from_this<Simulation> {
public:
  static shared_ptr<Simulation> create();

  template <class T, class... Args>
  shared_ptr<Process> start_process(Args &&...args) {
    return run_process(std::make_shared<T>(shared_from_this(), args...));
  }

  shared_ptr<Process> run_process(shared_ptr<Process> process);

  shared_ptr<Event> event();

  shared_ptr<Event> timeout(double delay);

  shared_ptr<Event> any_of(std::initializer_list<shared_ptr<Event>> events);

  shared_ptr<Event> all_of(std::initializer_list<shared_ptr<Event>> events);

  shared_ptr<Event> schedule(shared_ptr<Event> event, double delay = 0.0);

  bool step();

  void advance_by(double duration);

  void advance_to(shared_ptr<Event> event);

  void run();

  double get_now();

private:
  class QueuedEvent {
  public:
    double time;
    size_t id;
    shared_ptr<Event> event;

    QueuedEvent(double time, size_t id, shared_ptr<Event> event);

    bool operator<(const QueuedEvent &other) const;
  };

  double now = 0.0;
  size_t next_id = 0;
  priority_queue<QueuedEvent> queued_events;

  bool has_next();

  double peek_next_time();
};

class Event : public std::enable_shared_from_this<Event> {
public:
  enum class State { Pending, Triggered, Aborted };

  Event(shared_ptr<Simulation> sim);

  bool add_handler(shared_ptr<Process> process);

  void trigger();

  void abort();

  void fire();

  bool is_pending();

  bool is_triggered();

  bool is_aborted();

  bool is_processed();

  State get_state();

  virtual void Aborted();

protected:
  shared_ptr<Simulation> sim;

private:
  State state = State::Pending;
  unique_ptr<vector<shared_ptr<Process>>> listeners;
};

class Process : public Event, public Protothread {
public:
  Process(shared_ptr<Simulation> sim);

  void resume();

  shared_ptr<Process> shared_from_this();
};

class Condition : public Process {
public:
  Condition(shared_ptr<Simulation> sim, int n);

  virtual bool Run() override;

private:
  int n;
};

#endif
