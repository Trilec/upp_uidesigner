#include "Fixture.h"
#include <Ui/UiDateTime.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Services/UiDesignerAdvancedCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>
#include <UiDesigner/Editors/UiDesignerDateTimeEditor.h>
#include <Utilities/PropertyEditor/PropertyValueEditors.h>
#include <limits>

using namespace Upp;

namespace {
struct Tests {
    int checks = 0;
    int failed = 0;
    void Check(bool ok, const String& label)
    {
        ++checks;
        if(!ok) { ++failed; Cout() << "FAIL: " << label << '\n'; }
    }
};

void Run(Tests& t)
{
    String error;
    Time decoded = Time(1999, 3, 4, 5, 6, 7);
    t.Check(UiDesignerReadDateTimeValue("2024-02-29T23:47:58", decoded, error) &&
            decoded == Time(2024, 2, 29, 23, 47, 58), "strict parser accepts leap day");
    for(const char* invalid : {"2023-02-29T12:00:00", "2024-13-01T00:00:00",
                              "2024-01-01T24:00:00", "2024-01-01T00:60:00",
                              "2024-01-01T00:00:60", "2024-01-01T00:00:00Z",
                              "2024-01-01 00:00:00", "2024-01-01T00:00:00+12:00"}) {
        const Time before = decoded;
        t.Check(!UiDesignerReadDateTimeValue(invalid, decoded, error) && decoded == before,
                "invalid timestamp leaves destination intact: " + String(invalid));
    }
    t.Check(!UiDesignerReadDateTimeValue(123, decoded, error), "numeric timestamp is not silently coerced");
    t.Check(UiDesignerReadDateTimeValue(Value(), decoded, error) && IsNull(decoded), "null is explicit");
    t.Check(UiDesignerDateTimeValue(Time(2024, 2, 29, 23, 47, 58)) == "2024-02-29T23:47:58", "ISO encoding is deterministic");

    // The initialization unit must be retained by the linker without a GUI shell hook.
    t.Check(PropertyEditorFactory::Global().HasCustom("designer.date-time.value"), "DateTime editor automatically registered");
    UiDesignerSession session;
    Vector<UiDesignerNodeId> ids;
    const bool built = BuildDateTimeFixture(session, ids, error);
    t.Check(built, "build authored fixture: " + error);
    if(!built) return;
    const UiDesignerControlSpec* spec = session.Catalog().Find("UiDateTime");
    t.Check(spec && spec->runtime_kind == UiDesignerRuntimeKind::UiDateTime, "explicit runtime kind");
    if(!spec || !spec->FindProperty("datetime_value")) return;
    t.Check(spec->preview && spec->codegen && spec->theme && spec->inspector, "complete registered authoring path");
    t.Check(spec->FindProperty("datetime_value") && spec->FindProperty("datetime_value")->preserve_null, "value preserves authored null");
    t.Check(spec->data_property_id == "datetime_value" && spec->data_capability == UiDesignerDataCapability::Scalar, "Data binds canonical value");
    t.Check(session.Catalog().Validate(error), "catalog validates: " + error);
    t.Check(spec->FindEvent("WhenAction") && spec->FindEvent("WhenChanging") && spec->FindEvent("WhenOpenPicker"), "supported no-argument events");
    for(const char* field : {"mode", "show_seconds", "datetime_value", "minimum_value", "maximum_value", "allow_null", "editable"})
        t.Check(!spec->FindThemeOverride(field), "Theme cannot own " + String(field));

    session.Select(ids[3]);
    t.Check(IsNull(session.InspectorModel().Find("datetime_value")->value), "Inspector retains explicit null instead of default");
    PropertyEditorModel data;
    t.Check(UiDesignerBuildScalarDataPropertyModel(*spec, *session.Document().Find(ids[3]), data) &&
            data.Find("datetime_value") && IsNull(data.Find("datetime_value")->value), "Data retains explicit null through spec copy");
    t.Check(session.Document().Find(ids[3])->data.IsEmpty(), "no second persistent data payload");
    if(!data.Find("datetime_value")) return;
    auto editor = PropertyEditorFactory::Global().Create(*data.Find("datetime_value"));
    t.Check((bool)editor, "real property editor constructs");
    if(editor) {
        editor->Configure(*data.Find("datetime_value"));
        editor->SetEditorValue(Value(), false);
        t.Check(IsNull(editor->GetEditorValue()), "editor round-trips null");
        auto* native = dynamic_cast<UiDateTime*>(editor->GetFirstChild());
        t.Check(native && native->IsNullValue(), "editor contains real nullable DateTime control");
        if(native) {
            int commits = 0;
            Value accepted;
            editor->WhenCommit = [&](Value value) { ++commits; accepted = value; };
            native->SetValue(Time(2025, 3, 4, 5, 6, 7), true);
            t.Check(commits == 1 && accepted == "2025-03-04T05:06:07", "native picker edit commits one ISO value");
            auto* apply = dynamic_cast<UiButton*>(editor->GetLastChild());
            native->SetValue(Time(2025, 5, 6, 7, 8, 9), false);
            if(apply) apply->WhenAction();
            t.Check(apply && commits == 2 && accepted == "2025-05-06T07:08:09",
                    "explicit Apply commits live typed value even without native final event");
            editor->WhenCommit.Clear();
        }
    }

    UiDesignerPreviewCanvas canvas;
    canvas.SetRect(0, 0, 640, 480);
    UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark);
    canvas.SetRuntimeTheme(session.Theme().Get());
    session.AttachProjection(&canvas);
    auto Get = [&](int index) { return dynamic_cast<UiDateTime*>(canvas.FindRuntime(ids[index])); };
    t.Check(Get(0) && Get(0)->GetValue() == Time(2024, 2, 29), "Date Preview clears time components");
    t.Check(Get(1) && Get(1)->GetValue() == Time(1970, 1, 1, 23, 47), "Time Preview anchors date and hides seconds");
    t.Check(Get(2) && Get(2)->GetValue() == Time(2024, 2, 29, 23, 47, 58), "DateTime Preview preserves all components");
    t.Check(Get(3) && Get(3)->IsNullValue(), "null Preview never becomes current clock");
    t.Check(Get(2) && !Get(2)->IsValueEditable() && !Get(2)->IsNullAllowed() &&
            !Get(2)->IsCopyAllowed() && !Get(2)->IsPasteAllowed(), "configuration reaches real Preview");
    const String authored = AsString(session.Document().Find(ids[1])->GetProperty("datetime_value"));
    session.Select(ids[1]);
    t.Check(session.CommitProperty("show_seconds", true, error) && Get(1) &&
            Get(1)->GetValue() == Time(1970, 1, 1, 23, 47, 58), "seconds restored from canonical source");
    t.Check(session.CommitProperty("mode", "DateTime", error) && Get(1) &&
            Get(1)->GetValue() == Time(2024, 2, 29, 23, 47, 58), "date restored on mode switch");
    t.Check(session.Document().Find(ids[1])->GetProperty("datetime_value") == authored, "mode/seconds never rewrite authored value");
    t.Check(session.Undo() && Get(1) && Get(1)->GetMode() == UiDateTimeMode::Time, "mode undo updates Preview");
    t.Check(session.Redo() && Get(1) && Get(1)->GetMode() == UiDateTimeMode::DateTime, "mode redo updates Preview");
    t.Check(session.CommitProperty("format_style", "Locale", error) &&
            session.CommitProperty("clock_format", "Hour12", error) && Get(1) &&
            Get(1)->GetValue() == Time(2024, 2, 29, 23, 47, 58), "display format does not mutate value");

