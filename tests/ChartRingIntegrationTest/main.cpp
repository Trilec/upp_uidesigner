#include "Fixture.h"
#include <UiDesigner/Editors/UiDesignerChartRingEditor.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>
#include <UiDesigner/Services/UiDesignerRuntimeTheme.h>
#include <Ui/UiChartRing.h>
#include <limits>

namespace {
struct ChartTests {
    int checks = 0;
    int failed = 0;
    void Check(bool ok, const String& label)
    {
        ++checks;
        if(!ok) { ++failed; Cout() << "FAIL: " << label << '\n'; }
    }
};

void TestChartData(ChartTests& t)
{
    String error;
    Vector<UiDesignerChartRingSegment> rows;
    t.Check(UiDesignerReadChartRingSegments(ValueArray(), rows, error) && rows.IsEmpty(),
            "empty segment collection is valid");
    const Value original = ChartFixtureSegments();
    t.Check(UiDesignerReadChartRingSegments(original, rows, error) && rows.GetCount() == 3,
            "ordered typed segments decode");
    t.Check(rows.GetCount() == 3 && rows[0].value == 12.5 && rows[1].value == 7.5 &&
            rows[2].value == 0 && rows[0].color == Color(12, 34, 56) && IsNull(rows[1].color),
            "values, zero rows, custom and automatic colours remain distinct");
    auto Reject = [&](const Value& bad, const char* reason) {
        const Value before = UiDesignerChartRingSegmentsValue(rows);
        t.Check(!UiDesignerReadChartRingSegments(bad, rows, error) && !error.IsEmpty() &&
                Value(UiDesignerChartRingSegmentsValue(rows)) == before, reason);
    };
    Reject("not an array", "non-array data rejects transactionally");
    ValueArray bad;
    bad.Add(7);
    Reject(bad, "non-object row rejects transactionally");
    for(const Value& invalid : {Value(-1.0), Value("12"), Value(true),
                               Value(std::numeric_limits<double>::infinity()),
                               Value(std::numeric_limits<double>::quiet_NaN())}) {
        ValueMap row;
        row.Set("value", invalid);
        bad.Clear(); bad.Add(row);
        Reject(bad, "invalid numeric segment rejects without clamping/coercion");
    }
    ValueMap row;
    row.Set("value", 2.0); row.Set("color", "red");
    bad.Clear(); bad.Add(row);
    Reject(bad, "invalid colour rejects");
    row.Set("color", Value()); row.Set("label", 42);
    bad.Clear(); bad.Add(row);
    Reject(bad, "invalid label rejects");
    row.Set("label", "valid"); row.Set("unknown", true);
    bad.Clear(); bad.Add(row);
    Reject(bad, "unknown segment fields are not silently discarded");
    bad.Clear();
    bad.Add(UiDesignerChartRingSegmentValue(std::numeric_limits<double>::max(), "A"));
    bad.Add(UiDesignerChartRingSegmentValue(std::numeric_limits<double>::max(), "B"));
    Reject(bad, "overflowing segment sum rejects");

    UiDesignerChartRingDraft draft;
    t.Check(draft.Set(original, error), "dialog draft copies source");
    t.Check(draft.Move(0, 2) && draft.Get(0).value == 7.5 && draft.Get(2).value == 12.5,
            "reorder moves the entire record and preserves intervening order");
    t.Check(draft.Replace(0, 9.0, "Replaced", Null, error) &&
            draft.Get(0).label == "Replaced" && IsNull(draft.Get(0).color), "draft row updates");
    const Value before_bad = draft.GetValue();
    t.Check(!draft.Replace(0, -5.0, "Bad", Null, error) && Value(draft.GetValue()) == before_bad,
            "invalid edit preserves draft");
    t.Check(!draft.Move(-1, 0) && !draft.Remove(99), "invalid draft indices are safe");
    t.Check(draft.Append(4, "Added", Color(9, 8, 7), error) && draft.Remove(1) && draft.GetCount() == 3,
            "draft add and remove preserve whole records");
    t.Check(original == Value(ChartFixtureSegments()), "discarding a draft leaves original data untouched");
    while(draft.GetCount()) draft.Remove(0);
    t.Check(draft.GetValue().IsEmpty(), "all segments can be removed");
}

void TestChartIntegration(ChartTests& t)
{
    String error;
    RegisterUiDesignerChartRingEditor();
    UiDesignerSession session;
    UiDesignerNodeId chart = 0, empty = 0;
    const bool built = BuildChartFixture(session, chart, empty, error);
    t.Check(built, "authoring fixture builds and validates: " + error);
    if(!built) return;
    const auto* spec = session.Catalog().Find("UiChartRing");
    t.Check(spec && spec->runtime_kind == UiDesignerRuntimeKind::UiChartRing &&
            spec->preview && spec->codegen && spec->inspector && spec->theme,
            "ChartRing advertises a complete supported authoring path");
    if(!spec) return;
    t.Check(spec->events.IsEmpty() && !spec->FindProperty("value") &&
            spec->data_property_id == "segments", "chart is not scalar progress or an invented action control");
    t.Check(session.Catalog().Validate(error), "catalog including ChartRing validates: " + error);
    t.Check(!spec->FindThemeOverride("segments") && !spec->FindThemeOverride("explicit_total") &&
            !spec->FindThemeOverride("center_text"), "theme does not own chart data/configuration");

    PropertyEditorModel data;
    t.Check(UiDesignerBuildScalarDataPropertyModel(*spec, *session.Document().Find(chart), data) &&
            data.Find("segments") && !data.Find("value") &&
            data.Find("segments")->value == Value(ChartFixtureSegments()),
            "Data and Inspector project the same authored segment collection");
    const PropertyEditorItem* item = session.InspectorModel().Find("segments");
    t.Check(item && item->custom_editor == "designer.chart-ring.segments", "Inspector uses registered segment editor");
    if(item) {
        auto editor = PropertyEditorFactory::Global().Create(*item);
        t.Check((bool)editor, "real segment property editor constructs");
        if(editor) {
            int commits = 0;
            editor->WhenCommit = [&](const Value&) { ++commits; };
            editor->Configure(*item);
            editor->SetEditorValue(item->value, false);
            t.Check(editor->GetEditorValue() == item->value && commits == 0,
                    "editor refresh preserves values without fabricating a user commit");
        }
    }

    const Value source = session.Document().Find(chart)->GetProperty("segments");
    const int history = session.Commands().GetHistoryPosition();
    ValueArray replacement; replacement.Add(UiDesignerChartRingSegmentValue(3, "Only"));
    t.Check(session.CommitProperty("segments", replacement, error) &&
            session.Commands().GetHistoryPosition() == history + 1, "accepted collection uses one command-history entry");
    t.Check(session.Undo() && session.Document().Find(chart)->GetProperty("segments") == source,
            "undo restores exact segment collection");
    t.Check(session.Redo() && session.Document().Find(chart)->GetProperty("segments") == Value(replacement),
            "redo reapplies collection");
    t.Check(session.ResetProperty("segments", error) &&
            session.Document().Find(chart)->GetProperty("segments") == spec->FindProperty("segments")->default_value,
            "reset restores catalog collection");
    t.Check(session.CommitProperty("segments", source, error), "restore authored fixture");

    UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark);
    UiDesignerPreviewCanvas canvas;
    canvas.SetCatalog(&session.Catalog());
    canvas.SetDocument(&session.Document());
    canvas.SetRuntimeTheme(session.Theme().Get());
    canvas.RebuildDocument();
    auto* runtime = dynamic_cast<UiChartRing*>(canvas.FindRuntime(chart));
    t.Check(runtime && runtime->GetSegmentCount() == 3 && runtime->GetDataSum() == 20 &&
            runtime->GetTotal() == 40 && runtime->GetCenterText() == "Chart fixture",
            "Preview constructs the actual chart with authored data/configuration");
    t.Check(runtime && runtime->GetSegment(0).color == Color(12, 34, 56) &&
            IsNull(runtime->GetSegment(1).color) && runtime->GetStyle().series[1] == Color(18, 90, 140),
            "Preview preserves custom colour and resolves automatic colour through Theme recipe");
    const auto* adapter = UiDesignerGetThemeAdapter(*spec);
    t.Check(adapter && adapter->Supports(spec->runtime_kind), "ChartRing Theme adapter resolves");
    if(adapter) {
        for(const auto& field : spec->theme_overrides)
            t.Check(adapter->HasField(field.adapter_field_id), "registered Theme field is backed: " + field.id);
    }
    t.Check(session.CommitThemeOverride("series.1", Color(140, 20, 80), error), "local series override commits");
    canvas.RebuildDocument();
    runtime = dynamic_cast<UiChartRing*>(canvas.FindRuntime(chart));
    t.Check(runtime && runtime->GetStyle().series[1] == Color(140, 20, 80), "local colour overrides inherited recipe");
    t.Check(session.SetThemeOverrideActive("series.1", false, error), "local series override disables");
    canvas.RebuildDocument();
    runtime = dynamic_cast<UiChartRing*>(canvas.FindRuntime(chart));
    t.Check(runtime && runtime->GetStyle().series[1] == Color(18, 90, 140) &&
            session.Document().Find(chart)->GetProperty("segments") == source &&
            runtime->GetExplicitTotal() == 40, "disabled override inherits again without changing data or total");
    if(runtime) {
        t.Check(UiDesignerPreviewFactory::Apply(*runtime, *spec, "segments", ValueArray()) !=
                    UiDesignerApplyResult::Rejected && runtime->GetSegmentCount() == 0,
                "Preview clears an empty collection instead of retaining old segments");
        t.Check(UiDesignerPreviewFactory::Apply(*runtime, *spec, "segments", "invalid") ==
                    UiDesignerApplyResult::Rejected && runtime->GetSegmentCount() == 0,
                "Preview rejects malformed input without partial mutation");
    }

