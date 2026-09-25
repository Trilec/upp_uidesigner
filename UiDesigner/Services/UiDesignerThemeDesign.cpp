#include "UiDesignerAutomation.h"
#include "UiDesignerRuntimeTheme.h"
#include <cmath>

namespace Upp {

bool UiDesignerSession::SaveThemeFile(const String& path, String& error)
{
    if(theme_.HasProposal()) { error = "Keep or discard the proposed theme before saving"; return false; }
    UiDesignerExportRequest request;
    request.profile = UiDesignerExportProfile::ThemeJson;
    request.destination = path;
    request.write.overwrite = UiDesignerOverwritePolicy::ReplaceAll;
    auto result = UiDesignerExportService(catalog_).Execute(document_, theme_, request);
    if(!result.success) { error = result.diagnostic; return false; }
    theme_path_ = path; theme_file_checkpoint_ = theme_.Serialize(false);
    error.Clear(); return true;
}

bool UiDesignerSession::LoadThemeFile(const String& path, String& error)
{
    if(theme_.HasProposal()) { error = "Keep or discard the proposed theme before loading another"; return false; }
    if(!theme_.ImportTheme(LoadFile(path), error)) return false;
    theme_path_ = path; theme_file_checkpoint_ = theme_.Serialize(false);
    return true;
}

static Color MixThemeColor(Color a, Color b, int percent)
{
    return Color((a.GetR() * (100-percent) + b.GetR() * percent) / 100,
                 (a.GetG() * (100-percent) + b.GetG() * percent) / 100,
                 (a.GetB() * (100-percent) + b.GetB() * percent) / 100);
}

static double ThemeLuminance(Color color)
{
    auto channel = [](int v) { double x = v / 255.0; return x <= 0.04045 ? x / 12.92 : std::pow((x + 0.055) / 1.055, 2.4); };
    return 0.2126 * channel(color.GetR()) + 0.7152 * channel(color.GetG()) + 0.0722 * channel(color.GetB());
}

static Color ProgressTextColor(Color foreground, Color fill)
{
    double a = ThemeLuminance(foreground), b = ThemeLuminance(fill);
    if((max(a,b) + 0.05) / (min(a,b) + 0.05) >= 4.5) return foreground;
    return (b + 0.05) / 0.05 >= 1.05 / (b + 0.05) ? Black() : White();
}

static bool ValidateDesignStyle(const ValueMap& style, String& error)
{
    for(int i = 0; i < style.GetCount(); ++i) {
        String key = AsString(style.GetKey(i));
        Value v = style.GetValue(i);
        if(key == "body_font" || key == "heading_font") {
            bool installed = false;
            if(v.Is<String>()) for(int f = 0; f < Font::GetFaceCount(); ++f)
                if(Font::GetFaceName(f) == (String)v) installed = true;
            if(installed) continue;
            error = "Choose an installed font using list_fonts: " + key; return false;
        }
        if((key == "body_bold" || key == "heading_bold") && v.Is<bool>()) continue;
        if(key == "shadow" && v.Is<String>() && (v == "None" || v == "Hard")) continue;
        int lo = 0, hi = -1;
        if(key == "body_size" || key == "heading_size") { lo = 6; hi = 48; }
        if(key == "shadow_offset") hi = 12;
        if(key == "shadow_alpha") hi = 255;
        if(key == "line_width") hi = 6;
        if(hi >= 0 && IsNumber(v) && !v.Is<bool>() && (double)v >= lo &&
           (double)v <= hi && (double)v == (int)v) continue;
        error = "Invalid theme design style field: " + key; return false;
    }
    return true;
}

// Only assign fields the adapter actually exposes. Typography is hierarchical;
// shadows belong to outer surfaces, not every nested track and text decoration.
static bool DesignStyleField(const UiDesignerThemeOverrideSpec& f, const ValueMap& style, Value& value)
{
    String id = ToLower(f.adapter_field_id), key;
    bool heading = id.Find("title_font") >= 0 && id.Find("subtitle_font") < 0;
    if(id == "row_height" && style.Find("body_size") >= 0) {
        value = (int)style["body_size"] + 18; return true;
    }
    if(id == "header_height" && style.Find("heading_size") >= 0) {
        value = (int)style["heading_size"] + 12; return true;
    }
    if(id.Find("font") >= 0) {
        String prefix = heading ? "heading_" : "body_";
        if(id.EndsWith("font_face")) key = prefix + "font";
        else if(id.EndsWith("font_size") || id.EndsWith("font_height")) key = prefix + "size";
        else if(id.EndsWith("font_bold")) key = prefix + "bold";
    }
    else if(id == "shadow_enabled" && style.Find("shadow") >= 0) {
        value = style["shadow"] != "None"; return true;
    }
    else if(id == "shadow_mode" && style["shadow"] == "Hard") { value = "Hard"; return true; }
    else if(id == "shadow_distance" && style["shadow"] == "Hard") { value = 1; return true; }
    else if(id == "shadow_inset" && style["shadow"] == "Hard") { value = false; return true; }
    else if(id == "shadow_x" || id == "shadow_y" || id == "shadow_offset_x" || id == "shadow_offset_y") key = "shadow_offset";
    else if(id == "shadow_alpha") key = "shadow_alpha";
    else if(id == "line_width" || id == "separator_width" || id.EndsWith("_line_width")) key = "line_width";
    if(key.IsEmpty() || style.Find(key) < 0) return false;
    value = style[key];
    if(!IsNull(f.minimum) && IsNumber(value) && (double)value < (double)f.minimum) value = f.minimum;
    if(!IsNull(f.maximum) && IsNumber(value) && (double)value > (double)f.maximum) value = f.maximum;
    return true;
}

// This is an authoring policy, not a new runtime role resolver. Its output is
// ordinary explicit adapter recipes, still editable and exportable individually.
static bool BaselineField(const UiDesignerThemeOverrideSpec& f,
    const UiDesignerThemePalette& palette, int role, int radius, int border, Value& value)
{
    String id = ToLower(f.adapter_field_id);
    const Color paper = palette.Get(0), surface = palette.Get(1), line = palette.Get(2);
    const Color ink = palette.Get(3), key = palette.Get(role == 3 ? 5 : 4);
    const bool emphasized = role >= 2;
    Color face = emphasized ? MixThemeColor(surface, key, 12) : role == 1 ? paper : surface;
    // The border seed anchors outlines; a small role tint keeps emphasis without
    // making a yellow outline disappear against a yellow face or black shadow.
    Color edge = emphasized ? MixThemeColor(key, line, 75) : line;
    const bool disabled = id.Find("disabled") >= 0;
    const bool selected = id.Find("select") >= 0 || id.Find("pressed") >= 0;
    const bool hot = id.Find("hot") >= 0;
    if(selected) face = MixThemeColor(surface, key, 25);
    else if(hot) face = MixThemeColor(surface, key, 18);
    if(disabled) { face = MixThemeColor(surface, paper, 65); edge = line; }

    if(f.kind == PropertyEditorKind::Color || PropertyEditorKindName(f.kind) == "FillRecipe") {
        Color color;
        if(id.StartsWith("series.")) {
            int index = atoi(~id.Mid(7));
            color = MixThemeColor(key, index % 2 ? ink : surface, 12 + (index % 4) * 12);
        }
        else if(id.Find("shadow") >= 0) color = Color(0, 0, 0);
        else if(id.Find("ink") >= 0 || id.Find("text") >= 0 || id.Find("title") >= 0 ||
                id.Find("copy") >= 0 || id.Find("icon") >= 0 || id.Find("glyph") >= 0) {
            bool muted = disabled || id.Find("subtitle") >= 0 || id.Find("copy") >= 0 ||
                         id.Find("muted") >= 0 || id.Find("placeholder") >= 0 || id.Find("right_") >= 0;
            color = muted ? MixThemeColor(ink, surface, disabled ? 55 : 30) : ink;
        }
        else if(id.Find("progress") >= 0 || id.StartsWith("fill_face") ||
                id.Find("focus") >= 0 || id.Find("highlight") >= 0 ||
                id.Find("caret") >= 0 || id.Find("check") >= 0 ||
                id.Find("indicator") >= 0 || id.Find("drag") >= 0 || id.Find("metadata") >= 0)
            color = disabled ? MixThemeColor(key, surface, 60) : key;
        else if(id.Find("frame") >= 0 || id.Find("line") >= 0 || id.Find("separator") >= 0 ||
                id.Find("tick") >= 0 || id.Find("grip") >= 0 || id.Find("border") >= 0 || id.Find("grid") >= 0 || id.Find("guide") >= 0) color = edge;
        else if(id.Find("face") >= 0 || id.Find("bg") >= 0 || id.Find("background") >= 0 ||
                id.Find("track") >= 0 || id.Find("selection") >= 0 || id.Find("gradient") >= 0)
            color = id.Find("track") >= 0 ? MixThemeColor(surface, key, 15) : face;
        else return false;
        if(f.kind == PropertyEditorKind::Color) value = color;
        else { ValueMap fill; fill.Set("schema", 1); fill.Set("mode", "Solid"); fill.Set("solid", color); value = fill; }
        return true;
    }
    if(id == "frame_enabled" || id.EndsWith("_frame_enabled")) { value = border > 0; return true; }
    if(id == "radius" || id.EndsWith("_radius") || id.EndsWith(".radius")) value = radius;
    else if(id == "frame_width" || id.EndsWith("_frame_width") || id.EndsWith(".frame_width")) value = border;
    else return false;
    if(!IsNull(f.minimum) && (double)value < (double)f.minimum) value = f.minimum;
    if(!IsNull(f.maximum) && (double)value > (double)f.maximum) value = f.maximum;
    return true;
}

bool UiDesignerAutomationService::BuildThemeDesign(const ValueMap& params,
    const UiDesignerThemeSnapshot& base, UiDesignerThemeSnapshot& result, String& error) const
{
    result = base;
    ValueMap style;
    if(params.Find("style") >= 0) {
        if(!params["style"].Is<ValueMap>()) { error = "Style must be an object"; return false; }
        style = params["style"];
        if(!ValidateDesignStyle(style, error)) return false;
    }
    if(!params["light"].Is<ValueArray>() || !params["dark"].Is<ValueArray>() ||
       !result.light_palette.FromValue(params["light"], error) ||
       !result.dark_palette.FromValue(params["dark"], error)) {
        if(error.IsEmpty()) error = "Expected six hex colours for both light and dark";
        return false;
    }
    int radius = (int)params["radius"], border = (int)params["border_width"];
    if(radius < 0 || radius > 32 || border < 0 || border > 6) {
        error = "Radius must be 0..32 and border width 0..6"; return false;
    }
    result.radius = radius; result.border_width = border;
    bool replace = params["replace_authored"] == true;
    for(const auto& spec : session_.Catalog().GetControls()) {
        if(spec.theme_overrides.IsEmpty()) continue;
        const auto* role_field = spec.FindProperty("role");
        if(!role_field) continue;
        const char* names[] = { "Standard", "Subtle", "Accent", "Alert" };
        for(int dark = 0; dark < 2; ++dark) for(int role = 0; role < 4; ++role) {
            bool supported = false;
            for(const auto& choice : role_field->choices) if(choice.value == names[role]) supported = true;
            if(!supported) continue;
            String target = String(dark ? "Dark" : "Light") + "|" +
                (UiDesignerUsesPanelThemeDomain(spec) ? "panel" : "control") + "|" + spec.type_id + "|" + names[role];
            for(const auto& field : spec.theme_overrides) {
                ValueMap generated;
                int g = base.generated_fields.Find(target);
                if(g >= 0) generated = base.generated_fields.GetValue(g);
                bool owned = generated.Find(field.id) >= 0 && generated[field.id] == base.GetStyleOverride(target, field.id);
                if(field.read_only || field.designer_only || (!replace && !owned && base.HasStyleOverride(target, field.id))) continue;
                Value value;
                bool thin_track = spec.type_id == "UiSlider" || spec.type_id == "UiScrollBar";
                bool no_shadow = style.Find("shadow") >= 0 &&
                    field.adapter_field_id.EndsWith("shadow_enabled") &&
                    (thin_track || field.adapter_field_id != "shadow_enabled");
                if(no_shadow) value = false;
                if(no_shadow || DesignStyleField(field, style, value) || BaselineField(field, result.GetPalette(dark), role, radius, border, value)) {
                    if((thin_track && field.adapter_field_id == "frame_enabled") ||
                       (spec.type_id == "UiProgressBar" && field.adapter_field_id == "fill_frame_enabled")) value = false;
                    if(spec.type_id == "UiProgressBar" && field.adapter_field_id == "ink_normal")
                        value = ProgressTextColor(result.GetPalette(dark).Get(3), result.GetPalette(dark).Get(role == 3 ? 5 : 4));
                    result.SetStyleOverride(target, field.id, value);
                    ValueMap ownership;
                    int q = result.generated_fields.Find(target);
                    if(q >= 0) ownership = result.generated_fields.GetValue(q);
                    ownership.Set(field.id, value); result.generated_fields.Set(target, ownership);
                }
            }
        }
    }
    result.SyncLegacyAccent(); error.Clear(); return true;
}
}
