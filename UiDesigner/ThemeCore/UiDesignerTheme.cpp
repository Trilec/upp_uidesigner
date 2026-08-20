#include "UiDesignerTheme.h"

namespace Upp {

static bool IsThemePresetName(const String& value)
{
    static const char *names[] = {
        "Minimal", "Pill", "Linear", "Solid", "Outline", "Compact", "Layered"
    };
    for(int i = 0; i < __countof(names); i++)
        if(value == names[i])
            return true;
    return false;
}

static bool IsThemeModeName(const String& value)
{
    return value == "Light" || value == "Dark" || value == "System";
}

static String ThemeColorText(Color color)
{
    return Format("#%02X%02X%02X", color.GetR(), color.GetG(), color.GetB());
}

static bool ParseThemeColor(const Value& value, Color& color)
{
    if(value.Is<Color>()) {
        color = value;
        return true;
    }
    String text = value;
    if(text.GetCount() != 7 || text[0] != '#')
        return false;
    int digit[6];
    for(int i = 0; i < 6; i++) {
        int c = text[i + 1];
        if(c >= '0' && c <= '9') digit[i] = c - '0';
        else if(c >= 'a' && c <= 'f') digit[i] = c - 'a' + 10;
        else if(c >= 'A' && c <= 'F') digit[i] = c - 'A' + 10;
        else return false;
    }
    color = Color(digit[0] * 16 + digit[1], digit[2] * 16 + digit[3],
                  digit[4] * 16 + digit[5]);
    return true;
}

// Theme Studio style recipes can contain typed U++ Values such as Color,
// including Colors nested inside compound adapter recipes. Keep standalone
// Theme JSON portable by using the same explicit recursive representation as
// Designer document serialization instead of asking AsJSON to encode runtime
// Value types directly.
static Value EncodeThemeStyleValue(const Value& value)
{
    if(value.Is<Color>()) {
        Color c = value;
        ValueMap encoded;
        encoded.Set("$type", "Color");
        encoded.Set("r", c.GetR());
        encoded.Set("g", c.GetG());
        encoded.Set("b", c.GetB());
        return encoded;
    }
    if(value.Is<ValueArray>()) {
        ValueArray source = value;
        ValueArray encoded;
        for(const Value& item : source)
            encoded.Add(EncodeThemeStyleValue(item));
        return encoded;
    }
    if(value.Is<ValueMap>()) {
        ValueMap source = value;
        ValueMap encoded;
        for(int i = 0; i < source.GetCount(); i++)
            encoded.Set(source.GetKey(i),
                        EncodeThemeStyleValue(source.GetValue(i)));
        return encoded;
    }
    return value;
}

static Value DecodeThemeStyleValue(const Value& value)
{
    if(value.Is<ValueMap>()) {
        ValueMap source = value;
        if((String)UiDesignerMapValue(source, "$type", "") == "Color")
            return Color((int)UiDesignerMapValue(source, "r", 0),
                         (int)UiDesignerMapValue(source, "g", 0),
                         (int)UiDesignerMapValue(source, "b", 0));
        ValueMap decoded;
        for(int i = 0; i < source.GetCount(); i++)
            decoded.Set(source.GetKey(i),
                        DecodeThemeStyleValue(source.GetValue(i)));
        return decoded;
    }
    if(value.Is<ValueArray>()) {
        ValueArray source = value;
        ValueArray decoded;
        for(const Value& item : source)
            decoded.Add(DecodeThemeStyleValue(item));
        return decoded;
    }
    return value;
}

static int ThemePaletteIndex(int value)
{
    return minmax(value, 0, UI_DESIGNER_THEME_PALETTE_SIZE - 1);
}

Color UiDesignerThemePalette::Get(int index) const
{
    return colors[ThemePaletteIndex(index)];
}

void UiDesignerThemePalette::Set(int index, Color color)
{
    colors[ThemePaletteIndex(index)] = color;
}

ValueArray UiDesignerThemePalette::ToValue() const
{
    ValueArray out;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        out.Add(ThemeColorText(colors[i]));
    return out;
}

bool UiDesignerThemePalette::FromValue(const Value& value, String& error)
{
    if(!value.Is<ValueArray>()) {
        error = "Theme palette must be an array";
        return false;
    }
    ValueArray in = value;
    if(in.GetCount() != UI_DESIGNER_THEME_PALETTE_SIZE) {
        error = Format("Theme palette requires exactly %d colours",
                       UI_DESIGNER_THEME_PALETTE_SIZE);
        return false;
    }
    Color parsed[UI_DESIGNER_THEME_PALETTE_SIZE];
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        if(!ParseThemeColor(in[i], parsed[i])) {
            error = Format("Invalid theme palette colour at slot %d", i + 1);
            return false;
        }
    }
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        colors[i] = parsed[i];
    error.Clear();
    return true;
}

