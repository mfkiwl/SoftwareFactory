#pragma once
#include <functional>
#include <glog/logging.h>

namespace bp {

class BpLogger : public ::google::base::Logger {
public:
    explicit BpLogger(::google::base::Logger* warpped);
    ~BpLogger() override;

    void Write(bool force_flush, time_t timestamp, const char* message, int message_len) override;

    void Flush() override;

    uint32_t LogSize() override;

    void RegisterWriter(std::function<void(const std::string&)> func) { _write_cb = func; }

    void UnRegisterWriteCallback() { _write_cb = _null_write_cb; }
private:
    ::google::base::Logger* const _warpped;
    std::function<void(const std::string&)> _write_cb;
    std::function<void(const std::string&)> _null_write_cb;
};

} // namespace bp