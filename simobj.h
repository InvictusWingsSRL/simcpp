#include "simcpp.h"

#define OBSERVABLE_PROPERTY(TYP, NAM, VAL)                                     \
private:                                                                       \
  TYP NAM = VAL;                                                               \
                                                                               \
public:                                                                        \
  shared_ptr<Event> NAM##_event = std::make_shared<Event>(env);                \
  TYP get_##NAM() { return this->NAM; }                                        \
  void set_##NAM(TYP v) {                                                      \
    this->NAM = v;                                                             \
    this->env->schedule(this->NAM##_event);                                    \
    this->NAM##_event = std::make_shared<Event>(env);                          \
  }


namespace simcpp {

    class EnvObj {
    protected:
        std::shared_ptr<Simulation> env;

    public:
        explicit EnvObj(const std::shared_ptr<Simulation>& s) : env(s) {}
        virtual ~EnvObj() = default;
    };

}
