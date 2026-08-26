#include "UiDesignerStyledThemeCommon.h"
#include <Ui/UiButton.h>
#include <Ui/UiToolButton.h>
#include <Ui/UiSplitButton.h>
#include <Ui/UiTheme.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;
using namespace UiDesignerStyledTheme;

static UiRole ButtonRole(const UiDesignerNode& node)
{
    return Role(node.GetProperty("role", "Standard"));
}

static UiButton::Style ResolveButtonStyle(UiDesignerRuntimeKind kind, UiRole role)
{
    if(kind == UiDesignerRuntimeKind::UiToolButton)
        return UiTheme::ResolveToolButton(role);
    return UiTheme::ResolveButton(role);
}

static String ButtonCommonId(const String& id)
{
    if(id == "text_normal") return "ink_normal";
    if(id == "text_hot") return "ink_hot";
    if(id == "text_pressed") return "ink_pressed";
    if(id == "text_disabled") return "ink_disabled";
    if(id == "shadow_offset_x") return "shadow_x";
    if(id == "shadow_offset_y") return "shadow_y";
    return id;
}

static void RenameOverride(UiDesignerControlSpec& spec, const String& from,
                           const String& to, const String& label = String())
{
    for(UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(p.id == from) {
            p.id = to;
            p.adapter_field_id = to;
            if(!label.IsEmpty())
                p.label = label;
            return;
        }
}

static void DisambiguateButtonThemeIds(UiDesignerControlSpec& spec)
{
    static const char *ids[] = {
        "align_h", "align_v", "icon_side", "content_gap", "icon_render_mode"
    };
    for(UiDesignerThemeOverrideSpec& property : spec.theme_overrides)
        for(const char *id : ids)
            if(property.id == id) {
                property.id = String("style_") + property.id;
                break;
            }
}

static void AddButtonOverrides(UiDesignerControlSpec& spec)
{
    const UiButton::Style s = ResolveButtonStyle(spec.runtime_kind, UiRole::Standard);
    AddPaletteMetrics(spec, "", "", s.palette, s.metrics,
                      true, true, true, true, true, true);

    // Retain the serialized button IDs that predate the normalized shared
    // vocabulary. Only their presentation/editor kind changes.
    RenameOverride(spec, "ink_normal", "text_normal");
    RenameOverride(spec, "ink_hot", "text_hot");
    RenameOverride(spec, "ink_pressed", "text_pressed");
    RenameOverride(spec, "ink_disabled", "text_disabled");
    RenameOverride(spec, "shadow_x", "shadow_offset_x");
    RenameOverride(spec, "shadow_y", "shadow_offset_y");

    Add(spec, "font_face", "Font face", "Typography",
        PropertyEditorKind::Text, s.font.GetFaceName(), true)
        .Editor("property.font");
    AddNumeric(spec, "font_size", "Font size", "Typography",
               max(1, s.font.GetHeight()), 6, 128, true);
    Add(spec, "font_bold", "Bold", "Typography",
        PropertyEditorKind::Boolean, s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography",
        PropertyEditorKind::Boolean, s.font.IsItalic(), true);

    Add(spec, "transparent", "Transparent", "General",
        PropertyEditorKind::Boolean, s.transparent);

    AddAlignChoice(spec, "align_h", "Horizontal align", "Appearance", s.align_h);
    AddAlignChoice(spec, "align_v", "Vertical align", "Appearance", s.align_v);
    AddAlignChoice(spec, "icon_side", "Icon side", "Appearance", s.icon_side, true);
    AddNumeric(spec, "content_gap", "Content gap", "Appearance",
               s.content_gap, 0, 128, true);
    AddIconMode(spec, "icon_render_mode", "Icon render mode", "Appearance",
                s.icon_render_mode);

    AddNumeric(spec, "press_offset_x", "Pressed offset X", "Additional",
               s.press_offset.x, -64, 64, true);
    AddNumeric(spec, "press_offset_y", "Pressed offset Y", "Additional",
               s.press_offset.y, -64, 64, true);
    AddNumeric(spec, "overpaint", "Overpaint", "Additional",
               s.overpaint, 0, 128, true);
    Add(spec, "underline_enabled", "Underline", "Additional",
        PropertyEditorKind::Boolean, s.underline, true);
    AddNumeric(spec, "underline_width", "Underline width", "Additional",
               s.underline_width, 0, 32, true);
    AddNumeric(spec, "underline_offset", "Underline offset", "Additional",
               s.underline_offset, -32, 64, true);

    // Button, ToolButton and SplitButton expose these layout/content controls
    // as normal Designer properties as well as durable Style fields. Keep the
    // established normal property ids and place the Theme document keys in the
    // same style_* namespace used for other normal/Theme overlaps. The adapter
    // field remains canonical so preview, resolution and code generation keep
    // applying UiButton::Style members by their original names.
    DisambiguateButtonThemeIds(spec);

    // UiButton::Style::skin is a real nine-slice skin. It remains deliberately
    // unexposed until ThemeDocument preview/codegen can resolve Designer
    // resources. When that contract exists the row should be named
    // "Skin (Nine Slice)", never a raw file path.
}

static bool ApplyButtonField(UiButton::Style& s, const String& id, const Value& value)
{
    const String common = ButtonCommonId(id);
    if(ApplyPaletteMetrics(s.palette, s.metrics, "", common, value))
        return true;

    if(id == "font_face") s.font.FaceName(AsString(value));
    else if(id == "font_size") s.font.Height(max(1, (int)value));
    else if(id == "font_bold") s.font.Bold((bool)value);
    else if(id == "font_italic") s.font.Italic((bool)value);
    else if(id == "transparent") s.transparent = (bool)value;
    else if(id == "align_h") s.align_h = AlignFromValue(value, s.align_h);
    else if(id == "align_v") s.align_v = AlignFromValue(value, s.align_v);
    else if(id == "icon_side") s.icon_side = AlignFromValue(value, s.icon_side);
    else if(id == "content_gap") s.content_gap = max(0, (int)value);
    else if(id == "icon_render_mode") s.icon_render_mode = IconModeFromValue(value);
    else if(id == "press_offset_x") s.press_offset.x = (int)value;
    else if(id == "press_offset_y") s.press_offset.y = (int)value;
    else if(id == "overpaint") s.overpaint = max(0, (int)value);
    else if(id == "underline_enabled") s.underline = (bool)value;
    else if(id == "underline_width") s.underline_width = max(0, (int)value);
    else if(id == "underline_offset") s.underline_offset = (int)value;
    else return false;
    return true;
}

static Value ButtonFieldValue(const UiButton::Style& s, const String& id)
{
    const String common = ButtonCommonId(id);
    if(IsPaletteMetricsField("", common))
        return PaletteMetricsValue(s.palette, s.metrics, "", common);
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_size") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "transparent") return s.transparent;
    if(id == "align_h") return AlignName(s.align_h);
    if(id == "align_v") return AlignName(s.align_v);
    if(id == "icon_side") return AlignName(s.icon_side);
    if(id == "content_gap") return s.content_gap;
    if(id == "icon_render_mode") return IconModeName(s.icon_render_mode);
    if(id == "press_offset_x") return s.press_offset.x;
    if(id == "press_offset_y") return s.press_offset.y;
    if(id == "overpaint") return s.overpaint;
    if(id == "underline_enabled") return s.underline;
    if(id == "underline_width") return s.underline_width;
    if(id == "underline_offset") return s.underline_offset;
    return Value();
}

static bool IsButtonExtra(const String& id)
{
    static const char *ids[] = {
        "font_face", "font_size", "font_bold", "font_italic", "transparent",
        "align_h", "align_v", "icon_side", "content_gap", "icon_render_mode",
        "press_offset_x", "press_offset_y", "overpaint", "underline_enabled",
        "underline_width", "underline_offset"
    };
    for(const char *candidate : ids)
        if(id == candidate)
            return true;
    return false;
}

static bool EmitButtonField(String& out, const String& var,
                            const String& id, const Value& value)
{
    const String common = ButtonCommonId(id);
    if(EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "",
                          common, value))
        return true;
    if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(value)) << ");\n";
    else if(id == "font_size") out << "\t" << var << ".font.Height(" << max(1, (int)value) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << AsString((bool)value) << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << AsString((bool)value) << ");\n";
    else if(id == "transparent") out << "\t" << var << ".transparent = " << AsString((bool)value) << ";\n";
    else if(id == "align_h") out << "\t" << var << ".align_h = " << AlignCode(AlignFromValue(value)) << ";\n";
    else if(id == "align_v") out << "\t" << var << ".align_v = " << AlignCode(AlignFromValue(value)) << ";\n";
    else if(id == "icon_side") out << "\t" << var << ".icon_side = " << AlignCode(AlignFromValue(value)) << ";\n";
    else if(id == "content_gap") out << "\t" << var << ".content_gap = " << max(0, (int)value) << ";\n";
    else if(id == "icon_render_mode") out << "\t" << var << ".icon_render_mode = " << IconModeCode(IconModeFromValue(value)) << ";\n";
    else if(id == "press_offset_x") out << "\t" << var << ".press_offset.x = " << (int)value << ";\n";
    else if(id == "press_offset_y") out << "\t" << var << ".press_offset.y = " << (int)value << ";\n";
    else if(id == "overpaint") out << "\t" << var << ".overpaint = " << max(0, (int)value) << ";\n";
    else if(id == "underline_enabled") out << "\t" << var << ".underline = " << AsString((bool)value) << ";\n";
    else if(id == "underline_width") out << "\t" << var << ".underline_width = " << max(0, (int)value) << ";\n";
    else if(id == "underline_offset") out << "\t" << var << ".underline_offset = " << (int)value << ";\n";
    else return false;
    return true;
}