ValueMap UiDesignerThemeRoleAssignments::ToValue() const
{
    ValueMap controls;
    controls.Set("standard", control_standard);
    controls.Set("subtle", control_subtle);
    controls.Set("accent", control_accent);
    controls.Set("alert", control_alert);

    ValueMap panels;
    panels.Set("surface", panel_surface);
    panels.Set("subtle", panel_subtle);
    panels.Set("strong", panel_strong);

    ValueMap out;
    out.Set("controls", controls);
    out.Set("panels", panels);
    return out;
}

bool UiDesignerThemeRoleAssignments::FromValue(const Value& value, String& error)
{
    if(!value.Is<ValueMap>()) {
        error = "Theme roles must be an object";
        return false;
    }
    ValueMap root = value;
    Value controls_value = UiDesignerMapValue(root, "controls", ValueMap());
    Value panels_value = UiDesignerMapValue(root, "panels", ValueMap());
    if(!controls_value.Is<ValueMap>() || !panels_value.Is<ValueMap>()) {
        error = "Theme roles require controls and panels objects";
        return false;
    }
    ValueMap controls = controls_value;
    ValueMap panels = panels_value;
    control_standard = ThemePaletteIndex((int)UiDesignerMapValue(
        controls, "standard", control_standard));
    control_subtle = ThemePaletteIndex((int)UiDesignerMapValue(
        controls, "subtle", control_subtle));
    control_accent = ThemePaletteIndex((int)UiDesignerMapValue(
        controls, "accent", control_accent));
    control_alert = ThemePaletteIndex((int)UiDesignerMapValue(
        controls, "alert", control_alert));
    panel_surface = ThemePaletteIndex((int)UiDesignerMapValue(
        panels, "surface", panel_surface));
    panel_subtle = ThemePaletteIndex((int)UiDesignerMapValue(
        panels, "subtle", panel_subtle));
    panel_strong = ThemePaletteIndex((int)UiDesignerMapValue(
        panels, "strong", panel_strong));
    error.Clear();
    return true;
}

UiDesignerThemeSnapshot::UiDesignerThemeSnapshot()
{
    dark_palette.Set(0, Color(15, 23, 42));
    dark_palette.Set(1, Color(25, 25, 25));
    dark_palette.Set(2, Color(51, 65, 85));
    dark_palette.Set(3, Color(241, 245, 249));
    dark_palette.Set(4, Color(96, 165, 250));
    dark_palette.Set(5, Color(248, 113, 113));
    SyncLegacyAccent();
}

Color UiDesignerThemeSnapshot::GetActiveAccent() const
{
    return GetPalette(UsesDarkPalette()).Get(roles.control_accent);
}

void UiDesignerThemeSnapshot::SyncLegacyAccent()
{
    accent = GetActiveAccent();
}

ValueMap UiDesignerThemeSnapshot::GetStyleOverrides(const String& target) const
{
    const int q = style_overrides.Find(target);
    if(q < 0)
        return ValueMap();
    const Value value = style_overrides.GetValue(q);
    return value.Is<ValueMap>() ? (ValueMap)value : ValueMap();
}

bool UiDesignerThemeSnapshot::HasStyleOverride(const String& target,
                                               const String& field) const
{
    return GetStyleOverrides(target).Find(field) >= 0;
}