    const auto* theme = UiDesignerGetThemeAdapter(*spec);
    t.Check(theme && theme->Supports(spec->runtime_kind), "DateTime Theme adapter resolves");
    if(theme && Get(2)) {
        const Time before = Get(2)->GetValue();
        for(const auto& property : spec->theme_overrides)
            t.Check(theme->HasField(property.adapter_field_id), "supported appearance field " + property.id);
        theme->ApplyPreviewStyle(*Get(2), *session.Document().Find(ids[2]), *spec);
        t.Check(Get(2)->GetValue() == before && !Get(2)->IsValueEditable(), "Theme reapply preserves value and configuration");
        t.Check(!Get(2)->GetStyle().editable.show_readonly_bg &&
                !Get(2)->GetStyle().presentation.show_readonly_bg, "read-only DateTime style stays themed");
    }
    session.Select(ids[3]);
    t.Check(session.CommitProperty("datetime_value", "2026-01-02T03:04:05", error) &&
            session.Undo() && IsNull(session.Document().Find(ids[3])->GetProperty("datetime_value")), "undo restores null");
    t.Check(session.Redo() && session.Document().Find(ids[3])->GetProperty("datetime_value") == "2026-01-02T03:04:05", "redo restores exact timestamp");
    t.Check(session.CommitProperty("datetime_value", Value(), error), "clear canonical value");
    UiDesignerDocument loaded;
    t.Check(UiDesignerDeserialize(UiDesignerSerialize(session.Document(), true), loaded, error) &&
            loaded.Find(ids[1])->GetProperty("datetime_value") == authored &&
            IsNull(loaded.Find(ids[3])->GetProperty("datetime_value")), "JSON preserves ISO and null");

