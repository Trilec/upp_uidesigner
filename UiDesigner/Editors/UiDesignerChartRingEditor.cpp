#include "UiDesignerChartRingEditor.h"

namespace Upp {

bool UiDesignerChartRingDraft::Set(const Value& value, String& error)
{
    return UiDesignerReadChartRingSegments(value, segments_, error);
}

ValueArray UiDesignerChartRingDraft::GetValue() const
{
    return UiDesignerChartRingSegmentsValue(segments_);
}

bool UiDesignerChartRingDraft::Append(double value, const String& label,
                                     Color color, String& error)
{
    ValueArray candidate = GetValue();
    candidate.Add(UiDesignerChartRingSegmentValue(value, label, color));
    return Set(candidate, error);
}

bool UiDesignerChartRingDraft::Replace(int index, double value, const String& label,
                                      Color color, String& error)
{
    if(index < 0 || index >= segments_.GetCount()) {
        error = "No segment is selected";
        return false;
    }
    ValueArray candidate;
    for(int i = 0; i < segments_.GetCount(); ++i) {
        const auto& row = segments_[i];
        candidate.Add(i == index
            ? UiDesignerChartRingSegmentValue(value, label, color)
            : UiDesignerChartRingSegmentValue(row.value, row.label, row.color));
    }
    return Set(candidate, error);
}

bool UiDesignerChartRingDraft::Remove(int index)
{
    if(index < 0 || index >= segments_.GetCount()) return false;
    segments_.Remove(index);
    return true;
}

bool UiDesignerChartRingDraft::Move(int from, int to)
{
    if(from < 0 || to < 0 || from >= segments_.GetCount() || to >= segments_.GetCount())
        return false;
    const int step = to > from ? 1 : -1;
    for(int i = from; i != to; i += step)
        Swap(segments_[i], segments_[i + step]);
    return true;
}

namespace {

class ChartRingSegmentsDialog : public TopWindow {
public:
    ChartRingSegmentsDialog()
    {
        Title("Chart Ring segments").Sizeable();
        SetRect(0, 0, DPI(700), DPI(420));
        Add(list_);
        Add(label_caption_); Add(label_);
        Add(value_caption_); Add(value_);
        Add(custom_color_);
        Add(hint_); Add(status_);
        Add(add_); Add(remove_); Add(up_); Add(down_);
        Add(apply_); Add(cancel_);
        label_caption_.SetText("Label");
        value_caption_.SetText("Proportional value");
        custom_color_.SetText("Use custom colour");
        hint_.SetText("Automatic colours follow the Theme. Apply commits one edit; Cancel discards it.");
        value_.Min(0).NotNull().Precision(12);
        list_.EnableInternalMutation(false).EnableDragReorder(false)
             .EnableRenameOnDblClick(false);

        PropertyEditorItem color_item;
        color_item.id = "color";
        color_item.label = "Colour";
        color_item.kind = PropertyEditorKind::Color;
        color_item.value = SColorHighlight();
        color_ = PropertyEditorFactory::Global().Create(color_item);
        if(color_) {
            color_->Configure(color_item);
            Add(*color_);
        }

        list_.WhenSelection = [=] {
            if(loading_) return;
            const int next = list_.GetCursor();
            if(next == selected_) return;
            if(!StoreSelected()) {
                loading_ = true;
                list_.SetCursor(selected_);
                if(selected_ >= 0) list_.Select(selected_);
                loading_ = false;
                return;
            }
            RefreshRows(next);
        };
        custom_color_.WhenAction = [=] {
            if(color_) color_->Enable(selected_ >= 0 && (bool)custom_color_.GetData());
        };
        add_.SetText("Add");
        add_.WhenAction = [=] {
            if(!StoreSelected()) return;
            String error;
            if(!draft_.Append(1.0, "Segment " + AsString(draft_.GetCount() + 1), Null, error)) {
                status_.SetText(error);
                return;
            }
            RefreshRows(draft_.GetCount() - 1);
        };
        remove_.SetText("Remove");
        remove_.WhenAction = [=] {
            // Removing the selected row intentionally discards its uncommitted fields.
            if(draft_.Remove(selected_))
                RefreshRows(min(selected_, draft_.GetCount() - 1));
        };
        up_.SetText("Move up");
        down_.SetText("Move down");
        up_.WhenAction = [=] { MoveSelected(-1); };
        down_.WhenAction = [=] { MoveSelected(1); };
        apply_.SetText("Apply");
        apply_.WhenAction = [=] { if(StoreSelected()) Break(IDOK); };
        cancel_.SetText("Cancel");
        cancel_.WhenAction = [=] { Break(IDCANCEL); };
        WhenClose = [=] { Break(IDCANCEL); };
    }