Value UiDesignerThemeSnapshot::GetStyleOverride(const String& target,
                                                const String& field,
                                                const Value& fallback) const
{
    ValueMap values = GetStyleOverrides(target);
    const int q = values.Find(field);
    return q >= 0 ? values.GetValue(q) : fallback;
}

void UiDesignerThemeSnapshot::SetStyleOverride(const String& target,
                                               const String& field,
                                               const Value& value)
{
    if(target.IsEmpty() || field.IsEmpty())
        return;
    ValueMap values = GetStyleOverrides(target);
    values.Set(field, value);
    style_overrides.Set(target, values);
}

bool UiDesignerThemeSnapshot::RemoveStyleOverride(const String& target,
                                                  const String& field)
{
    if(target.IsEmpty() || field.IsEmpty())
        return false;
    const int target_index = style_overrides.Find(target);
    ValueMap values = GetStyleOverrides(target);
    if(target_index < 0 || values.Find(field) < 0)
        return false;
    ValueMap kept;
    for(int i = 0; i < values.GetCount(); i++)
        if(AsString(values.GetKey(i)) != field)
            kept.Set(values.GetKey(i), values.GetValue(i));
    if(kept.IsEmpty())
        style_overrides.Remove(target_index);
    else
        style_overrides.Set(target, kept);
    return true;
}

ValueMap UiDesignerThemeSnapshot::ToValue() const
{
    ValueMap palettes;
    palettes.Set("light", light_palette.ToValue());
    palettes.Set("dark", dark_palette.ToValue());

    ValueMap out;
    out.Set("preset", preset);
    out.Set("mode", mode);
    out.Set("accent", ThemeColorText(accent));
    out.Set("palettes", palettes);
    out.Set("roles", roles.ToValue());
    out.Set("styles", EncodeThemeStyleValue(style_overrides));
    out.Set("spacing", spacing);
    out.Set("radius", radius);
    out.Set("pill_radius", pill_radius);
    out.Set("border_width", border_width);
    out.Set("shadows", shadows);
    out.Set("shadow_distance", shadow_distance);
    out.Set("shadow_offset_y", shadow_offset_y);
    out.Set("shadow_alpha", shadow_alpha);
    return out;
}

bool UiDesignerThemeSnapshot::FromValue(const Value& value, String& error)
{
    if(!value.Is<ValueMap>()) {
        error = "Theme root must be an object";
        return false;
    }

    *this = UiDesignerThemeSnapshot();
    ValueMap in = value;
    preset = UiDesignerMapValue(in, "preset", "Minimal");
    mode = UiDesignerMapValue(in, "mode", "Light");
    if(!IsThemePresetName(preset))
        preset = "Minimal";
    if(!IsThemeModeName(mode))
        mode = "Light";

    Color parsed_accent = Color(58, 132, 255);
    ParseThemeColor(UiDesignerMapValue(in, "accent", "#3A84FF"), parsed_accent);

    const int palettes_index = in.Find("palettes");
    const bool has_palettes = palettes_index >= 0;
    if(has_palettes) {
        Value palettes_value = in.GetValue(palettes_index);
        if(!palettes_value.Is<ValueMap>()) {
            error = "Theme palettes must be an object";
            return false;
        }
        ValueMap palettes = palettes_value;
        if(!light_palette.FromValue(
               UiDesignerMapValue(palettes, "light", ValueArray()), error))
            return false;
        if(!dark_palette.FromValue(
               UiDesignerMapValue(palettes, "dark", ValueArray()), error))
            return false;

        if(in.Find("roles") >= 0 &&
           !roles.FromValue(UiDesignerMapValue(in, "roles", ValueMap()), error))
            return false;
    }
    else {
        // Schema-1 themes had one accent. Preserve it in both appearance
        // palettes so migration does not silently change colour on mode switch.
        light_palette.Set(roles.control_accent, parsed_accent);
        dark_palette.Set(roles.control_accent, parsed_accent);
    }

    if(in.Find("styles") >= 0) {
        Value styles = DecodeThemeStyleValue(
            UiDesignerMapValue(in, "styles", ValueMap()));
        if(!styles.Is<ValueMap>()) {
            error = "Theme styles must be an object";
            return false;
        }
        ValueMap map = styles;
        for(int i = 0; i < map.GetCount(); i++)
            if(!map.GetValue(i).Is<ValueMap>()) {
                error = "Theme style target must contain a field object";
                return false;
            }
        style_overrides = map;
    }

    spacing = UiDesignerMapValue(in, "spacing", 8);
    radius = UiDesignerMapValue(in, "radius", 8);
    pill_radius = UiDesignerMapValue(in, "pill_radius", 25);
    border_width = UiDesignerMapValue(in, "border_width", 1);
    shadows = UiDesignerMapValue(in, "shadows", true);
    shadow_distance = UiDesignerMapValue(in, "shadow_distance", 6);
    shadow_offset_y = UiDesignerMapValue(in, "shadow_offset_y", 2);
    shadow_alpha = UiDesignerMapValue(in, "shadow_alpha", 24);
    SyncLegacyAccent();
    error.Clear();
    return true;
}