    UiDesignerDocument loaded;
    t.Check(UiDesignerDeserialize(UiDesignerSerialize(session.Document(), true), loaded, error),
            "chart document round-trips: " + error);
    Vector<UiDesignerChartRingSegment> decoded;
    t.Check(loaded.Find(chart) && UiDesignerReadChartRingSegments(
                loaded.Find(chart)->GetProperty("segments"), decoded, error) &&
            decoded.GetCount() == 3 && decoded[0].color == Color(12, 34, 56) &&
            IsNull(decoded[1].color) && decoded[0].label == "Quoted \"label\"\nnext\\line",
            "persistence preserves typed colours, automatic colour and escaped labels");

    UiDesignerCodeGenerator generator(session.Catalog());
    auto generated = generator.Generate(session.Document(), "ChartRingWindow");
    t.Check(generated.IsValid(), "generated chart document validates: " + Join(generated.diagnostics, " | "));
    t.Check(generated.generated_header.Find("UiChartRing") >= 0 &&
            generated.generated_source.Find(".AddSegment(12.5,") >= 0 &&
            generated.generated_source.Find("Color(12, 34, 56)") >= 0 &&
            generated.generated_source.Find(".AddSegment(7.5, \"Automatic\", Null)") >= 0 &&
            generated.generated_source.Find(".AddSegment(0, \"\", Null)") >= 0 &&
            generated.generated_source.Find(".ClearCenterText()") >= 0,
            "generator uses collection API and preserves automatic/empty values");
    t.Check(generated.generated_source.Find(".SetData(") < 0 &&
            generated.generated_source.Find("Quoted \\\"label\\\"\\nnext\\\\line") >= 0,
            "no generic no-op SetData; text is escaped as C++");
    UiDesignerDocument corrupt;
    corrupt.ReplaceFrom(session.Document(), "Invalid fixture", false);
    corrupt.Find(chart)->properties.Set("segments", "bad");
    auto rejected = generator.Generate(corrupt, "RejectedChart");
    t.Check(!rejected.IsValid() && rejected.generated_source.IsEmpty() &&
            Join(rejected.diagnostics, " ").Find("ChartRing") >= 0,
            "malformed external/imported chart cannot export as a partial success");

