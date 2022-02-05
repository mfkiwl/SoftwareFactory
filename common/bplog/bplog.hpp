#pragma once
#include <functional>
#include <glog/logging.h>
#include <glog/raw_logging.h>

namespace bp {

void InitLogging(const char* binary_name);

void RegisterWriteCallback(std::function<void(const std::string&)> func);

void UnRegisterWriteCallback();

} // namespace bp