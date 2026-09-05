#include "UiDesignerCodeGen.h"

namespace Upp {

void UiDesignerCodeGenerator::EmitModelData(String& out, const UiDesignerNode& node) const
{
    if(node.type != "UiList" && node.type != "UiTree") return;
    const Value value = node.GetData("root");
    if(!value.Is<ValueMap>()) return;
    const ValueMap root = value;
    if(node.type == "UiList" && root.IsEmpty()) return;
    const String member = MemberName(node);
    out << "\t{\n\t\tauto& model = " << member << ".Model();\n\t\tmodel.Clear();\n";
    if(node.type == "UiList") {
        const Value items = UiDesignerMapValue(root, "items", ValueArray());
        if(items.Is<ValueArray>()) for(const Value& v : (ValueArray)items) {
            if(!v.Is<ValueMap>()) continue;
            const ValueMap item = v;
            out << "\t\t{ UiModelItem item(" << EmitValue(UiDesignerMapValue(item, "text", "Item"))
                << ", " << EmitValue(UiDesignerMapValue(item, "data", Value())) << ");\n";
            for(const char* field : {"enabled", "has_check", "checked"})
                out << "\t\t  item." << field << " = "
                    << EmitValue(UiDesignerMapValue(item, field, String(field) == "enabled")) << ";\n";
            for(const char* field : {"description", "right_text"})
                out << "\t\t  item." << field << " = "
                    << EmitValue(UiDesignerMapValue(item, field, String())) << ";\n";
            out << "\t\t  model.Add(item); }\n";
        }
    }
    else {
        out << "\t\tmodel.Set(model.Root(), UiModelItem("
            << EmitValue(UiDesignerMapValue(root, "text", "Root")) << ", "
            << EmitValue(UiDesignerMapValue(root, "data", Value())) << "));\n";
        int next = 0;
        Function<void(const String&, const Value&)> Children;
        Children = [&](const String& parent, const Value& children) {
            if(!children.Is<ValueArray>()) return;
            for(const Value& v : (ValueArray)children) {
                if(!v.Is<ValueMap>()) continue;
                const ValueMap item = v;
                const String name = "child_" + AsString(++next);
                out << "\t\tUiTreeNodeRef " << name << ";\n";
                out << "\t\t{ UiModelItem item(" << EmitValue(UiDesignerMapValue(item, "text", "Item"))
                    << ", " << EmitValue(UiDesignerMapValue(item, "data", Value())) << ");\n";
                out << "\t\t  item.enabled = " << EmitValue(UiDesignerMapValue(item, "enabled", true)) << ";\n";
                out << "\t\t  item.editable = " << EmitValue(UiDesignerMapValue(item, "editable", false)) << ";\n";
                out << "\t\t  " << name << " = model.AddChild(" << parent << ", item); }\n";
                Children(name, UiDesignerMapValue(item, "children", ValueArray()));
            }
        };
        Children("model.Root()", UiDesignerMapValue(root, "children", ValueArray()));
    }
    out << "\t}\n";
}

}
