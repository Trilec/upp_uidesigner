#include "UiDesignerWidgets.h"
#include <Ui/UiIcons.h>

namespace Upp {

UiBaseEdit::Style UiDesignerReadOnlyEditStyle()
{
    UiBaseEdit::Style style = UiTheme::ResolveEdit(UiTheme::GetContext(), UiRole::Standard);
    // Code viewers use the surrounding panel surface in both appearance modes.
    style.show_readonly_bg = false;
    style.metrics.face_enabled = true;
    const UiFill surface = UiTheme::ResolvePanel(UiPanelRole::Surface).palette.face[ST_NORMAL];
    for(int state = ST_NORMAL; state <= ST_DISABLED; ++state)
        style.palette.face[state] = surface;
    return style;
}

void UiDesignerCodeView::RefreshTheme()
{
    edit_.SetCustomStyle(UiDesignerReadOnlyEditStyle());
    copy_.SetCustomStyle(UiTheme::ResolveToolButton(UiRole::Subtle));
    fullscreen_.SetCustomStyle(UiTheme::ResolveToolButton(UiRole::Subtle));
    Refresh();
}

UiDesignerCodeView::UiDesignerCodeView()
{
    Add(edit_);
    Add(copy_);
    Add(fullscreen_);
    Add(build_);
    build_.SetText("Build...");
    build_.WhenAction = [=] { if(WhenBuild) WhenBuild(); };

    edit_.SetReadOnly();

    copy_.SetCustomStyle(UiTheme::ResolveToolButton(UiRole::Subtle));
    copy_.SetText("")
         .SetIcon(ICON_CONTENT_CONTENT_COPY_48())
         .SetIconSize(DPI(18), DPI(18))
         .SetContentInset(DPI(3))
         .SetContentGap(DPI(0))
         .SetAlign(UiAlign::CENTER, UiAlign::CENTER)
         .SetIconScaleToContent(false)
         .NoWantFocus();
    copy_.Tip("Copy all generated code");
    copy_.WhenAction = [=] { CopyAll(); };

    fullscreen_.SetCustomStyle(UiTheme::ResolveToolButton(UiRole::Subtle));
    fullscreen_.SetText("")
               .SetIcon(ICON_DESIGN_UNFOLD_MORE_48())
               .SetIconSize(DPI(18), DPI(18))
               .SetContentInset(DPI(3))
               .SetContentGap(DPI(0))
               .SetAlign(UiAlign::CENTER, UiAlign::CENTER)
               .SetIconScaleToContent(false)
               .NoWantFocus();
    fullscreen_.Tip("Open generated code (Escape closes the window)");
    fullscreen_.WhenAction = [=] { ShowFullscreen(); };
    RefreshTheme();
}

void UiDesignerCodeView::SetCode(const String& code)
{
    edit_.SetData(code);
}

String UiDesignerCodeView::GetCode() const
{
    return AsString(edit_.GetData());
}

void UiDesignerCodeView::Layout()
{
    const int toolbar_height = DPI(40);
    const int button_size = DPI(30);
    const int y = max(0, (toolbar_height - button_size) / 2);
    copy_.SetRect(DPI(6), y, button_size, button_size);
    fullscreen_.SetRect(DPI(42), y, button_size, button_size);
    build_.Show(bool(WhenBuild));
    build_.SetRect(DPI(78), y, DPI(90), button_size);
    edit_.SetRect(0, toolbar_height, GetSize().cx,
                  max(0, GetSize().cy - toolbar_height));
}

void UiDesignerCodeView::CopyAll()
{
    WriteClipboardText(GetCode());
}

void UiDesignerCodeView::ShowFullscreen()
{
    struct CodeWindow : TopWindow {
        bool Key(dword key, int count) override {
            if(key == K_ESCAPE) { Break(IDCANCEL); return true; }
            return TopWindow::Key(key, count);
        }
    } dialog;
    UiMultiEdit code;
    UiButton close;
    close.SetText("Close (Esc)");
    close.WhenAction = [&] { dialog.Break(IDCANCEL); };
    code.SetReadOnly();
    code.SetCustomStyle(UiDesignerReadOnlyEditStyle());
    code.SetData(GetCode());
    dialog.Title("Generated code").Sizeable().Zoomable();
    dialog.Add(code.HSizePos(8, 8).VSizePos(8, 48));
    dialog.Add(close.RightPos(8, 120).BottomPos(8, 32));
    dialog.SetRect(0, 0, DPI(1000), DPI(700));
    dialog.Maximize();
    dialog.Run();
}

}
