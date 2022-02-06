#include <memory>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "bplog/bplog.hpp"
#include "bpflags.hpp"
#include "bpbase.pb.h"
#include "TemplateFlags.hpp"
#include "Bp.hpp"
#include "BpGraph.hpp"

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::shared_ptr<bp::BpGraph> g = nullptr;
    Json::Value nodes_desc;
    auto state = bp::Bp::Instance().LoadGraph(bp::FLAGS_graph_exec_file, g, nodes_desc);
    if (state == bp::LoadSaveState::OK) {
        LOG(ERROR) << "Load " << bp::FLAGS_graph_exec_file << " failed, " << (int)state;
        return 1;
    }
    g->Run();
    return 0;
}