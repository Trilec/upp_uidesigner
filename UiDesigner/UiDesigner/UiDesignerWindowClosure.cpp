#include "UiDesignerWindow.h"
#include <Ui/UiIcons.h>
#include <UiDesigner/Services/UiDesignerAdvancedCatalog.h>

namespace Upp {

void UiDesignerWindow::Paint(Draw& w)
{
    const UiPanel::Style surface = UiTheme::ResolvePanel(UiPanelRole::Surface);
    const Rect outer = GetSize();
    if(outer.IsEmpty())
        return;
    const UiFill& fill = surface.palette.face[ST_NORMAL];
    w.DrawRect(outer, fill.IsSolid() && !IsNull(fill.color)
                      ? fill.color : SColorFace());
}

UiDesignerWindowClosureHook::UiDesignerWindowClosureHook(UiDesignerWindow& owner)
{
    Ptr<UiDesignerWindow> safe = &owner;
    PostCallback([safe] {
        if(!safe)
            return;
        UiDesignerWindow& window = *safe;

        // UiDesigner.rc embeds the application artwork as resource 5555.
        // Reuse that resource for both the native window and the header so
        // the application never substitutes the shared-library brand icon.
        const Image brand = Win32Icon(5555);
        window.Icon(brand);
        window.brand_.SetMedia(brand, Size(DPI(18), DPI(18)))
                     .SetMediaAlign(UiAlign::CENTER, UiAlign::CENTER)
                     .SetMediaGap(DPI(4))
                     .SetContentInset(DPI(2))
                     .ShowTitleLine(false)
                     .ShowCardLine(false);
        window.version_.ClearIcon();

        // Keep one compact identity block at the left: brand icon, Designer,
        // then version. The Accent title-card resolver can enable a card line,
        // so the final shell hook also owns the no-line presentation contract.
        window.header_layout_.PauseLayout();
        window.header_layout_.ClearItems();
        window.header_layout_.Add(window.brand_).Fixed(DPI(120)).MinCross(DPI(30));
        window.header_layout_.Add(window.version_).Fixed(DPI(82)).MinCross(DPI(24));
        window.header_layout_.Add(window.save_).Fixed(DPI(92)).MinCross(DPI(24));
        window.header_layout_.Add(window.load_).Fixed(DPI(92)).MinCross(DPI(24));
        window.header_layout_.Add(window.export_).Fixed(DPI(100)).MinCross(DPI(24));
        window.header_layout_.AddSpacer(1).Expand(1).MinMain(DPI(10));
        window.header_layout_.Add(window.designer_mode_).Fixed(DPI(92)).MinCross(DPI(24));
        window.header_layout_.Add(window.theme_mode_).Fixed(DPI(120)).MinCross(DPI(24));
        window.header_layout_.Add(window.theme_select_).Fixed(DPI(150)).MinCross(DPI(24));
        window.header_layout_.Add(window.dark_).Fixed(DPI(36)).MinCross(DPI(24));
        window.header_layout_.Add(window.help_).Fixed(DPI(36)).MinCross(DPI(24));
        window.header_layout_.Add(window.exit_).Fixed(DPI(36)).MinCross(DPI(24));
        window.header_layout_.ResumeLayout(true);

        const auto RefreshHeaderThemeChrome = [safe] {
            if(!safe)
                return;
            // Re-resolve application chrome after every theme revision, then
            // reassert the identity presentation that the Accent TitleCard
            // resolver must not override (no line, tight icon/title spacing).
            safe->theme_select_.SetCustomStyle(
                UiTheme::ResolveDropdown(UiRole::Standard));
            safe->brand_.SetCustomStyle(UiTheme::ResolveTitleCard(UiRole::Accent))
                        .ShowTitleLine(false)
                        .ShowCardLine(false)
                        .SetContentInset(DPI(2))
                        .SetMediaGap(DPI(4));
            safe->version_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Accent))
                          .ClearIcon();
            safe->header_layout_.Layout();
            safe->Refresh();
        };

        // Preset activation and preview dragging use the same location-aware
        // pipeline. Catalog rows deliberately carry the `preset:` envelope for
        // drag identity; click activation strips it before entering the preset
        // library. A blank document therefore accepts a preset at the Window
        // root without pretending that Window is a normal catalog control.
        window.presets_list_.WhenActivate = [safe](const String& id) {
            if(!safe)
                return;
            const String preset_id = id.StartsWith("preset:") ? id.Mid(7) : id;
            String error;
            UiDesignerNodeId created = 0;
            if(!safe->session_.InsertPresetAt(
                   preset_id, 0, Point(), false, -1, -1, -1,
                   &created, error))
                safe->RefreshStatus(error);
            else
                safe->RefreshStatus("Preset inserted");
        };
        window.presets_list_.WhenToolDrag = [safe](const String& id, Point screen) {
            if(safe)
                safe->TrackCatalogDrag(id, screen);
        };
        window.presets_list_.WhenToolDrop = [safe](const String& id, Point screen) {
            if(safe)
                safe->FinishCatalogDrag(id, screen);
        };
        window.presets_list_.WhenToolCancel = [safe] {
            if(safe)
                safe->CancelCatalogDrag();
        };

        const auto RefreshScalarData = [safe] {
            if(!safe)
                return;
            UiDesignerWindow& w = *safe;
            const UiDesignerNodeId selected = w.session_.State().selection.primary;
            const UiDesignerNode *node = selected
                ? w.session_.Document().Find(selected) : nullptr;
            const UiDesignerControlSpec *spec = node
                ? w.session_.Catalog().Find(node->type) : nullptr;
            if(!node || !spec ||
               spec->data_capability != UiDesignerDataCapability::Scalar)
                return;

            const bool prior_guard = w.data_projection_refreshing_;
            w.data_projection_refreshing_ = true;
            w.data_model_.Clear();
            w.data_model_.Add("Value", String("scalar:value"), true);
            w.data_list_.SetData(String("scalar:value"));
            w.data_selected_token_ = String("scalar:value");

            if(!UiDesignerBuildScalarDataPropertyModel(
                   *spec, *node, w.data_editor_model_)) {
                w.data_projection_refreshing_ = prior_guard;
                return;
            }
            w.data_editor_.SetModel(&w.data_editor_model_);

            w.data_add_.Enable(false);
            w.data_remove_.Enable(false);
            w.data_rename_.Enable(false);
            w.data_up_.Enable(false);
            w.data_down_.Enable(false);
            w.data_enable_.Enable(false);
            w.data_active_.Enable(false);
            w.data_select_content_.Enable(false);
            w.data_remove_content_.Enable(false);
            w.data_enable_.SetText("Enable");
            w.data_active_.SetText("Set Active");
            w.data_projection_refreshing_ = prior_guard;
        };

        // The normal Data handler deliberately ignores Scalar controls. Append
        // the scalar path so the canonical property continues through the same
        // Session command/undo/Preview/codegen pipeline.
        window.data_editor_.WhenCommit << [safe](const String& id,
                                                  const Value& value) {
            if(!safe || id != "value")
                return;
            UiDesignerWindow& w = *safe;
            const UiDesignerNodeId selected = w.session_.State().selection.primary;
            const UiDesignerNode *node = selected
                ? w.session_.Document().Find(selected) : nullptr;
            const UiDesignerControlSpec *spec = node
                ? w.session_.Catalog().Find(node->type) : nullptr;
            if(!node || !spec ||
               spec->data_capability != UiDesignerDataCapability::Scalar)
                return;
            String error;
            if(!w.session_.CommitProperty("value", value, error))
                w.RefreshStatus(error);
        };
        window.data_editor_.WhenReset << [safe](const String& id) {
            if(!safe || id != "value")
                return;
            UiDesignerWindow& w = *safe;
            const UiDesignerNodeId selected = w.session_.State().selection.primary;
            const UiDesignerNode *node = selected
                ? w.session_.Document().Find(selected) : nullptr;
            const UiDesignerControlSpec *spec = node
                ? w.session_.Catalog().Find(node->type) : nullptr;
            if(!node || !spec ||
               spec->data_capability != UiDesignerDataCapability::Scalar)
                return;
            String error;
            if(!w.session_.ResetProperty("value", error))
                w.RefreshStatus(error);
        };

        window.session_.WhenSelectionChanged << [RefreshScalarData] {
            RefreshScalarData();
            // UiDesignerWindow posts its normal selection-details batch first;
            // queue Scalar after that batch so its generic RefreshData cannot
            // overwrite the specialized projection.
            PostCallback(RefreshScalarData);
        };
        window.session_.Document().WhenChanged <<
            [RefreshScalarData](const UiDesignerChangeSet&) {
                RefreshScalarData();
            };
        // The base list-selection callback refreshes the generic Data pane;
        // re-project Scalar immediately afterwards so clicking its single row
        // cannot fall back to the unsupported-data message.
        window.data_list_.WhenSelection << RefreshScalarData;

        const auto RefreshCatalogPainting = [safe] {
            if(!safe)
                return;
            safe->layouts_list_.Refresh();
            safe->containers_list_.Refresh();
            safe->controls_list_.Refresh();
            safe->presets_list_.Refresh();
            safe->upp_controls_list_.Refresh();
        };
        window.session_.Theme().WhenChanged << RefreshCatalogPainting;
        window.session_.Theme().WhenPreviewChanged << RefreshCatalogPainting;
        window.session_.Theme().WhenChanged << RefreshHeaderThemeChrome;
        window.session_.Theme().WhenPreviewChanged << RefreshHeaderThemeChrome;

        RefreshScalarData();
        RefreshCatalogPainting();
        RefreshHeaderThemeChrome();
    });
}

}
