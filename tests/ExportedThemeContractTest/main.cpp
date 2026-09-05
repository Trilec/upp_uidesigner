#include <Core/Core.h>
#include <Ui/Ui.h>
#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Services/UiDesignerExport.h>
#include <UiDesigner/Services/UiDesignerRuntimeTheme.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

static Color ButtonFace(const UiButton& button)
{
    return button.GetStyle().palette.face[ST_NORMAL].color;
}

CONSOLE_APP_MAIN
{
    int checks = 0, failed = 0;
    auto Check = [&](bool ok, const String& label) {
        checks++;
        if(!ok) {
            failed++;
            Cout() << "FAIL: " << label << '\n';
        }
    };

    UiDesignerSession session;
    session.NewDocument("blank");
    const UiDesignerNodeId button_id = session.AddControl("UiButton");
    session.Select(button_id);
    String error;
    Check(button_id != 0, "button authors");
    Check(session.CommitProperty("role", "Accent", error),
          "button Accent role commits");

    const UiDesignerControlSpec* spec = session.Catalog().Find("UiButton");
    Check(spec && spec->FindThemeOverride("face_normal"),
          "button exposes runtime face recipe");

    UiDesignerThemeDocument& theme = session.Theme();
    Check(theme.Commit("preset", "Pill", "Use Pill", error),
          "non-default preset commits");
    Check(theme.Commit("mode", "Dark", "Use Dark", error),
          "Dark mode commits");

    const Color recipe_color(18, 52, 86);
    const Color local_color(170, 40, 60);
    theme.SetActiveStyleTarget("Dark|control|UiButton|Accent");
    Check(theme.Commit("studio.face_normal", recipe_color,
                       "Author Accent button face", error),
          "Theme Studio recipe commits");

    theme.SetActivePreviewTarget("control|UiButton");
    Check(theme.Commit("preview.icon_side", "Right",
                       "Move sample icon", error),
          "sample-only preview state commits");

    UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark);
    UiDesignerPreviewCanvas canvas;
    canvas.SetCatalog(&session.Catalog());
    canvas.SetDocument(&session.Document());
    canvas.SetRuntimeTheme(theme.Get());
    canvas.RebuildDocument();

    UiButton* button = dynamic_cast<UiButton*>(canvas.FindRuntime(button_id));
    Check(button && ButtonFace(*button) == recipe_color,
          "Designer Preview inherits ThemeDocument recipe");
    const UiDesignerNode* source_node = session.Document().Find(button_id);
    Check(source_node &&
          AsString(source_node->GetProperty("icon_side", "Left")) != "Right",
          "studio_preview never mutates authored structure");

    Check(session.CommitThemeOverride("face_normal", local_color, error),
          "active instance override commits");
    canvas.RebuildDocument();
    button = dynamic_cast<UiButton*>(canvas.FindRuntime(button_id));
    Check(button && ButtonFace(*button) == local_color,
          "active instance override wins over ThemeDocument recipe");

    Check(session.SetThemeOverrideActive("face_normal", false, error),
          "instance override disables");
    canvas.RebuildDocument();
    button = dynamic_cast<UiButton*>(canvas.FindRuntime(button_id));
    Check(button && ButtonFace(*button) == recipe_color,
          "disabled instance override inherits ThemeDocument recipe");

    const UiDesignerNode effective = UiDesignerResolveRuntimeThemedNode(
        *session.Document().Find(button_id), theme.Get(), *spec);
    Check(effective.theme_overrides.Find("face_normal") >= 0 &&
          (Color)effective.theme_overrides.Get("face_normal") == recipe_color,
          "shared runtime resolver selects exact appearance/domain/type/role recipe");
    Check(AsString(effective.GetProperty("icon_side", "Left")) != "Right",
          "shared runtime resolver excludes studio_preview");

    const String temp = AppendFileName(
        GetTempPath(), "uidesigner-exported-theme-" + AsString(Uuid::Create()));
    DeleteFolderDeep(temp);

    UiDesignerExportService exporter(session.Catalog());
    UiDesignerExportRequest request;
    request.profile = UiDesignerExportProfile::CompleteCppPackage;
    request.destination = AppendFileName(temp, "complete");
    request.generation.package_name = "ThemeFixture";
    request.generation.class_name = "ThemeWindow";
    request.generation.namespace_name = "Upp";
    request.generation.include_source_design = true;
    request.generation.include_theme = true;
    request.write.overwrite = UiDesignerOverwritePolicy::ReplaceGenerated;

    UiDesignerExportResult result = exporter.Execute(
        session.Document(), theme, request);
    Check(result.success, "complete themed package exports");

    const String generated_path = AppendFileName(
        request.destination, "ThemeWindow.generated.cpp");
    const String generated = LoadFile(generated_path);
    Check(generated.Find(
              "UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark)") >= 0,
          "generated component applies compiled theme before control build");
    Check(generated.Find("Color(18, 52, 86)") >= 0,
          "generated component contains inherited ThemeDocument recipe");
    Check(generated.Find("Color(170, 40, 60)") < 0,
          "disabled local override is not emitted over inherited recipe");
    Check(generated.Find("theme.json") < 0,
          "generated runtime has no theme.json working-directory dependency");

    const String design_path = AppendFileName(request.destination, "design.json");
    Check(LoadFile(design_path) == UiDesignerSerialize(session.Document(), true),
          "source design remains canonical instead of flattening theme into nodes");
    Check(FileExists(AppendFileName(request.destination, "theme.json")),
          "complete export retains ThemeDocument source metadata");

    UiDesignerExportRequest component = request;
    component.profile = UiDesignerExportProfile::ComponentOnly;
    component.destination = AppendFileName(temp, "component");
    result = exporter.Execute(session.Document(), theme, component);
    const String component_source = LoadFile(AppendFileName(
        component.destination, "ThemeWindow.generated.cpp"));
    Check(result.success &&
          component_source.Find(
              "UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark)") >= 0 &&
          !FileExists(AppendFileName(component.destination, "main.cpp")) &&
          !FileExists(AppendFileName(component.destination, "ThemeFixture.upp")),
          "component export self-initializes compiled theme without owning app entry point");

    UiDesignerExportRequest no_json = request;
    no_json.destination = AppendFileName(temp, "compiled-only");
    no_json.generation.include_theme = false;
    result = exporter.Execute(session.Document(), theme, no_json);
    const String no_json_source = LoadFile(AppendFileName(
        no_json.destination, "ThemeWindow.generated.cpp"));
    Check(result.success &&
          !FileExists(AppendFileName(no_json.destination, "theme.json")) &&
          no_json_source.Find(
              "UiTheme::Set(UiThemePreset::Pill, UiThemeMode::Dark)") >= 0 &&
          no_json_source.Find("Color(18, 52, 86)") >= 0,
          "compiled theme remains effective when optional theme.json is omitted");

    DeleteFolderDeep(temp);
    Cout() << "EXPORTED_THEME_CONTRACT checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
