#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiDateTime.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

UiDateTime::Style DateTimeBase(const UiDesignerNode& node)
{
    UiDateTime field;
    field.SetRole(Role(node.GetProperty("role", "Standard")))
         .SetButtonRole(Role(node.GetProperty("button_role", "Subtle")));
    UiDateTime::Style style = field.GetStyle();
    // Designer-owned read-only presentation must keep the selected Theme,
    // including when ShowPresentationFrame chooses the editable style.
    style.editable.show_readonly_bg = false;
    style.presentation.show_readonly_bg = false;
    return style;
}

Value DateTimeField(const UiDateTime::Style& style, const String& id)
{
    for(int part = 0; part < 2; ++part) {
        const String prefix = part ? "presentation_" : "editable_";
        const UiBaseEdit::Style& s = part ? style.presentation : style.editable;
        int st = DotState(id, prefix + "face");
        if(st >= 0) return s.palette.face[st].color;
        st = DotState(id, prefix + "text");
        if(st >= 0) return s.palette.ink[st];
        st = DotState(id, prefix + "frame");
        if(st >= 0) return s.palette.frame[st];
    }
    if(id == "font_height") return style.editable.font.GetHeight();
    if(id == "radius") return style.editable.metrics.radius;
    if(id == "frame_width") return style.editable.metrics.frame_width;
    return Value();
}

void SetDateTimeField(UiDateTime::Style& style, const String& id, const Value& value)
{
    for(int part = 0; part < 2; ++part) {
        const String prefix = part ? "presentation_" : "editable_";
        UiBaseEdit::Style& s = part ? style.presentation : style.editable;
        int st = DotState(id, prefix + "face");
        if(st >= 0) { s.palette.face[st] = UiFill::Solid((Color)value); return; }
        st = DotState(id, prefix + "text");
        if(st >= 0) { s.palette.ink[st] = (Color)value; return; }
        st = DotState(id, prefix + "frame");
        if(st >= 0) { s.palette.frame[st] = (Color)value; return; }
    }
    if(id == "font_height") {
        style.editable.font.Height(max(1, (int)value));
        style.presentation.font.Height(max(1, (int)value));
    }
    else if(id == "radius")
        style.editable.metrics.radius = style.presentation.metrics.radius = max(0, (int)value);
    else if(id == "frame_width")
        style.editable.metrics.frame_width = style.presentation.metrics.frame_width = max(0, (int)value);
}

void EmitDateTimeField(String& out, const String& var, const String& id, const Value& value)
{
    for(int part = 0; part < 2; ++part) {
        const String prefix = part ? "presentation_" : "editable_";
        const String field = part ? ".presentation" : ".editable";
        for(const char* channel : {"face", "text", "frame"}) {
            const int state = DotState(id, prefix + channel);
            if(state < 0) continue;
            const bool face = String(channel) == "face";
            out << "\t" << var << field << ".palette."
                << (String(channel) == "text" ? "ink" : channel)
                << "[" << StateCode(state) << "] = "
                << (face ? "UiFill::Solid(" : "") << EmitValue(value)
                << (face ? ")" : "") << ";\n";
            return;
        }
    }
    for(const char* part : {"editable", "presentation"}) {
        if(id == "font_height")
            out << "\t" << var << "." << part << ".font.Height(" << max(1, (int)value) << ");\n";
        else if(id == "radius" || id == "frame_width")
            out << "\t" << var << "." << part << ".metrics." << id << " = " << max(0, (int)value) << ";\n";
    }
}

class DateTimeThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char* Id() const override { return "date_time"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiDateTime; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        const auto style = DateTimeBase(UiDesignerNode());
        for(const char* prefix : {"editable_", "presentation_"})
            for(const char* channel : {"face", "text", "frame"})
                for(const char* state : {"normal", "hot", "pressed", "disabled"}) {
                    // Frameless presentation never paints its own face/frame.
                    if(String(prefix) == "presentation_" && String(channel) != "text") continue;
                    const String id = String(prefix) + channel + "." + state;
                    Add(spec, id, state, String(prefix) + channel,
                        PropertyEditorKind::Color, DateTimeField(style, id));
                }
        AddInt(spec, "font_height", "Font height", "Typography", style.editable.font.GetHeight(), 1, 128, true);
        AddInt(spec, "radius", "Radius", "Frame", style.editable.metrics.radius, 0, 256, true);
        AddInt(spec, "frame_width", "Frame width", "Frame", style.editable.metrics.frame_width, 0, 32, true);
    }
    bool HasField(const String& id) const override
    {
        if(id == "font_height" || id == "radius" || id == "frame_width") return true;
        for(const char* prefix : {"editable_", "presentation_"})
            for(const char* channel : {"face", "text", "frame"})
                if((String(prefix) != "presentation_" || String(channel) == "text") &&
                   DotState(id, String(prefix) + channel) >= 0) return true;
        return false;
    }
    bool FieldAffectsLayout(const String& id) const override
    { return id == "font_height" || id == "radius" || id == "frame_width"; }
    UiDateTime::Style Resolve(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                             const UiDesignerTransientOverlay* overlay) const
    {
        auto style = DateTimeBase(node);
        for(const auto& property : spec.theme_overrides)
            if((node.IsThemeOverrideActive(property.id) && node.theme_overrides.Find(property.id) >= 0) ||
               (overlay && overlay->Has(node.id, UiDesignerTransientValueKind::ThemeOverride, property.id)))
                SetDateTimeField(style, property.adapter_field_id,
                    ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        return style;
    }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    { return DateTimeField(Resolve(node, spec, overlay), id); }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        if(auto* field = dynamic_cast<UiDateTime*>(&ctrl)) {
            field->ClearCustomStyle().SetRole(Role(node.GetProperty("role", "Standard")))
                 .SetButtonRole(Role(node.GetProperty("button_role", "Subtle")));
            field->SetCustomStyle(Resolve(node, spec, overlay));
        }
    }
    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        out << "\t" << member << ".ClearCustomStyle().SetRole(" << RoleExpr(node.GetProperty("role", "Standard"))
            << ").SetButtonRole(" << RoleExpr(node.GetProperty("button_role", "Subtle")) << ");\n";
        const String var = member + "_date_time_style";
        out << "\tUiDateTime::Style " << var << " = " << member << ".GetStyle();\n";
        out << "\t" << var << ".editable.show_readonly_bg = false;\n";
        out << "\t" << var << ".presentation.show_readonly_bg = false;\n";
        for(const auto& property : spec.theme_overrides) {
            const int index = node.theme_overrides.Find(property.id);
            if(index >= 0 && node.IsThemeOverrideActive(property.id))
                EmitDateTimeField(out, var, property.adapter_field_id, node.theme_overrides.GetValue(index));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};
}

const UiDesignerThemeAdapter& UiDesignerDateTimeThemeAdapterInstance()
{
    static DateTimeThemeAdapter adapter;
    return adapter;
}
}
