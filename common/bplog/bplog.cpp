#include <iostream>
#include "bplog.hpp"
#include "bplogger.hpp"

namespace {
bp::BpLogger* g_bp_logger = nullptr;
};

namespace bp {

void InitLogging(const char* binary_name) {
    FLAGS_log_dir = ".";
    // output log immediately
    FLAGS_logbufsecs = 0;
    // set the log file to 100MB
    FLAGS_max_log_size = 100;
    FLAGS_stop_logging_if_full_disk = true;

    ::google::InitGoogleLogging(binary_name);
    ::google::SetLogDestination(::google::ERROR, "");
    ::google::SetLogDestination(::google::WARNING, "");
    ::google::SetLogDestination(::google::FATAL, "");

    g_bp_logger = new BpLogger(google::base::GetLogger(FLAGS_minloglevel));
    google::base::SetLogger(FLAGS_minloglevel, g_bp_logger);
    
    LOG(INFO) << "glog inited, using custom bp logger";
    LOG(INFO) << "glog FLAGS_log_dir=" << FLAGS_log_dir;
    LOG(INFO) << "glog FLAGS_minloglevel=" << FLAGS_minloglevel;
    LOG(INFO) << "glog FLAGS_max_log_size=" << FLAGS_max_log_size;
    LOG(INFO) << "glog FLAGS_alsologtostderr=" << FLAGS_alsologtostderr;
    LOG(INFO) << "glog FLAGS_colorlogtostderr=" << FLAGS_colorlogtostderr;
    LOG(INFO) << "glog FLAGS_logbufsecs=" << FLAGS_logbufsecs;
    LOG(INFO) << "glog FLAGS_v=" << FLAGS_v;
}

void RegisterWriteCallback(std::function<void(const std::string&)> func) {
    g_bp_logger->RegisterWriter(func);
}

void UnRegisterWriteCallback() {
    g_bp_logger->UnRegisterWriteCallback();
}   

} // namespace bp