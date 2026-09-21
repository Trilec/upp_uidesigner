#ifndef _DateTimeIntegrationTest_Fixture_h_
#define _DateTimeIntegrationTest_Fixture_h_

#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Services/UiDesignerExport.h>
#include <UiDesigner/Core/UiDesignerDateTimeData.h>

using namespace Upp;

inline bool BuildDateTimeFixture(UiDesignerSession& session,
                                 Vector<UiDesignerNodeId>& fields, String& error)
{
    session.NewDocument("blank");
    fields.Clear();
    const UiDesignerNodeId box = session.AddControl("UiBoxLayout");
    if(!box) { error = "Cannot create DateTime fixture layout"; return false; }
    for(int i = 0; i < 4; ++i) {
        const UiDesignerNodeId id = session.AddControl("UiDateTime", box);
        if(!id) { error = "Cannot create DateTime fixture control"; return false; }
        fields.Add(id);
        session.Select(id);
        if(!session.CommitProperty("datetime_value", "2024-02-29T23:47:58", error) ||
           !session.CommitProperty("role", "Accent", error)) return false;
    }
    session.Select(fields[0]);
    if(!session.CommitProperty("mode", "Date", error)) return false;
    session.Select(fields[1]);
    if(!session.CommitProperty("mode", "Time", error) ||
       !session.CommitProperty("show_seconds", false, error)) return false;
    session.Select(fields[2]);
    if(!session.CommitProperty("minimum_value", "2024-01-01T00:00:00", error) ||
       !session.CommitProperty("maximum_value", "2025-12-31T23:59:59", error) ||
       !session.CommitProperty("allow_null", false, error) ||
       !session.CommitProperty("allow_copy", false, error) ||
       !session.CommitProperty("allow_paste", false, error) ||
       !session.CommitProperty("presentation_frame", true, error) ||
       !session.CommitProperty("editable", false, error) ||
       !session.CommitProperty("first_day", 0, error)) return false;
    session.Select(fields[3]);
    if(!session.CommitProperty("datetime_value", Value(), error)) return false;
    auto& theme = session.Theme();
    if(!theme.Commit("preset", "Pill", "Fixture preset", error) ||
       !theme.Commit("mode", "Dark", "Fixture mode", error)) return false;
    theme.SetActiveStyleTarget("Dark|control|UiDateTime|Accent");
    if(!theme.Commit("studio.editable_face.normal", Color(25, 45, 65), "Fixture face", error)) return false;
    session.Select(fields[0]);
    if(!session.CommitThemeOverride("editable_face.normal", Color(71, 81, 91), error)) return false;
    return session.Catalog().ValidateDocument(session.Document(), error);
}

inline String DateTimeVerifierSource(const Vector<String>& members)
{
    String code;
    code << "#include \"DateTimeWindow.h\"\nusing namespace Upp;\n"
         << "class DateTimeVerifier : public DateTimeWindow { public: bool Check() {\n";
    for(int i = 0; i < members.GetCount(); ++i)
        code << "auto& f" << i << " = " << members[i] << ";\n";
    code << "if(f0.GetMode() != UiDateTimeMode::Date || f0.GetValue() != Time(2024,2,29)) return false;\n"
         << "if(f1.GetMode() != UiDateTimeMode::Time || f1.GetValue() != Time(1970,1,1,23,47,0) || f1.IsSecondsShown()) return false;\n"
         << "if(f2.GetMode() != UiDateTimeMode::DateTime || f2.GetValue() != Time(2024,2,29,23,47,58)) return false;\n"
         << "if(f2.GetMinimum() != Time(2024,1,1) || f2.GetMaximum() != Time(2025,12,31,23,59,59)) return false;\n"
         << "if(f2.IsNullAllowed() || f2.IsValueEditable() || f2.IsCopyAllowed() || f2.IsPasteAllowed()) return false;\n"
         << "if(!f2.IsPresentationFrameShown() || f2.GetFirstDayOfWeek() != 0 || !f3.IsNullValue()) return false;\n"
         << "if(f0.GetStyle().editable.palette.face[ST_NORMAL].color != Color(71,81,91)) return false;\n"
         << "if(f2.GetStyle().editable.palette.face[ST_NORMAL].color != Color(25,45,65)) return false;\n"
         << "if(f2.GetStyle().editable.show_readonly_bg || f2.GetStyle().presentation.show_readonly_bg) return false;\n"
         << "return f0.GetDisplayText() == \"2024-02-29\" && f1.GetDisplayText() == \"23:47\" && f2.GetDisplayText() == \"2024-02-29T23:47:58\";\n"
         << "} };\nGUI_APP_MAIN { bool ok; { DateTimeVerifier fixture; ok = fixture.Check(); }\n"
         << "Cout() << \"DATETIME_GENERATED_RUNTIME failed=\" << (ok ? 0 : 1) << '\\n'; SetExitCode(ok ? 0 : 1); }\n";
    return code;
}

inline bool ExportDateTimeFixture(const String& root, String& error)
{
    UiDesignerSession session;
    Vector<UiDesignerNodeId> fields;
    if(!BuildDateTimeFixture(session, fields, error)) return false;
    Vector<String> members;
    for(UiDesignerNodeId id : fields)
        members.Add(session.Document().Find(id)->name + "_n" + AsString(id));
    const String verifier = DateTimeVerifierSource(members);
    UiDesignerExportService exporter(session.Catalog());
    for(bool component : {false, true}) {
        const String folder = AppendFileName(root, component ? "Component" : "DateTimeFixture");
        UiDesignerExportRequest request;
        request.destination = folder;
        request.profile = component ? UiDesignerExportProfile::ComponentOnly : UiDesignerExportProfile::CompleteCppPackage;
        request.generation.package_name = "DateTimeFixture";
        request.generation.class_name = "DateTimeWindow";
        request.generation.namespace_name = "Upp";
        request.write.overwrite = UiDesignerOverwritePolicy::ReplaceGenerated;
        request.write.preserve_user_files = true;
        const auto result = exporter.Execute(session.Document(), session.Theme(), request);
        if(!result.success) { error = result.diagnostic; return false; }
        if(component && (FileExists(AppendFileName(folder, "main.cpp")) ||
                         FileExists(AppendFileName(folder, "DateTimeFixture.upp")))) {
            error = "Component export unexpectedly owns an application entry point";
            return false;
        }
        if(!SaveFile(AppendFileName(folder, "Verify.cpp"), verifier)) {
            error = "Cannot write DateTime runtime verifier"; return false;
        }
        const String verify_folder = component ? folder : AppendFileName(root, "DateTimeVerify");
        RealizeDirectory(verify_folder);
        const String prefix = component ? String() : String("../DateTimeFixture/");
        String manifest;
        manifest << "uses CtrlLib, Ui;\nfile\n    " << prefix << "DateTimeWindow.generated.cpp,\n    "
                 << prefix << "DateTimeWindow.cpp,\n    " << prefix << "Verify.cpp;\n"
                 << "mainconfig \"\" = \"GUI\";\n";
        if(!SaveFile(AppendFileName(verify_folder, component ? "Component.upp" : "DateTimeVerify.upp"), manifest)) {
            error = "Cannot write DateTime verifier manifest"; return false;
        }
    }
    error.Clear();
    return true;
}

#endif
