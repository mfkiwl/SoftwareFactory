#pragma once
#include <vector>
#include <memory>

#include "SFEPanel.hpp"

#include "GL/glew.h"

#include "BpPin.hpp"
#include "BpGraph.hpp"

#include "ax/Math2D.h"
#include "ax/Builders.h"
#include "ax/Widgets.h"
#include "imgui_node_editor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;
using ax::Widgets::IconType;

namespace sfe {

struct ImTexture {
    GLuint TextureID = 0;
    int    Width     = 0;
    int    Height    = 0;
};

class SFEPanelBp : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    void ShowVarNodeAttr();
    void ShowNode(util::BlueprintNodeBuilder& builder, std::shared_ptr<bp::BpObj>& node);
    void DrawPinIcon(bp::BpPin& pin, int alpha);
    ImColor GetNodeKindColor(bp::BpObjType kind);
    ImColor GetIconColor(bp::BpPin& pin);
    bool CanCreateLink(bp::BpPin* a, bp::BpPin* b);
    ImTextureID LoadTexture(const char* path);
    ImTextureID CreateTexture(const void* data, int width, int height);
    ImTexture _bg_texture;
    ed::EditorContext* _node_editor = nullptr;
    std::weak_ptr<bp::BpGraph> _graph;
    bool _is_doubleclick_node = false;
    std::string _doubleclick_node_attr = "";
    char _doubleclick_node_edit_attr[2048] = {};
    std::shared_ptr<bp::BpObj> _doubleclick_node = nullptr;
};

} // namespace sfe