#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiProgressRing.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

// Resolve through the reusable control itself; do not copy its role/preset
// algorithm into Designer or depend on a nonexistent ResolveProgressRing API.
UiProgressRing::Style RingBase(const UiDesignerNode& node)
{
    UiProgressRing ring;
    ring.SetRole(Role(node.GetProperty("role", "Standard")));
    return ring.GetStyle();
}

Value RingField(const UiProgressRing::Style& s, const String& id)
{
    int st = DotState(id, "progress");
    if(st >= 0) return s.progress_palette.face[st].color;
    st = DotState(id, "track");
    if(st >= 0) return s.track_palette.face[st].color;
    st = DotState(id, "text");
    if(st >= 0) return s.text_palette.ink[st];
    st = DotState(id, "gradient_end");
    if(st >= 0) return s.gradient_end[st];
    if(id == "gradient_enabled") return s.gradient_enabled;
    if(id == "thickness") return s.thickness;
    if(id == "cap_roundness") return s.cap_roundness;
    if(id == "ring_inset") return s.ring_inset;
    if(id == "min_text_height") return s.min_text_height;
    if(id == "font_height") return s.font.GetHeight();
    return Value();
}

void SetRingField(UiProgressRing::Style& s, const String& id, const Value& v)
{
    int st = DotState(id, "progress");
    if(st >= 0) { s.progress_palette.face[st] = UiFill::Solid((Color)v); return; }
    st = DotState(id, "track");
    if(st >= 0) { s.track_palette.face[st] = UiFill::Solid((Color)v); return; }
    st = DotState(id, "text");
    if(st >= 0) { s.text_palette.ink[st] = (Color)v; return; }
    st = DotState(id, "gradient_end");
    if(st >= 0) { s.gradient_end[st] = (Color)v; return; }
    if(id == "gradient_enabled") s.gradient_enabled = (bool)v;
    else if(id == "thickness") s.thickness = max(1, (int)v);
    else if(id == "cap_roundness") s.cap_roundness = clamp((int)v, 0, 100);
    else if(id == "ring_inset") s.ring_inset = max(0, (int)v);
    else if(id == "min_text_height") s.min_text_height = max(1, (int)v);
    else if(id == "font_height") s.font.Height(max(1, (int)v));
}

void EmitRingField(String& out, const String& var, const String& id, const Value& v)
{
    const char* names[] = {"progress", "track", "text", "gradient_end"};
    const char* members[] = {"progress_palette.face", "track_palette.face", "text_palette.ink", "gradient_end"};
    for(int i = 0; i < 4; i++) {
        int st = DotState(id, names[i]);
        if(st < 0) continue;
        out << "\t" << var << "." << members[i] << "[" << StateCode(st) << "] = ";
        if(i < 2) out << "UiFill::Solid(";
        out << EmitValue(v);
        if(i < 2) out << ")";
        out << ";\n";
        return;
    }
    if(id == "font_height")
        out << "\t" << var << ".font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "gradient_enabled")
        out << "\t" << var << ".gradient_enabled = " << EmitValue(v) << ";\n";
    else {
        int n = (int)v;
        if(id == "cap_roundness") n = clamp(n, 0, 100);
        else n = max(id == "ring_inset" ? 0 : 1, n);
        out << "\t" << var << "." << id << " = " << n << ";\n";
    }
}

class ProgressRingThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char* Id() const override { return "progress_ring"; }
    bool Supports(UiDesignerRuntimeKind k) const override { return k == UiDesignerRuntimeKind::UiProgressRing; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        const auto s = RingBase(UiDesignerNode());
        const char* states[] = {"normal", "hot", "pressed", "disabled"};
        for(const char* field : {"progress", "track", "text", "gradient_end"})
            for(const char* state : states) {
                String id = String(field) + "." + state;
                Add(spec, id, state, field, PropertyEditorKind::Color, RingField(s, id));
            }
        Add(spec, "gradient_enabled", "Enable gradient", "Ring", PropertyEditorKind::Boolean, s.gradient_enabled);
        AddInt(spec, "thickness", "Thickness", "Ring", s.thickness, 1, 256, true);
        AddInt(spec, "cap_roundness", "Cap roundness (%)", "Ring", s.cap_roundness, 0, 100);
        AddInt(spec, "ring_inset", "Ring inset", "Ring", s.ring_inset, 0, 256, true);
        AddInt(spec, "min_text_height", "Minimum text height", "Typography", s.min_text_height, 1, 128, true);
        AddInt(spec, "font_height", "Font height", "Typography", s.font.GetHeight(), 1, 128, true);
    }
    bool HasField(const String& id) const override
    {
        return !IsNull(RingField(UiProgressRing::StyleDefault(), id));
    }
    bool FieldAffectsLayout(const String& id) const override
    {
        return id == "thickness" || id == "ring_inset" || id == "min_text_height" || id == "font_height";
    }
    UiProgressRing::Style Resolve(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                                 const UiDesignerTransientOverlay* overlay) const
    {
        auto s = RingBase(node);
        for(const auto& p : spec.theme_overrides)
            if((node.IsThemeOverrideActive(p.id) && node.theme_overrides.Find(p.id) >= 0) ||
               (overlay && overlay->Has(node.id, UiDesignerTransientValueKind::ThemeOverride, p.id)))
                SetRingField(s, p.adapter_field_id, ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
        // These style-backed runtime switches are document configuration, not
        // appearance overrides. Theme changes must preserve canonical values.
        auto Config = [&](const char* id, Value fallback) {
            Value v = node.GetProperty(id, fallback);
            return overlay ? overlay->Resolve(node.id, UiDesignerTransientValueKind::NormalProperty, id, v) : v;
        };
        s.animate_on_show = (bool)Config("animate_on_show", true);
        s.intro_duration_ms = (int)Config("intro_duration", 600);
        s.indeterminate_duration_ms = (int)Config("indeterminate_duration", 1100);
        return s;
    }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        return RingField(Resolve(node, spec, overlay), id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        if(auto* ring = dynamic_cast<UiProgressRing*>(&ctrl)) {
            ring->SetRole(Role(node.GetProperty("role", "Standard")));
            ring->SetCustomStyle(Resolve(node, spec, overlay));
        }
    }
    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        out << "\t" << member << ".ClearCustomStyle().SetRole(" << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        bool authored = false;
        for(const auto& p : spec.theme_overrides)
            authored |= node.IsThemeOverrideActive(p.id) && node.theme_overrides.Find(p.id) >= 0;
        if(!authored) return;
        const String var = member + "_style";
        out << "\tUiProgressRing::Style " << var << " = " << member << ".GetStyle();\n";
        for(const auto& p : spec.theme_overrides) {
            int q = node.theme_overrides.Find(p.id);
            if(q >= 0 && node.IsThemeOverrideActive(p.id)) EmitRingField(out, var, p.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};
}

const UiDesignerThemeAdapter& UiDesignerProgressRingThemeAdapterInstance()
{
    static ProgressRingThemeAdapter adapter;
    return adapter;
}
}
