#pragma once
#include <chrono>
#include <ratio>
#include <vector>

namespace sfe {

using stdclock = std::chrono::steady_clock;

struct SFETimeInfo {
    int step;
    std::string desc;
    stdclock::time_point ts;

    SFETimeInfo(int s, const std::string& d)
        : step(s)
        , desc(d)
        , ts(stdclock::now())
    {}
};

class SFETimeRecorder {
public:
    static SFETimeRecorder& Instance() {
        static SFETimeRecorder tr;
        return tr;
    }
    
    void Record(const std::string& desc) {
        _tss.emplace_back(++_idx, desc);
    }

    void Clear() {
        _idx = 0;
        _tss.clear();
    }

    std::string TimeCostInfo() {
        if (_tss.empty()) {
            return "";
        }
        std::stringstream ss;
        ss << "time cost info:\n" << _tss[0].desc << "\n";
        for (int i = 0; i < _tss.size() - 1; ++i) {
            auto cost_ms = std::chrono::duration_cast<std::chrono::milliseconds>(_tss[i + 1].ts - _tss[i].ts).count();
            ss << _tss[i + 1].desc << " cost " << cost_ms << "ms\n";
        }
        return ss.str();
    }

private:
    SFETimeRecorder() {}
    ~SFETimeRecorder() {};
    SFETimeRecorder(const SFETimeRecorder&);
    SFETimeRecorder& operator=(const SFETimeRecorder&);

private:
    int _idx = 0;
    std::vector<SFETimeInfo> _tss;

};

} // namespace sfe