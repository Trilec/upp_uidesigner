#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {
namespace {

static bool IsSafeCatalogIconName(const String& name)
{
    if(!name.StartsWith("ICON_") || name.IsEmpty() || !IsAlpha(name[0]))
        return false;
    for(int i = 1; i < name.GetCount(); ++i)
        if(!IsAlNum(name[i]) && name[i] != '_')
            return false;
    return true;
}

class UiDesignerGroupPanelThemeAdapterV3 final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "group_panel"; }

    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return UiDesignerGroupPanelThemeAdapterInstance().Supports(kind);
    }

    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        UiDesignerGroupPanelThemeAdapterInstance().AddThemeOverrides(spec);
    }

    bool HasField(const String& field_id) const override
    {
        return UiDesignerGroupPanelThemeAdapterInstance().HasField(field_id);
    }

    bool FieldAffectsLayout(const String& field_id) const override
    {
        return UiDesignerGroupPanelThemeAdapterInstance().FieldAffectsLayout(field_id);
    }

    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& field_id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        return UiDesignerGroupPanelThemeAdapterInstance().ResolveFieldValue(
            node, spec, field_id, overlay);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiDesignerGroupPanelThemeAdapterInstance().ApplyPreviewStyle(
            ctrl, node, spec, overlay);
    }

    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        UiDesignerGroupPanelThemeAdapterInstance().EmitSetup(out, member, node, spec);

        const UiDesignerPropertySpec *icon = spec.FindProperty("icon");
        if(!icon)
            return;
        const String name = AsString(node.GetProperty("icon", icon->default_value));
        if(name.IsEmpty() || name == "None")
            out << "\t" << member << ".ClearIcon();\n";
        else if(IsSafeCatalogIconName(name))
            out << "\t" << member << ".SetIcon(" << name << "());\n";
        else
            out << "\t" << member << ".ClearIcon();\n";
    }
};

}

const UiDesignerThemeAdapter& UiDesignerGroupPanelThemeAdapterV3Instance()
{
    static UiDesignerGroupPanelThemeAdapterV3 adapter;
    return adapter;
}

}