UiDesignerThemeDocument::UiDesignerThemeDocument()
{
    preview_ = value_;
}

void UiDesignerThemeDocument::SetPropertyModelProvider(
    Function<void(PropertyEditorModel&, const UiDesignerThemeSnapshot&)> provider)
{
    property_model_provider_ = pick(provider);
}

void UiDesignerThemeDocument::ClearPropertyModelProvider()
{
    property_model_provider_.Clear();
}

void UiDesignerThemeDocument::SetActiveStyleTarget(const String& target)
{
    if(active_style_target_ == target)
        return;
    if(preview_active_) {
        preview_ = value_;
        preview_active_ = false;
    }
    active_style_target_ = target;
    // Reuse the existing theme projection notification. Session rebuilds the
    // ThemeModel in response, and the Window already observes this event.
    WhenPreview();
}

static void AddThemeChoice(PropertyEditorModel& model, const String& id,
                           const String& label, const Value& value,
                           const char *const *choices, int count)
{
    PropertyEditorItem& item = model.AddChoice(id, label, value, "Theme");
    for(int i = 0; i < count; i++)
        item.AddChoice(choices[i], choices[i]);
    item.SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal |
                   PropertyImpactPaint |
                   PropertyImpactFullPreview);
}

static void AddPaletteRoleChoice(PropertyEditorModel& model, const String& id,
                                 const String& label, int value,
                                 const String& group)
{
    PropertyEditorItem& item = model.AddChoice(id, label, value, group);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        item.AddChoice(i, "Palette " + AsString(i + 1));
    item.SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal |
                   PropertyImpactPaint |
                   PropertyImpactFullPreview);
}