class ButtonThemeAdapterV2 final : public UiDesignerThemeAdapter {
public:
    ButtonThemeAdapterV2(const char *id, UiDesignerRuntimeKind kind)
        : id_(id), kind_(kind) {}

    const char *Id() const override { return id_; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        if(kind_ == UiDesignerRuntimeKind::UiButton)
            return kind == UiDesignerRuntimeKind::UiButton ||
                   kind == UiDesignerRuntimeKind::UiSplitButton;
        return kind == kind_;
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        AddButtonOverrides(spec);
    }
    bool HasField(const String& id) const override
    {
        return IsPaletteMetricsField("", ButtonCommonId(id)) || IsButtonExtra(id);
    }
    bool FieldAffectsLayout(const String& id) const override
    {
        return PaletteMetricsAffectsLayout("", ButtonCommonId(id)) ||
               id == "font_face" || id == "font_size" || id == "font_bold" ||
               id == "font_italic" || id == "align_h" || id == "align_v" ||
               id == "icon_side" || id == "content_gap" ||
               id == "press_offset_x" || id == "press_offset_y" ||
               id == "overpaint" || id == "underline_enabled" ||
               id == "underline_width" || id == "underline_offset";
    }
    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiButton::Style style = ResolveButtonStyle(spec.runtime_kind, ButtonRole(node));
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            const int q = node.theme_overrides.Find(p.id);
            active = q >= 0 || HasValue(node, overlay, p.id);
            if(active) {
                const Value canonical = q >= 0 ? node.theme_overrides.GetValue(q)
                                               : p.default_value;
                ApplyButtonField(style, p.adapter_field_id,
                                 ResolveValue(node, overlay, p.id, canonical));
            }
        }
        return ButtonFieldValue(style, id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiButton *button = dynamic_cast<UiButton *>(&ctrl);
        if(!button)
            return;
        UiButton::Style style = ResolveButtonStyle(spec.runtime_kind, ButtonRole(node));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            const bool active = q >= 0 || HasValue(node, overlay, p.id);
            if(!active)
                continue;
            authored = true;
            const Value canonical = q >= 0 ? node.theme_overrides.GetValue(q)
                                           : p.default_value;
            ApplyButtonField(style, p.adapter_field_id,
                             ResolveValue(node, overlay, p.id, canonical));
        }
        if(authored || ButtonRole(node) != UiRole::Standard)
            button->SetCustomStyle(style);
        else
            button->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
            if(node.theme_overrides.Find(p.id) >= 0) { authored = true; break; }
        if(!authored && ButtonRole(node) == UiRole::Standard)
            return;
        const String var = member + "_style";
        if(spec.runtime_kind == UiDesignerRuntimeKind::UiToolButton)
            out << "\tUiButton::Style " << var << " = UiTheme::ResolveToolButton("
                << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        else
            out << "\tUiButton::Style " << var << " = UiTheme::ResolveButton("
                << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitButtonField(out, var, p.adapter_field_id,
                                node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }

private:
    const char *id_;
    UiDesignerRuntimeKind kind_;
};

ButtonThemeAdapterV2 button_adapter("button", UiDesignerRuntimeKind::UiButton);
ButtonThemeAdapterV2 tool_button_adapter("tool_button", UiDesignerRuntimeKind::UiToolButton);

} // namespace

const UiDesignerThemeAdapter& UiDesignerButtonThemeAdapterV2Instance()
{
    return button_adapter;
}

const UiDesignerThemeAdapter& UiDesignerToolButtonThemeAdapterV2Instance()
{
    return tool_button_adapter;
}

} // namespace Upp