    const String temp = AppendFileName(GetTempPath(), "uid-chart-" + AsString(Uuid::Create()));
    UiDesignerExportService exporter(session.Catalog());
    auto request = ChartFixtureRequest(temp);
    auto exported = exporter.Execute(session.Document(), session.Theme(), request);
    t.Check(exported.success, "complete themed chart package exports: " + exported.diagnostic);
    if(exported.success) {
        const String generated_path = AppendFileName(temp, "ChartRingWindow.generated.cpp");
        const String text = LoadFile(generated_path);
        t.Check(text.Find("UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark)") >= 0 &&
                text.Find("Color(18, 90, 140)") >= 0 && text.Find("Color(140, 20, 80)") < 0 &&
                text.Find("theme.json") < 0, "export compiles inherited Theme recipe without a CWD dependency");
        const String user_path = AppendFileName(temp, "ChartRingWindow.cpp");
        const String sentinel = LoadFile(user_path) + "\n// ChartRing user preservation sentinel\n";
        t.Check(SaveFile(user_path, sentinel), "write preserved user sentinel");
        exported = exporter.Execute(session.Document(), session.Theme(), request);
        t.Check(exported.success && LoadFile(user_path) == sentinel, "re-export preserves user implementation");
        request.profile = UiDesignerExportProfile::ComponentOnly;
        request.destination = AppendFileName(temp, "component");
        request.generation.include_theme = false;
        exported = exporter.Execute(session.Document(), session.Theme(), request);
        t.Check(exported.success && !FileExists(AppendFileName(request.destination, "main.cpp")) &&
                !FileExists(AppendFileName(request.destination, "ChartRingFixture.upp")) &&
                !FileExists(AppendFileName(request.destination, "theme.json")) &&
                LoadFile(AppendFileName(request.destination, "ChartRingWindow.generated.cpp")).Find(
                    "Color(18, 90, 140)") >= 0, "ComponentOnly omits app ownership but retains compiled Theme");
    }
    DeleteFolderDeep(temp);
}
}

GUI_APP_MAIN
{
    const auto& args = CommandLine();
    if(!args.IsEmpty()) {
        String error;
        const bool ok = args.GetCount() == 2 && args[0] == "--export-fixture" &&
                        ExportChartFixture(args[1], error);
        if(!ok) Cout() << "FAIL export fixture: " << error << '\n';
        SetExitCode(ok ? 0 : 1);
        return;
    }
    ChartTests tests;
    TestChartData(tests);
    TestChartIntegration(tests);
    Cout() << "CHARTRING_INTEGRATION checks=" << tests.checks << " failed=" << tests.failed << '\n';
    SetExitCode(tests.failed ? 1 : 0);
}