    bool SetValue(const Value& value, String& error)
    {
        if(!draft_.Set(value, error)) return false;
        RefreshRows(draft_.GetCount() ? 0 : -1);
        return true;
    }
    ValueArray GetValue() const { return draft_.GetValue(); }

    void Layout() override
    {
        const int m = DPI(10), gap = DPI(6), h = DPI(28);
        const Size size = GetSize();
        const int left = max(0, min(DPI(240), size.cx / 3));
        const int right_x = m + left + gap;
        const int right_w = max(0, size.cx - right_x - m);
        const int bottom = max(m, size.cy - m - h);
        const int tools_y = max(m, bottom - h * 2 - gap * 3);
        list_.SetRect(m, m, left, max(0, tools_y - m - gap));
        const int half = max(0, (left - gap) / 2);
        add_.SetRect(m, tools_y, half, h);
        remove_.SetRect(m + half + gap, tools_y, half, h);
        up_.SetRect(m, tools_y + h + gap, half, h);
        down_.SetRect(m + half + gap, tools_y + h + gap, half, h);
        int y = m;
        label_caption_.SetRect(right_x, y, right_w, h); y += h;
        label_.SetRect(right_x, y, right_w, h); y += h + gap;
        value_caption_.SetRect(right_x, y, right_w, h); y += h;
        value_.SetRect(right_x, y, right_w, h); y += h + gap;
        custom_color_.SetRect(right_x, y, right_w, h); y += h + gap;
        if(color_) color_->SetRect(right_x, y, right_w, h);
        y += h + gap;
        hint_.SetRect(right_x, y, right_w, h * 2);
        status_.SetRect(m, max(m, bottom - h - gap), max(0, size.cx - m * 2), h);
        cancel_.SetRect(max(m, size.cx - m - DPI(90)), bottom, DPI(90), h);
        apply_.SetRect(max(m, size.cx - m - DPI(186)), bottom, DPI(90), h);
    }

    bool Key(dword key, int count) override
    {
        if(key == K_ESCAPE) { Break(IDCANCEL); return true; }
        if(key == K_CTRL_ENTER) { if(StoreSelected()) Break(IDOK); return true; }
        return TopWindow::Key(key, count);
    }

private:
    bool StoreSelected()
    {
        if(selected_ < 0 || selected_ >= draft_.GetCount()) return true;
        double number = draft_.Get(selected_).value;
        const String number_text = value_.GetText().ToString();
        // Detect actual field changes rather than one particular callback:
        // UiFloatEdit's spin/wheel commits and typed edits take different paths.
        // Untouched formatted fields must not round the original stored double.
        if(number_text != number_text_ && !value_.TryGetValue(number)) {
            status_.SetText("Enter a complete non-negative number before continuing.");
            return false;
        }
        const String label_text = AsString(label_.GetData());
        // A single-line field may normalize a loaded multiline label. Preserve
        // the full original label unless the user actually changes its field.
        const String label = label_text == label_text_ ? draft_.Get(selected_).label : label_text;
        Color color = Null;
        if((bool)custom_color_.GetData()) {
            const Value selected_color = color_ ? color_->GetEditorValue() : Value();
            if(!selected_color.Is<Color>() || IsNull(selected_color)) {
                status_.SetText("Choose a custom colour, or turn off custom colour.");
                return false;
            }
            color = (Color)selected_color;
        }
        String error;
        if(!draft_.Replace(selected_, number, label, color, error)) {
            status_.SetText(error);
            return false;
        }
        number_text_ = number_text;
        label_text_ = label_text;
        status_.SetText(String());
        return true;
    }

