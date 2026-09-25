#include "UiDesignerWidgets.h"
#include <UiDesigner/Theme/UiDesignerThemeBuilderV2.h>

namespace Upp {

// The fixed filter and scrolling rows share this boundary for paint, input,
// resize and wheel limits. Scrolled rows must never paint into the filter lane.
static int CatalogRowsTop() { return DPI(48); }

UiDesignerCatalogList::UiDesignerCatalogList()
{
    BackPaint();
    Add(filter_edit_);
    filter_edit_.SetPlaceholder("Filter controls...");
    filter_edit_.WhenChange = [=] {
        filter_ = AsString(filter_edit_.GetData());
        RebuildMatches();
        WhenFilter(filter_);
    };
}

void UiDesignerCatalogList::SetCatalog(const UiDesignerCatalog *catalog)
{
    catalog_ = catalog;
    RebuildMatches();
}

void UiDesignerCatalogList::SetCategory(const String& category)
{
    category_ = category;
    presets_ = false;
    RebuildMatches();
}

void UiDesignerCatalogList::SetPresets(bool on)
{
    presets_ = on;
    filter_edit_.SetPlaceholder(on ? "Filter presets..." : "Filter controls...");
    RebuildMatches();
}

void UiDesignerCatalogList::SetFilter(const String& filter)
{
    filter_ = filter;
    filter_edit_.SetData(filter);
    RebuildMatches();
}

void UiDesignerCatalogList::RebuildMatches()
{
    matches_.Clear();
    if(catalog_) {
        const String needle = ToLower(TrimBoth(filter_));
        if(presets_) {
            for(int i = 0; i < catalog_->GetPresets().GetCount(); i++) {
                const UiDesignerPreset& preset = catalog_->GetPresets()[i];
                if(needle.IsEmpty() ||
                   ToLower(preset.display_name).Find(needle) >= 0 ||
                   ToLower(preset.help).Find(needle) >= 0)
                    matches_.Add(i);
            }
        }
        else
            matches_ = catalog_->Search(filter_, category_.IsEmpty() ? "All" : category_);
    }
    hover_ = -1;
    selected_ = matches_.IsEmpty() ? -1 : minmax(selected_, 0, matches_.GetCount() - 1);
    scroll_ = 0;
    Refresh();
}

int UiDesignerCatalogList::Count() const
{
    return matches_.GetCount();
}

String UiDesignerCatalogList::ItemId(int index) const
{
    if(!catalog_ || index < 0 || index >= matches_.GetCount())
        return String();
    const int source = matches_[index];
    return presets_ ? "preset:" + catalog_->GetPresets()[source].id
                    : catalog_->GetControls()[source].type_id;
}

String UiDesignerCatalogList::ItemLabel(int index) const
{
    if(!catalog_ || index < 0 || index >= matches_.GetCount())
        return String();
    const int source = matches_[index];
    return presets_ ? catalog_->GetPresets()[source].display_name
                    : catalog_->GetControls()[source].display_name;
}

String UiDesignerCatalogList::ItemHelp(int index) const
{
    if(!catalog_ || index < 0 || index >= matches_.GetCount())
        return String();
    const int source = matches_[index];
    return presets_ ? catalog_->GetPresets()[source].help
                    : catalog_->GetControls()[source].help;
}

Image UiDesignerCatalogList::ItemIcon(int index) const
{
    if(!catalog_ || index < 0 || index >= matches_.GetCount())
        return Image();
    const int source = matches_[index];
    return presets_
        ? UiDesignerResolveCatalogIcon(catalog_->GetPresets()[source].icon_key)
        : UiDesignerResolveCatalogIcon(catalog_->GetControls()[source].icon_key);
}

Rect UiDesignerCatalogList::ItemRect(int index) const
{
    const int top = CatalogRowsTop();
    const int row = DPI(42);
    const int inset = DPI(6);
    return RectC(inset, top + index * row - scroll_,
                 max(0, GetSize().cx - inset * 2), row);
}

int UiDesignerCatalogList::RowAt(Point p) const
{
    if(p.y < CatalogRowsTop() || p.y >= GetSize().cy || p.x < 0 || p.x >= GetSize().cx)
        return -1;
    const int index = (p.y - CatalogRowsTop() + scroll_) / DPI(42);
    return index >= 0 && index < Count() ? index : -1;
}

int UiDesignerCatalogList::GetContentHeight() const
{
    return Count() * DPI(42);
}

void UiDesignerCatalogList::Layout()
{
    filter_edit_.SetRect(DPI(6), 0, max(0, GetSize().cx - DPI(12)), DPI(34));
    scroll_ = minmax(scroll_, 0, max(0, GetContentHeight() - max(0, GetSize().cy - CatalogRowsTop())));
}

void UiDesignerCatalogList::Paint(Draw& w)
{
    const UiDesignerThemeSurfacePalette palette =
        UiDesignerResolveThemeSurfacePalette();
    w.DrawRect(GetSize(), palette.paper);
    w.Clip(0, CatalogRowsTop(), GetSize().cx, max(0, GetSize().cy - CatalogRowsTop()));
    for(int i = 0; i < Count(); i++) {
        Rect r = ItemRect(i);
        if(r.bottom <= CatalogRowsTop() || r.top >= GetSize().cy)
            continue;
        const bool current = i == selected_;
        Color face = current
            ? Blend(palette.accent, palette.paper, 72)
            : i == hover_ ? Blend(palette.accent, palette.paper, 32)
            : (i & 1 ? Blend(palette.alternate, palette.paper, 96)
                     : palette.paper);
        w.DrawRect(r, face);
        Image icon = ItemIcon(i);
        if(!icon.IsEmpty())
            w.DrawImage(r.left + DPI(10), r.top + DPI(11), DPI(18), DPI(18), icon);
        w.DrawText(r.left + DPI(38), r.top + DPI(7), ItemLabel(i),
                   SansSerifZ(11).Bold(current), palette.ink);
        const String help = ItemHelp(i);
        if(!help.IsEmpty())
            w.DrawText(r.left + DPI(38), r.top + DPI(23),
                       help.Left(54), SansSerifZ(8), palette.disabled);
        w.DrawLine(r.left, r.bottom - 1, r.right, r.bottom - 1, 1,
                   palette.divider);
    }
    if(Count() == 0)
        w.DrawText(DPI(12), CatalogRowsTop() + DPI(8), "No matching controls", SansSerifZ(10),
                   palette.disabled);
    w.End();
}

void UiDesignerCatalogList::Activate(int index)
{
    if(index >= 0 && index < Count())
        WhenActivate(ItemId(index));
}

void UiDesignerCatalogList::LeftDown(Point p, dword)
{
    pressed_ = selected_ = RowAt(p);
    drag_type_ = pressed_ >= 0 ? ItemId(pressed_) : String();
    drag_start_ = GetMousePos();
    drag_armed_ = pressed_ >= 0 && !drag_type_.IsEmpty();
    dragging_ = false;
    if(drag_armed_ && !HasCapture())
        SetCapture();
    SetFocus();
    Refresh();
}

void UiDesignerCatalogList::LeftUp(Point p, dword)
{
    const int pressed = pressed_;
    const String type = drag_type_;
    const bool was_dragging = dragging_;
    const bool was_armed = drag_armed_;
    const Point screen = GetMousePos();
    const int index = RowAt(p);
    pressed_ = -1;
    dragging_ = false;
    drag_armed_ = false;
    drag_type_.Clear();
    if(HasCapture())
        ReleaseCapture();
    if(was_dragging && was_armed && !type.IsEmpty())
        WhenToolDrop(type, screen);
    else if(pressed >= 0 && index == pressed)
        Activate(index);
}

void UiDesignerCatalogList::LeftDouble(Point p, dword)
{
    Activate(RowAt(p));
}

void UiDesignerCatalogList::LeftDrag(Point, dword)
{
    if(pressed_ < 0 || pressed_ >= Count())
        return;
    if(!drag_armed_ && !drag_type_.IsEmpty()) {
        drag_armed_ = true;
        if(!HasCapture())
            SetCapture();
    }
    MouseMove(GetMousePos() - GetScreenRect().TopLeft(), K_MOUSELEFT);
}

void UiDesignerCatalogList::MouseMove(Point p, dword)
{
    if(drag_armed_) {
        if(!HasCapture())
            SetCapture();
        if(!dragging_ && Length(GetMousePos() - drag_start_) >= DPI(5))
            dragging_ = true;
        if(dragging_ && WhenToolDrag)
            WhenToolDrag(drag_type_, GetMousePos());
        return;
    }
    const int next = RowAt(p);
    if(next != hover_) {
        hover_ = next;
        Tip(next >= 0 ? ItemHelp(next) : String());
        Refresh();
    }
}

void UiDesignerCatalogList::MouseLeave()
{
    hover_ = -1;
    Tip(String());
    Refresh();
}

Image UiDesignerCatalogList::CursorImage(Point p, dword flags)
{
    return dragging_ ? Image::SizeAll() : ParentCtrl::CursorImage(p, flags);
}

void UiDesignerCatalogList::CancelMode()
{
    const bool active = drag_armed_ || dragging_;
    pressed_ = -1;
    dragging_ = false;
    drag_armed_ = false;
    drag_type_.Clear();
    if(active && WhenToolCancel)
        WhenToolCancel();
    ParentCtrl::CancelMode();
}

void UiDesignerCatalogList::MouseWheel(Point, int zdelta, dword)
{
    const int list_height = max(0, GetSize().cy - CatalogRowsTop());
    const int maximum = max(0, GetContentHeight() - list_height);
    scroll_ = minmax(scroll_ - zdelta / 4, 0, maximum);
    Refresh();
}

bool UiDesignerCatalogList::Key(dword key, int)
{
    if(key == K_UP && Count()) {
        selected_ = max(0, selected_ - 1);
        Refresh();
        return true;
    }
    if(key == K_DOWN && Count()) {
        selected_ = min(Count() - 1, selected_ + 1);
        Refresh();
        return true;
    }
    if(key == K_ENTER && selected_ >= 0) {
        Activate(selected_);
        return true;
    }
    if(key == K_CTRL_F) {
        filter_edit_.SetFocus();
        return true;
    }
    return ParentCtrl::Key(key, 1);
}

}