    UiDesignerCodeGenerator generator(session.Catalog());
    auto generated = generator.Generate(session.Document(), "DateTimeProof");
    t.Check(generated.IsValid() && generated.generated_header.Find("UiDateTime") >= 0, "DateTime source generates");
    t.Check(generated.generated_source.Find("Time(2024, 2, 29, 23, 47, 58)") >= 0 &&
            generated.generated_source.Find(".SetValue(Time(Null))") >= 0, "generated setup uses native deterministic Time/null");
    t.Check(generated.generated_source.Find(".SetNow(") < 0 &&
            generated.generated_source.Find("UiDesignerDateTime") < 0, "generated runtime has no Designer helper or current-clock initialization");
    session.AttachProjection(nullptr);

    // Invalid external property writes are rejected at canonical validation/export;
    // generic command transport is not silently changed into a new schema engine.
    UiDesignerNode* invalid = loaded.Find(ids[3]);
    if(invalid) {
        invalid->SetProperty("allow_null", false);
        t.Check(!session.Catalog().ValidateDocument(loaded, error), "null plus required value cannot export");
        invalid->SetProperty("allow_null", true);
        invalid->SetProperty("minimum_value", "not-a-date");
        t.Check(!generator.Generate(loaded, "InvalidDateTime").IsValid(), "invalid bound cannot export");
        invalid->SetProperty("minimum_value", Value());
        const int value_index = invalid->properties.Find("datetime_value");
        if(value_index >= 0) invalid->properties.Remove(value_index);
        const auto absent = generator.Generate(loaded, "AbsentDateTime");
        t.Check(absent.IsValid() && absent.generated_source.Find("Time(2000, 1, 1, 12, 0, 0)") >= 0,
                "absent property uses same deterministic default as Preview");
        invalid->SetProperty("first_day", std::numeric_limits<double>::quiet_NaN());
        t.Check(!UiDesignerValidateDateTimeNode(*invalid, error), "non-finite weekday rejected before integer cast");
    }
    // Inspector display normalization must not silently replace hidden source data.
    session.Select(ids[0]);
    PropertyEditorModel model;
    UiDesignerBuildScalarDataPropertyModel(*spec, *session.Document().Find(ids[0]), model);
    auto date_editor = PropertyEditorFactory::Global().Create(*model.Find("datetime_value"));
    if(date_editor) {
        date_editor->Configure(*model.Find("datetime_value"));
        date_editor->SetEditorValue("2024-02-29T23:47:58", false);
        t.Check(date_editor->GetEditorValue() == "2024-02-29T23:47:58", "untouched Date-only editor preserves hidden time");
    }
    else t.Check(false, "Date-only property editor constructs");
}
}

GUI_APP_MAIN
{
    const auto& args = CommandLine();
    if(args.GetCount() == 2 && args[0] == "--export-fixture") {
        String error;
        const bool ok = ExportDateTimeFixture(args[1], error);
        if(!ok) Cout() << "FAIL: " << error << '\n';
        SetExitCode(ok ? 0 : 1);
        return;
    }
    Tests tests;
    Run(tests);
    Cout() << "DATETIME_INTEGRATION checks=" << tests.checks << " failed=" << tests.failed << '\n';
    SetExitCode(tests.failed ? 1 : 0);
}
