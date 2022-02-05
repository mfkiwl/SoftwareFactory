#include "bplogger.hpp"

namespace bp {

BpLogger::BpLogger(::google::base::Logger* warpped) 
    : _warpped(warpped)
{}

BpLogger::~BpLogger() {}

void BpLogger::Write(bool force_flush, time_t timestamp, const char* message, int message_len) {
    _warpped->Write(force_flush, timestamp, message, message_len);
    if(_write_cb) {
        _write_cb(std::string(message, message_len));
    }
}

void BpLogger::Flush() {
    _warpped->Flush();
}

uint32_t BpLogger::LogSize() {
    return _warpped->LogSize();
}

} // namespace bp