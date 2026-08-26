#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {
namespace {

static void KeepFirstOverride(UiDesignerControlSpec& spec, const String& id)
{
    bool seen = false;
    for(int i = 0; i < spec.theme_overrides.GetCount();) {
        if(spec.theme_overrides[i].id == id) {
            if(seen) {
                spec.theme_overrides.Remove(i);
                continue;
            }
            seen = true;
        }
        ++i;
    }
}

class DeduplicatedThumbFaceThemeAdapter final : public UiDesignerThemeAdapter {
public:
    DeduplicatedThumbFaceThemeAdapter(const char *id,
                                      const UiDesignerThemeAdapter& base)
        : id_(id), base_(base) {}

    const char *Id() const override { return id_; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return base_.Supports(kind);
    }

    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        base_.AddThemeOverrides(spec);

        // The legacy core adapters replace thumb_face_normal with ink_normal,
        // but also re-add hot/pressed/disabled after AddPaletteMetrics already
        // created those three Thumb face fields. Keep the shared palette entry
        // and discard only the later duplicate. Runtime field ids are unchanged.
        KeepFirstOverride(spec, "thumb_face_hot");
        KeepFirstOverride(spec, "thumb_face_pressed");
        KeepFirstOverride(spec, "thumb_face_disabled");
    }

    bool HasField(const String& field_id) const override
    {
        return base_.HasField(field_id);
    }

    bool FieldAffectsLayout(const String& field_id) const override
    {
        return base_.FieldAffectsLayout(field_id);
    }

    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& field_id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        return base_.ResolveFieldValue(node, spec, field_id, overlay);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        base_.ApplyPreviewStyle(ctrl, node, spec, overlay);
    }

    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        base_.EmitSetup(out, member, node, spec);
    }

private:
    const char *id_;
    const UiDesignerThemeAdapter& base_;
};

DeduplicatedThumbFaceThemeAdapter slider_adapter_v2(
    "slider", UiDesignerSliderThemeAdapterInstance());
DeduplicatedThumbFaceThemeAdapter scroll_bar_adapter_v2(
    "scroll_bar", UiDesignerScrollBarThemeAdapterInstance());

} // namespace

const UiDesignerThemeAdapter& UiDesignerSliderThemeAdapterV2Instance()
{
    return slider_adapter_v2;
}

const UiDesignerThemeAdapter& UiDesignerScrollBarThemeAdapterV2Instance()
{
    return scroll_bar_adapter_v2;
}

} // namespace Upp
