#pragma once
#include "SFEPanel.hpp"
#include "Bp.hpp"

namespace sfe {

class SFEPanelUINodes : public SFEPanel {
public:
    bool Init() override;

    void Update() override;

    void Exit() override;
    /*
        注册生成节点函数
        注册目录
    */
   std::shared_ptr<::bp::BpNode> SpawnUINode(const std::string&);

private:
    std::shared_ptr<::BpContents> _uinode_contents;
};

} // namespace sfe