    void RefreshRows(int select)
    {
        loading_ = true;
        list_.Model().Clear();
        for(int i = 0; i < draft_.GetCount(); ++i) {
            const auto& segment = draft_.Get(i);
            UiModelItem row(segment.label.IsEmpty()
                ? "Segment " + AsString(i + 1) : segment.label, i);
            row.right_text = Format("%.12g", segment.value);
            list_.Model().Add(row);
        }
        selected_ = select >= 0 && select < draft_.GetCount() ? select : -1;
        list_.SetCursor(selected_);
        if(selected_ >= 0) {
            list_.Select(selected_);
            list_.ScrollTo(selected_);
            const auto& segment = draft_.Get(selected_);
            label_.SetData(segment.label);
            value_.SetValue(segment.value);
            custom_color_.SetData(!IsNull(segment.color));
            if(color_) color_->SetEditorValue(
                IsNull(segment.color) ? SColorHighlight() : segment.color, false);
        }
        else {
            label_.SetData(String());
            value_.SetValue(0);
            custom_color_.SetData(false);
        }
        label_.Enable(selected_ >= 0);
        value_.Enable(selected_ >= 0);
        custom_color_.Enable(selected_ >= 0);
        if(color_) color_->Enable(selected_ >= 0 && (bool)custom_color_.GetData());
        remove_.Enable(selected_ >= 0);
        up_.Enable(selected_ > 0);
        down_.Enable(selected_ >= 0 && selected_ + 1 < draft_.GetCount());
        number_text_ = value_.GetText().ToString();
        label_text_ = AsString(label_.GetData());
        loading_ = false;
        status_.SetText(String());
    }

    void MoveSelected(int delta)
    {
        if(!StoreSelected()) return;
        const int next = selected_ + delta;
        if(draft_.Move(selected_, next)) RefreshRows(next);
    }

    UiDesignerChartRingDraft draft_;
    UiList list_;
    UiLabel label_caption_, value_caption_, hint_, status_;
    UiLineEdit label_;
    UiFloatEdit value_;
    UiCheckBox custom_color_;
    One<PropertyValueEditor> color_;
    UiButton add_, remove_, up_, down_, apply_, cancel_;
    int selected_ = -1;
    bool loading_ = false;
    String number_text_, label_text_;
};

class ChartRingValueEditor final : public PropertyValueEditor {
public:
    ChartRingValueEditor()
    {
        Add(edit_.SizePos());
        edit_.WhenAction = [=] { Edit(); };
    }
    void Configure(const PropertyEditorItem& item) override
    {
        edit_.Enable(item.enabled && !item.read_only && item.value_editable);
    }
    void SetEditorValue(const Value& value, bool mixed) override
    {
        value_ = value;
        mixed_ = mixed;
        Vector<UiDesignerChartRingSegment> rows;
        String error;
        const bool valid = UiDesignerReadChartRingSegments(value, rows, error);
        edit_.SetText(mixed ? "Mixed segments - edit..." :
                      valid ? AsString(rows.GetCount()) + " segments - edit..." :
                              String("Invalid segments"));
        edit_.Tip(valid ? "Edit the complete ordered collection" : error);
    }
    Value GetEditorValue() const override { return value_; }
    void FocusEditor() override { edit_.SetFocus(); }

private:
    void Edit()
    {
        Ptr<ChartRingValueEditor> alive = this;
        const Value original = value_;
        const bool original_mixed = mixed_;
        ChartRingSegmentsDialog dialog;
        String error;
        if(!dialog.SetValue(original, error)) {
            PromptOK(DeQtf(error));
            return;
        }
        const int result = dialog.Execute();
        if(!alive || result != IDOK) return;
        // Do not overwrite an externally refreshed property while the dialog ran.
        if(value_ != original || mixed_ != original_mixed) return;
        const Value accepted = dialog.GetValue();
        if(!mixed_ && accepted == original) return;
        SetEditorValue(accepted, false);
        // This callback can rebuild/destroy the editor. Do not touch members after it.
        WhenCommit(accepted);
    }
    UiButton edit_;
    Value value_;
    bool mixed_ = false;
};

}

void RegisterUiDesignerChartRingEditor()
{
    auto& factory = PropertyEditorFactory::Global();
    if(!factory.HasCustom("designer.chart-ring.segments"))
        factory.RegisterCustom("designer.chart-ring.segments", [] {
            return One<PropertyValueEditor>(new ChartRingValueEditor);
        });
}

}
