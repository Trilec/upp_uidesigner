#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <Ui/UiTab.h>

namespace Upp {
namespace {

static UiTabVisual RuntimeTabVisual(const Value& value)
{
    const String s = AsString(value);
    if(s == "Underline") return UITAB_UNDERLINE;
    if(s == "Segmented") return UITAB_SEGMENTED;
    if(s == "Rail") return UITAB_RAIL;
    if(s == "Document") return UITAB_DOCUMENT;
    return UITAB_CLASSIC;
}

static const char *RuntimeTabVisualCode(UiTabVisual visual)
{
    switch(visual) {
    case UITAB_UNDERLINE: return "UITAB_UNDERLINE";
    case UITAB_SEGMENTED: return "UITAB_SEGMENTED";
    case UITAB_RAIL: return "UITAB_RAIL";
    case UITAB_DOCUMENT: return "UITAB_DOCUMENT";
    default: return "UITAB_CLASSIC";
    }
}

static bool FindAuthoredVisual(const UiDesignerNode& node,
                               const UiDesignerControlSpec& spec,
                               UiTabVisual& visual)
{
    for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
        if(property.adapter_field_id != "visual")
            continue;
        const int q = node.theme_overrides.Find(property.id);
        if(q < 0)
            return false;
        visual = RuntimeTabVisual(node.theme_overrides.GetValue(q));
        return true;
    }
    return false;
}

class UiDesignerTabThemeRuntimeAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "tab_runtime"; }

    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return Base().Supports(kind);
    }

    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        Base().AddThemeOverrides(spec);
    }

    bool HasField(const String& field_id) const override
    {
        return Base().HasField(field_id);
    }

    bool FieldAffectsLayout(const String& field_id) const override
    {
        return Base().FieldAffectsLayout(field_id);
    }

    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& field_id,
                            const UiDesignerTransientOverlay* overlay = nullptr) const override
    {
        return Base().ResolveFieldValue(node, spec, field_id, overlay);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay = nullptr) const override
    {
        UiTab *tab = dynamic_cast<UiTab *>(&ctrl);
        if(tab) {
            // UiTab keeps its visual family as instance state. Resolve the
            // effective recipe value first so the base adapter selects the
            // matching theme variant before it applies authored fields.
            const Value value = Base().ResolveFieldValue(
                node, spec, "visual", overlay);
            tab->SetVisual(RuntimeTabVisual(value));
        }
        Base().ApplyPreviewStyle(ctrl, node, spec, overlay);
    }

    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        UiTabVisual visual = UITAB_CLASSIC;
        const bool authored_visual = FindAuthoredVisual(node, spec, visual);

        String generated;
        Base().EmitSetup(generated, member, node, spec);

        if(authored_visual) {
            // The base adapter emits a role-aware UiTab::Style recipe. Replace
            // its resolver seed so locally authored style deltas inherit from
            // the same visual family that the runtime instance will use.
            const String classic = "UITAB_CLASSIC";
            const int q = generated.Find(classic);
            if(q >= 0)
                generated = generated.Left(q) + RuntimeTabVisualCode(visual) +
                            generated.Mid(q + classic.GetCount());
            out << "\t" << member << ".SetVisual("
                << RuntimeTabVisualCode(visual) << ");\n";
        }

        out << generated;
    }

private:
    static const UiDesignerThemeAdapter& Base()
    {
        return UiDesignerTabThemeAdapterInstance();
    }
};

UiDesignerTabThemeRuntimeAdapter s_tab_runtime_adapter;

} // namespace

const UiDesignerThemeAdapter& UiDesignerTabThemeRuntimeAdapterInstance()
{
    return s_tab_runtime_adapter;
}

} // namespace Upp
