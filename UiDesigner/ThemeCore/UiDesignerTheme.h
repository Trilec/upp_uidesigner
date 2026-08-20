#ifndef _Utilities_UiDesigner_Theme_UiDesignerTheme_h_
#define _Utilities_UiDesigner_Theme_UiDesignerTheme_h_

#include <Utilities/PropertyEditorCore/PropertyEditorCore.h>
#include <UiDesigner/Core/UiDesignerCore.h>

namespace Upp {

enum { UI_DESIGNER_THEME_PALETTE_SIZE = 6 };

struct UiDesignerThemePalette {
    Color colors[UI_DESIGNER_THEME_PALETTE_SIZE] = {
        Color(248, 250, 252), Color(255, 255, 255), Color(226, 232, 240),
        Color(15, 23, 42), Color(58, 132, 255), Color(220, 38, 38)
    };

    Color Get(int index) const;
    void Set(int index, Color color);
    ValueArray ToValue() const;
    bool FromValue(const Value& value, String& error);
};

struct UiDesignerThemeRoleAssignments {
    int control_standard = 1;
    int control_subtle = 2;
    int control_accent = 4;
    int control_alert = 5;

    int panel_surface = 1;
    int panel_subtle = 0;
    int panel_strong = 2;

    ValueMap ToValue() const;
    bool FromValue(const Value& value, String& error);
};

struct UiDesignerThemeSnapshot {
    UiDesignerThemeSnapshot();

    String preset = "Minimal";
    String mode = "Light";

    // Compatibility projection used by the existing Designer shell while the
    // runtime semantic-palette bridge is introduced. The Light/Dark palette
    // and role assignment remain authoritative.
    Color accent = Color(58, 132, 255);

    UiDesignerThemePalette light_palette;
    UiDesignerThemePalette dark_palette;
    UiDesignerThemeRoleAssignments roles;

    int spacing = 8;
    int radius = 8;
    int pill_radius = 25;
    int border_width = 1;
    bool shadows = true;
    int shadow_distance = 6;
    int shadow_offset_y = 2;
    int shadow_alpha = 24;

    const UiDesignerThemePalette& GetPalette(bool dark) const
    {
        return dark ? dark_palette : light_palette;
    }
    UiDesignerThemePalette& GetPalette(bool dark)
    {
        return dark ? dark_palette : light_palette;
    }
    bool UsesDarkPalette() const { return mode == "Dark"; }
    Color GetActiveAccent() const;
    void SyncLegacyAccent();

    ValueMap ToValue() const;
    bool FromValue(const Value& value, String& error);
};

struct UiDesignerThemeHistoryEntry {
    String label;
    UiDesignerThemeSnapshot before;
    UiDesignerThemeSnapshot after;
};

class UiDesignerThemeDocument {
public:
    typedef UiDesignerThemeDocument CLASSNAME;

    UiDesignerThemeDocument();

    const UiDesignerThemeSnapshot& Get() const { return value_; }
    const UiDesignerThemeSnapshot& GetEffective() const
    {
        return preview_active_ ? preview_ : value_;
    }

    void BuildPropertyModel(PropertyEditorModel& model) const;
    bool Preview(const String& property, const Value& value, String& error);
    bool Commit(const String& property, const Value& value,
                const String& label, String& error);
    bool CommitPalette(bool dark, const UiDesignerThemePalette& palette,
                       const String& label, String& error);
    void CancelPreview();
    bool Reset(const String& property, String& error);

    bool Undo();
    bool Redo();
    bool CanUndo() const { return position_ > 0; }
    bool CanRedo() const { return position_ < history_.GetCount(); }
    bool IsDirty() const { return position_ != saved_position_; }
    void MarkSaved() { saved_position_ = position_; }

    String Serialize(bool pretty = true) const;
    bool Deserialize(const String& json, String& error);
    bool Replace(const UiDesignerThemeSnapshot& value, bool mark_saved = true);

    Event<> WhenPreview;
    Event<>& WhenPreviewChanged = WhenPreview;
    Event<> WhenChanged;
    Event<> WhenHistoryChanged;

private:
    bool SetProperty(UiDesignerThemeSnapshot& target,
                     const String& property, const Value& value,
                     String& error) const;
    Value GetProperty(const UiDesignerThemeSnapshot& source,
                      const String& property) const;
    void TruncateRedo();

    UiDesignerThemeSnapshot value_;
    UiDesignerThemeSnapshot preview_;
    bool preview_active_ = false;

    Array<UiDesignerThemeHistoryEntry> history_;
    int position_ = 0;
    int saved_position_ = 0;
};

}

#endif
