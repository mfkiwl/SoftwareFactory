#pragma once
#include <thread>
#include <ratio>
#include <chrono>

using stdclock = std::chrono::steady_clock;

namespace sfe {

class RateCtl {
public:
    RateCtl(int frequency = 30)
        : _start(stdclock::now())
        , _expected_cycle_time(int(1000.0 / frequency)) {
    }

    bool sleep() {
        auto expected_end = _start + std::chrono::milliseconds(_expected_cycle_time);
        auto actual_end = stdclock::now();
        auto sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>(expected_end - actual_end).count();

        _start = expected_end;

        if (sleep_time < 0) {
            if (actual_end > (expected_end + std::chrono::milliseconds(_expected_cycle_time))) {
                _start = actual_end;
            }
            return false;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        return true;
    }

    void reset() { _start = stdclock::now(); }

private:
    stdclock::time_point _start;
    int _expected_cycle_time;
};

}  // namespace sfe
