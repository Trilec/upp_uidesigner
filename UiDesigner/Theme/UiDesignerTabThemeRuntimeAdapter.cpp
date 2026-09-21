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

static bool IsTabThemeIdCollision(const String& id)
{
    static const char *ids[] = {
        "tab_font_face", "tab_font_bold", "tab_font_italic",
        "tab_padding_left", "tab_padding_top",
        "tab_padding_right", "tab_padding_bottom",
        "strip_inset_left", "strip_inset_top",
        "strip_inset_right", "strip_inset_bottom",
        "affordance_gap", "min_tab_main", "visual"
    };
    for(const char *candidate : ids)
        if(id == candidate)
            return true;
    return false;
}

static void DisambiguateTabThemeIds(UiDesignerControlSpec& spec)
{
    // Normal Designer Tab properties predate the complete Theme adapter and
    // intentionally keep their established ids. Theme overrides occupy a
    // separate document namespace, so finish the existing style_* convention
    // for every remaining overlap while preserving the canonical runtime field
    // in adapter_field_id.
    for(UiDesignerThemeOverrideSpec& property : spec.theme_overrides)
        if(IsTabThemeIdCollision(property.id))
            property.id = String("style_") + property.id;
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
        for(int i = spec.theme_overrides.GetCount() - 1; i >= 0; --i) {
            const String& id = spec.theme_overrides[i].adapter_field_id;
            if(id == "visual" || id == "icon_side" || id == "tab_font_face")
                spec.theme_overrides.Remove(i);
        }
        DisambiguateTabThemeIds(spec);
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
            // The visual family is authored control configuration, not a Theme field.
            const Value value = overlay
                ? overlay->Resolve(node.id, UiDesignerTransientValueKind::NormalProperty,
                                   "visual", node.GetProperty("visual", "Classic"))
                : node.GetProperty("visual", "Classic");
            tab->SetVisual(RuntimeTabVisual(value));
        }
        Base().ApplyPreviewStyle(ctrl, node, spec, overlay);
    }

    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        const UiTabVisual visual = RuntimeTabVisual(node.GetProperty("visual", "Classic"));

        String generated;
        Base().EmitSetup(generated, member, node, spec);

        if(visual != UITAB_CLASSIC) {
            // Seed appearance deltas from the control's authored visual family.
            const String classic = "UITAB_CLASSIC";
            const int q = generated.Find(classic);
            if(q >= 0)
                generated = generated.Left(q) + RuntimeTabVisualCode(visual) +
                            generated.Mid(q + classic.GetCount());
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