void UiDesignerThemeDocument::BuildPropertyModel(
    PropertyEditorModel& model) const
{
    const UiDesignerThemeSnapshot& t = GetEffective();
    if(property_model_provider_) {
        property_model_provider_(model, t);
        return;
    }

    const UiDesignerThemeSnapshot defaults;
    model.Clear(false);

    static const char *preset_choices[] = {
        "Minimal", "Pill", "Linear", "Solid", "Outline", "Compact", "Layered"
    };
    static const char *mode_choices[] = {"Light", "Dark", "System"};
    AddThemeChoice(model, "preset", "Preset", t.preset,
                   preset_choices, __countof(preset_choices));
    AddThemeChoice(model, "mode", "Mode", t.mode,
                   mode_choices, __countof(mode_choices));

    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        const String id = "palette.light." + AsString(i);
        model.AddColor(id, "Colour " + AsString(i + 1),
                       t.light_palette.Get(i), "Palette / Light")
            .SetDefault(defaults.light_palette.Get(i))
            .SetDomain(PropertyEditorDomain::Theme)
            .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);
    }
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        const String id = "palette.dark." + AsString(i);
        model.AddColor(id, "Colour " + AsString(i + 1),
                       t.dark_palette.Get(i), "Palette / Dark")
            .SetDefault(defaults.dark_palette.Get(i))
            .SetDomain(PropertyEditorDomain::Theme)
            .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);
    }

    AddPaletteRoleChoice(model, "roles.control.standard", "Standard",
                         t.roles.control_standard, "Control Roles");
    AddPaletteRoleChoice(model, "roles.control.subtle", "Subtle",
                         t.roles.control_subtle, "Control Roles");
    AddPaletteRoleChoice(model, "roles.control.accent", "Accent",
                         t.roles.control_accent, "Control Roles");
    AddPaletteRoleChoice(model, "roles.control.alert", "Alert",
                         t.roles.control_alert, "Control Roles");

    AddPaletteRoleChoice(model, "roles.panel.surface", "Surface",
                         t.roles.panel_surface, "Panel Roles");
    AddPaletteRoleChoice(model, "roles.panel.subtle", "Subtle",
                         t.roles.panel_subtle, "Panel Roles");
    AddPaletteRoleChoice(model, "roles.panel.strong", "Strong",
                         t.roles.panel_strong, "Panel Roles");

    model.AddSliderInt("spacing", "Spacing", t.spacing, 0, 32, 1,
                       "Metrics")
        .SetDefault(8)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal |
                   PropertyImpactLocalLayout);

    model.AddSliderInt("radius", "Surface radius", t.radius, 0, 32, 1,
                       "Metrics")
        .SetDefault(8)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddSliderInt("pill_radius", "Pill radius", t.pill_radius, 0, 40, 1,
                       "Metrics")
        .SetDefault(25)
        .SetHelp("The authored icon-holder capsule radius.")
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddSliderInt("border_width", "Border width", t.border_width, 0, 6, 1,
                       "Metrics")
        .SetDefault(1)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddBoolean("shadows", "Shadows", t.shadows, "Shadow")
        .SetDefault(true)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddSliderInt("shadow_distance", "Distance", t.shadow_distance, 0, 24, 1,
                       "Shadow")
        .SetDefault(6)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddSliderInt("shadow_offset_y", "Y offset", t.shadow_offset_y, -12, 12, 1,
                       "Shadow")
        .SetDefault(2)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.AddSliderInt("shadow_alpha", "Alpha", t.shadow_alpha, 0, 255, 1,
                       "Shadow")
        .SetDefault(24)
        .SetDomain(PropertyEditorDomain::Theme)
        .SetImpact(PropertyImpactThemeGlobal | PropertyImpactPaint);

    model.StructureChanged();
}

Value UiDesignerThemeDocument::GetProperty(
    const UiDesignerThemeSnapshot& source, const String& property) const
{
    if(property.StartsWith("studio."))
        return source.GetStyleOverride(active_style_target_, property.Mid(7));
    if(property == "preset") return source.preset;
    if(property == "mode") return source.mode;
    if(property == "accent") return source.accent;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        if(property == "palette.light." + AsString(i))
            return source.light_palette.Get(i);
        if(property == "palette.dark." + AsString(i))
            return source.dark_palette.Get(i);
    }
    if(property == "roles.control.standard") return source.roles.control_standard;
    if(property == "roles.control.subtle") return source.roles.control_subtle;
    if(property == "roles.control.accent") return source.roles.control_accent;
    if(property == "roles.control.alert") return source.roles.control_alert;
    if(property == "roles.panel.surface") return source.roles.panel_surface;
    if(property == "roles.panel.subtle") return source.roles.panel_subtle;
    if(property == "roles.panel.strong") return source.roles.panel_strong;
    if(property == "spacing") return source.spacing;
    if(property == "radius") return source.radius;
    if(property == "pill_radius") return source.pill_radius;
    if(property == "border_width") return source.border_width;
    if(property == "shadows") return source.shadows;
    if(property == "shadow_distance") return source.shadow_distance;
    if(property == "shadow_offset_y") return source.shadow_offset_y;
    if(property == "shadow_alpha") return source.shadow_alpha;
    return Value();
}

