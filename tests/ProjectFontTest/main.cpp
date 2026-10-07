#include <UiDesigner/Fonts/UiDesignerFonts.h>
#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Services/UiDesignerExport.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>
#include <Utilities/PropertyEditor/PropertyValueEditors.h>
using namespace Upp;
GUI_APP_MAIN {
    int checks = 0, failures = 0;
    String log, error;
    auto Check = [&](bool ok, const char *message) {
        ++checks; if(!ok) ++failures; log << (ok ? "PASS " : "FAIL ") << message << "\n";
        if(!ok && !error.IsEmpty()) log << error << "\n";
    };
    String destination = GetExeDirFile("FontExport");
    if(!CommandLine().IsEmpty()) destination = CommandLine()[0];
    RealizeDirectory(destination);
    UiDesignerSession session; session.NewDocument("blank");
    String font_root = NormalizePath("../../../upp_Ui/examples/UiTypographyDemo/fonts", GetFileFolder(__FILE__));
    if(CommandLine().GetCount() > 1) font_root = CommandLine()[1];
    for(const char *family : { "pt-sans", "pt-serif" }) {
        String prefix = String(family) == "pt-sans" ? "PT_Sans-Web" : "PT_Serif-Web";
        String license = LoadFile(AppendFileName(font_root, String(family) == "pt-sans" ? "ptsans-OFL.txt" : "ptserif-OFL.txt"));
        for(const char *style : { "Regular", "Bold", "Italic", "BoldItalic" }) {
            String id;
            Check(session.ImportProjectFont( AppendFileName(font_root, prefix + "-" + style + ".ttf"), family, license, id, error), "import copied font resource");
        }
    }
    Check(session.Document().GetResources().GetCount() == 8, "eight resource-owned font faces");
    Check(session.Commands().IsDirty(), "font import marks the project unsaved");
    Check(session.Commands().Undo() && session.Document().GetResources().GetCount() == 7, "font import is undoable");
    Check(session.Commands().Redo() && session.Document().GetResources().GetCount() == 8, "font import redo restores copied bytes");
    Check(session.Theme().Commit("body_font", "project:pt-sans", "Choose body", error), "body authoring uses stable selection");
    Check(session.Theme().Commit("heading_font", "project:pt-serif", "Choose heading", error), "heading authoring uses stable selection");
    Check(session.Theme().Commit("code_font", "system:monospace", "Choose code", error), "code role keeps system source explicit");
    Check(UiFonts::GetTypography().body == "project:pt-sans", "session feeds shared typography");
    UiDesignerNodeId label = session.AddControl("UiLabel"), button = session.AddControl("UiButton");
    UiDesignerNodeId edit = session.AddControl("UiLineEdit"), dropdown = session.AddControl("UiDropdown");
    UiDesignerNodeId title = session.AddControl("UiTitleCard");
    Check(label && button && edit && dropdown && title, "representative controls authored");
    session.Commands().SetProperty(label, "text", "a.FaceName(\"x\");", UiDesignerImpactControlState | UiDesignerImpactCode);
    session.Select(button);
    Check(session.CommitThemeOverride("font_face", "project:pt-serif", error), "local font override retains stable family id");
    UiDesignerPreviewCanvas preview;
    preview.SetRuntimeTheme(session.Theme().GetEffective());
    session.AttachProjection(&preview);
    Check(preview.FindRuntime(label) != nullptr, "actual preview controls created");
    auto *preview_button = dynamic_cast<UiButton*>(preview.FindRuntime(button));
    Check(preview_button && UiFonts::Selection(preview_button->GetStyle().font) == "project:pt-serif", "preview applies project local override");
    String project = AppendFileName(destination, "project.uidesign.json");
    Check(session.Save(project, error), "project saves fonts and typography");
    UiDesignerSession restored;
    Check(restored.Load(project, error), "project restores copied font resources");
    Check(restored.Document().GetResources().GetCount() == 8 && restored.Theme().Get().typography == session.Theme().Get().typography, "save/load retains asset and role identity");
    restored.Select(button);
    Check(restored.Document().GetThemeOverride(button, "font_face") == "project:pt-serif", "local identity survives roundtrip");
    UiDesignerExportRequest request;
    request.destination = AppendFileName(destination, "PackagedFonts");
    request.generation.package_name = "PackagedFonts"; request.generation.class_name = "PackagedFontWindow";
    request.write.overwrite = UiDesignerOverwritePolicy::ReplaceAll;
    auto result = UiDesignerExportService(restored.Catalog()).Execute(restored.Document(), restored.Theme(), request);
    Check(result.success, "generated font package exports");
    if(!result.success) log << result.diagnostic << "\n";
    String source = LoadFile(AppendFileName(request.destination, "PackagedFontWindow.generated.cpp"));
    String header = LoadFile(AppendFileName(request.destination, "PackagedFontWindow.generated.h"));
    Check(source.Find("FontBootstrap::FontBootstrap()") >= 0 && header.Find("font_bootstrap_") < header.Find("UiButton "), "registration precedes construction of member controls");
    Check(source.Find(".SetText(\"a.FaceName(") >= 0, "font emitter does not rewrite quoted control text");
    Check(source.Find("UiFonts::ApplySelection(") >= 0 && source.Find(".FaceName(\"project:") < 0, "generated local fields use shared resolver");
    ValueMap manifest = ParseJSON(LoadFile(AppendFileName(request.destination, "fonts/manifest.json")));
    ValueArray assets = manifest["assets"];
    Check(assets.GetCount() == 8, "selected families package every genuine style");
    for(const Value& v : assets) {
        ValueMap a = v;
        String bytes = LoadFile(AppendFileName(request.destination, AsString(a["file"])));
        Check(SHA256String(bytes) == a["sha256"], "exported bytes preserve content identity");
    }
    UiDesignerThemeDocument body_only; body_only.Commit("body_font", "project:pt-sans", "Body", error);
    UiDesignerDocument simple; simple.NewDocument();
    for(const auto& r : restored.Document().GetResources()) simple.AddResource(r);
    simple.SetProperty(simple.GetRootId(), "title", "project:not-a-font", UiDesignerImpactCode);
    String err;
    auto selected = UiDesignerExportService(restored.Catalog()).BuildCppProject(simple, body_only, request, err);
    int font_count = 0; for(const auto& f : selected.files) if(f.relative_path.EndsWith(".ttf")) ++font_count;
    Check(selected.generated_source.Find("DeclareMissing") < 0, "ordinary project-prefixed text does not collect font assets");
    Check(font_count == 4, "unselected family is excluded from packaged export");
    UiDesignerThemeSnapshot missing = body_only.Get(); missing.typography.Set("body_font", "project:unavailable");
    UiDesignerThemeDocument missing_theme; missing_theme.Replace(missing);
    auto missing_export = UiDesignerExportService(restored.Catalog()).BuildCppProject(simple, missing_theme, request, err);
    Check(missing_export.generated_source.Find("DeclareMissing") >= 0 && missing_export.generated_source.Find("project:unavailable") >= 0, "missing export preserves selection and explicit fallback");
    UiDesignerThemeSnapshot legacy;
    ValueMap legacy_value = legacy.ToValue(); legacy_value.RemoveKey("typography");
    legacy.typography.Set("body_font", "project:pt-sans");
    Check(legacy.FromValue(legacy_value, error) && legacy.typography.IsEmpty(), "legacy family-name projects remain compatible");
    UiFonts::SetCatalog(UiFontCatalog());
    Check(UiFonts::Catalog().Choices(false).IsEmpty(), "project switch clears project font selectors");
    UiDesignerActivateFonts(restored.Document(), restored.Theme().Get());
    Check(UiFonts::Resolve("project:pt-sans", StdFont()).status == UiFontStatus::Loaded, "project activation restores resolution");
    log << Format("checks=%d failures=%d\n", checks, failures);
    SaveFile(AppendFileName(destination, "results.txt"), log);
    session.AttachProjection(nullptr);
    SetExitCode(failures ? 1 : 0);
}
