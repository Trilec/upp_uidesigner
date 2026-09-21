#ifndef _ChartRingIntegrationTest_Fixture_h_
#define _ChartRingIntegrationTest_Fixture_h_

#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Services/UiDesignerExport.h>
#include <UiDesigner/Core/UiDesignerChartRingData.h>

using namespace Upp;

inline ValueArray ChartFixtureSegments()
{
    ValueArray rows;
    rows.Add(UiDesignerChartRingSegmentValue(12.5, "Quoted \"label\"\nnext\\line", Color(12, 34, 56)));
    rows.Add(UiDesignerChartRingSegmentValue(7.5, "Automatic"));
    rows.Add(UiDesignerChartRingSegmentValue(0, ""));
    return rows;
}

inline bool BuildChartFixture(UiDesignerSession& session, UiDesignerNodeId& chart,
                               UiDesignerNodeId& empty, String& error)
{
    session.NewDocument("blank");
    const UiDesignerNodeId box = session.AddControl("UiBoxLayout");
    chart = session.AddControl("UiChartRing", box);
    empty = session.AddControl("UiChartRing", box);
    if(!box || !chart || !empty) {
        error = "Unable to create ChartRing fixture";
        return false;
    }
    session.Select(empty);
    if(!session.CommitProperty("segments", ValueArray(), error)) return false;
    session.Select(chart);
    if(!session.CommitProperty("role", "Accent", error) ||
       !session.CommitProperty("segments", ChartFixtureSegments(), error) ||
       !session.CommitProperty("explicit_total", 40.0, error) ||
       !session.CommitProperty("center_text", "Chart fixture", error)) return false;
    auto& theme = session.Theme();
    if(!theme.Commit("preset", "Pill", "Fixture preset", error) ||
       !theme.Commit("mode", "Dark", "Fixture mode", error)) return false;
    theme.SetActiveStyleTarget("Dark|control|UiChartRing|Accent");
    if(!theme.Commit("studio.series.1", Color(18, 90, 140), "Fixture series", error)) return false;
    return session.Catalog().ValidateDocument(session.Document(), error);
}

inline UiDesignerExportRequest ChartFixtureRequest(const String& destination)
{
    UiDesignerExportRequest request;
    request.profile = UiDesignerExportProfile::CompleteCppPackage;
    request.destination = destination;
    request.generation.package_name = "ChartRingFixture";
    request.generation.class_name = "ChartRingWindow";
    request.generation.namespace_name = "Upp";
    request.write.overwrite = UiDesignerOverwritePolicy::ReplaceGenerated;
    request.write.preserve_user_files = true;
    return request;
}

inline String ChartVerifierSource(const String& member, const String& empty_member)
{
    // Links only generated source + Ui, and checks real controls from a foreign
    // CWD. No Designer services or runtime theme.json lookup is involved.
    String code;
    code << "#include \"ChartRingWindow.h\"\nusing namespace Upp;\n"
         << "class ChartRingVerifier : public ChartRingWindow {\npublic:\n"
         << "bool Check() {\n"
         << "auto& chart = " << member << ";\n"
         << "auto& empty = " << empty_member << ";\n"
         << "if(chart.GetSegmentCount() != 3 || empty.GetSegmentCount() != 0) return false;\n"
         << "if(chart.GetDataSum() != 20 || chart.GetTotal() != 40 || chart.GetExplicitTotal() != 40) return false;\n"
         << "if(chart.GetCenterText() != \"Chart fixture\" || !empty.GetCenterText().IsEmpty()) return false;\n"
         << "if(chart.GetSegment(0).value != 12.5 || chart.GetSegment(1).value != 7.5 || chart.GetSegment(2).value != 0) return false;\n"
         << "if(chart.GetSegment(0).label != \"Quoted \\\"label\\\"\\nnext\\\\line\") return false;\n"
         << "if(!chart.GetSegment(2).label.IsEmpty()) return false;\n"
         << "if(chart.GetSegment(0).color != Color(12, 34, 56) || !IsNull(chart.GetSegment(1).color)) return false;\n"
         << "if(chart.GetRole() != UiRole::Accent || chart.GetStyle().series[1] != Color(18, 90, 140)) return false;\n"
         << "auto geometry = chart.GetGeometry(Size(160, 160));\n"
         << "return geometry.remainder == 20 && geometry.segments.GetCount() == 3 && geometry.segments[1].color == Color(18, 90, 140);\n"
         << "}\n};\n"
         << "GUI_APP_MAIN { ChartRingVerifier v; bool ok = v.Check();\n"
         << "Cout() << \"CHARTRING_GENERATED_RUNTIME failed=\" << (ok ? 0 : 1) << '\\n'; SetExitCode(ok ? 0 : 1); }\n";
    return code;
}

inline bool ExportChartFixture(const String& root, String& error)
{
    UiDesignerSession session;
    UiDesignerNodeId chart = 0, empty = 0;
    if(!BuildChartFixture(session, chart, empty, error)) return false;
    UiDesignerExportService exporter(session.Catalog());
    const String member = session.Document().Find(chart)->name + "_n" + AsString(chart);
    const String empty_member = session.Document().Find(empty)->name + "_n" + AsString(empty);
    const String verifier = ChartVerifierSource(member, empty_member);
    for(bool component : {false, true}) {
        const String folder = AppendFileName(root, component ? "Component" : "ChartRingFixture");
        auto request = ChartFixtureRequest(folder);
        if(component) request.profile = UiDesignerExportProfile::ComponentOnly;
        auto result = exporter.Execute(session.Document(), session.Theme(), request);
        if(!result.success) { error = result.diagnostic; return false; }
        if(component && (FileExists(AppendFileName(folder, "main.cpp")) ||
                         FileExists(AppendFileName(folder, "ChartRingFixture.upp")))) {
            error = "ComponentOnly unexpectedly owns an entry point or package";
            return false;
        }
        if(!SaveFile(AppendFileName(folder, "Verify.cpp"), verifier)) {
            error = "Unable to write generated runtime verifier";
            return false;
        }
        const String verify_folder = component ? folder : AppendFileName(root, "ChartRingVerify");
        RealizeDirectory(verify_folder);
        const String prefix = component ? String() : String("../ChartRingFixture/");
        String manifest;
        manifest << "uses CtrlLib, Ui;\nfile\n    " << prefix << "ChartRingWindow.generated.cpp,\n    "
                 << prefix << "ChartRingWindow.cpp,\n    " << prefix << "Verify.cpp;\n"
                 << "mainconfig \"\" = \"GUI\";\n";
        if(!SaveFile(AppendFileName(verify_folder,
                                  component ? "Component.upp" : "ChartRingVerify.upp"), manifest)) {
            error = "Unable to write generated verifier package";
            return false;
        }
    }
    error.Clear();
    return true;
}

#endif