bool UiDesignerThemeDocument::SetProperty(
    UiDesignerThemeSnapshot& target, const String& property,
    const Value& value, String& error) const
{
    if(property.StartsWith("studio.")) {
        if(active_style_target_.IsEmpty()) {
            error = "Select a Theme Studio sample first";
            return false;
        }
        const String field = property.Mid(7);
        if(field.IsEmpty()) {
            error = "Theme Studio style field is empty";
            return false;
        }
        target.SetStyleOverride(active_style_target_, field, value);
        error.Clear();
        return true;
    }

    if(property == "preset") {
        const String preset = value;
        if(!IsThemePresetName(preset)) {
            error = "Unknown theme preset: " + preset;
            return false;
        }
        target.preset = preset;
    }
    else if(property == "mode") {
        const String mode = value;
        if(!IsThemeModeName(mode)) {
            error = "Unknown theme mode: " + mode;
            return false;
        }
        target.mode = mode;
        target.SyncLegacyAccent();
    }
    else if(property == "accent") {
        Color color;
        if(!ParseThemeColor(value, color)) {
            error = "Invalid accent colour";
            return false;
        }
        target.GetPalette(target.UsesDarkPalette())
              .Set(target.roles.control_accent, color);
        target.SyncLegacyAccent();
    }
    else {
        bool palette_property = false;
        for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
            const String light_id = "palette.light." + AsString(i);
            const String dark_id = "palette.dark." + AsString(i);
            if(property == light_id || property == dark_id) {
                Color color;
                if(!ParseThemeColor(value, color)) {
                    error = "Invalid palette colour";
                    return false;
                }
                target.GetPalette(property == dark_id).Set(i, color);
                target.SyncLegacyAccent();
                palette_property = true;
                break;
            }
        }
        if(palette_property) {
            error.Clear();
            return true;
        }

        bool role_property = true;
        const int index = ThemePaletteIndex((int)value);
        if(property == "roles.control.standard") target.roles.control_standard = index;
        else if(property == "roles.control.subtle") target.roles.control_subtle = index;
        else if(property == "roles.control.accent") target.roles.control_accent = index;
        else if(property == "roles.control.alert") target.roles.control_alert = index;
        else if(property == "roles.panel.surface") target.roles.panel_surface = index;
        else if(property == "roles.panel.subtle") target.roles.panel_subtle = index;
        else if(property == "roles.panel.strong") target.roles.panel_strong = index;
        else role_property = false;
        if(role_property) {
            target.SyncLegacyAccent();
            error.Clear();
            return true;
        }

        if(property == "spacing") target.spacing = minmax((int)value, 0, 32);
        else if(property == "radius") target.radius = minmax((int)value, 0, 32);
        else if(property == "pill_radius") target.pill_radius = minmax((int)value, 0, 40);
        else if(property == "border_width") target.border_width = minmax((int)value, 0, 6);
        else if(property == "shadows") target.shadows = (bool)value;
        else if(property == "shadow_distance") target.shadow_distance = minmax((int)value, 0, 24);
        else if(property == "shadow_offset_y") target.shadow_offset_y = minmax((int)value, -12, 12);
        else if(property == "shadow_alpha") target.shadow_alpha = minmax((int)value, 0, 255);
        else {
            error = "Unknown theme property: " + property;
            return false;
        }
    }
    error.Clear();
    return true;
}

bool UiDesignerThemeDocument::Preview(
    const String& property, const Value& value, String& error)
{
    preview_ = preview_active_ ? preview_ : value_;
    if(!SetProperty(preview_, property, value, error))
        return false;
    preview_active_ = true;
    WhenPreview();
    return true;
}

void UiDesignerThemeDocument::TruncateRedo()
{
    while(history_.GetCount() > position_)
        history_.Remove(history_.GetCount() - 1);
    if(saved_position_ > position_)
        saved_position_ = -1;
}

bool UiDesignerThemeDocument::CommitSnapshot(
    const UiDesignerThemeSnapshot& after, const String& label, String& error)
{
    if(after.ToValue() == value_.ToValue()) {
        preview_ = value_;
        preview_active_ = false;
        error.Clear();
        return true;
    }

    TruncateRedo();
    UiDesignerThemeHistoryEntry& entry = history_.Add();
    entry.label = label.IsEmpty() ? "Edit theme" : label;
    entry.before = value_;
    entry.after = after;

    value_ = after;
    preview_ = value_;
    preview_active_ = false;
    position_ = history_.GetCount();
    WhenChanged();
    WhenHistoryChanged();
    error.Clear();
    return true;
}

bool UiDesignerThemeDocument::Commit(
    const String& property, const Value& value,
    const String& label, String& error)
{
    UiDesignerThemeSnapshot after = value_;
    if(!SetProperty(after, property, value, error))
        return false;
    return CommitSnapshot(after,
                          label.IsEmpty() ? "Set " + property : label,
                          error);
}

bool UiDesignerThemeDocument::CommitPalette(
    bool dark, const UiDesignerThemePalette& palette,
    const String& label, String& error)
{
    UiDesignerThemeSnapshot after = value_;
    after.GetPalette(dark) = palette;
    after.SyncLegacyAccent();
    return CommitSnapshot(after,
        label.IsEmpty()
            ? String("Set ") + (dark ? "Dark" : "Light") + " palette"
            : label,
        error);
}

void UiDesignerThemeDocument::CancelPreview()
{
    if(!preview_active_)
        return;
    preview_ = value_;
    preview_active_ = false;
    WhenPreview();
}

bool UiDesignerThemeDocument::Reset(
    const String& property, String& error)
{
    if(property.StartsWith("studio.")) {
        if(active_style_target_.IsEmpty()) {
            error = "Select a Theme Studio sample first";
            return false;
        }
        UiDesignerThemeSnapshot after = value_;
        after.RemoveStyleOverride(active_style_target_, property.Mid(7));
        return CommitSnapshot(after, "Reset " + property.Mid(7), error);
    }
    UiDesignerThemeSnapshot defaults;
    return Commit(property, GetProperty(defaults, property),
                  "Reset " + property, error);
}

bool UiDesignerThemeDocument::Undo()
{
    if(!CanUndo())
        return false;
    value_ = history_[position_ - 1].before;
    preview_ = value_;
    preview_active_ = false;
    position_--;
    WhenChanged();
    WhenHistoryChanged();
    return true;
}

bool UiDesignerThemeDocument::Redo()
{
    if(!CanRedo())
        return false;
    value_ = history_[position_].after;
    preview_ = value_;
    preview_active_ = false;
    position_++;
    WhenChanged();
    WhenHistoryChanged();
    return true;
}

bool UiDesignerThemeDocument::Replace(
    const UiDesignerThemeSnapshot& value, bool mark_saved)
{
    value_ = value;
    value_.SyncLegacyAccent();
    preview_ = value_;
    preview_active_ = false;
    history_.Clear();
    position_ = 0;
    saved_position_ = mark_saved ? 0 : -1;
    WhenChanged();
    WhenHistoryChanged();
    return true;
}

String UiDesignerThemeDocument::Serialize(bool pretty) const
{
    ValueMap root;
    root.Set("format", "upp-ui-theme-designer");
    root.Set("schema", 3);
    root.Set("theme", value_.ToValue());
    return AsJSON(root, pretty);
}

bool UiDesignerThemeDocument::Deserialize(
    const String& json, String& error)
{
    Value parsed = ParseJSON(json);
    if(IsError(parsed)) {
        error = GetErrorText(parsed);
        return false;
    }
    if(!parsed.Is<ValueMap>()) {
        error = "Theme root must be an object";
        return false;
    }
    ValueMap root = parsed;
    if((String)UiDesignerMapValue(root, "format", "") != "upp-ui-theme-designer") {
        error = "Unsupported theme document";
        return false;
    }
    const int schema = UiDesignerMapValue(root, "schema", 1);
    if(schema < 1 || schema > 3) {
        error = Format("Unsupported theme schema %d", schema);
        return false;
    }
    UiDesignerThemeSnapshot loaded;
    if(!loaded.FromValue(UiDesignerMapValue(root, "theme", ValueMap()), error))
        return false;
    value_ = loaded;
    preview_ = loaded;
    preview_active_ = false;
    history_.Clear();
    position_ = saved_position_ = 0;
    WhenChanged();
    WhenHistoryChanged();
    return true;
}

}
