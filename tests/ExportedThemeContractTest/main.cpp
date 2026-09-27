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
    const int recipe_q = effective.theme_overrides.Find("face_normal");
    Check(recipe_q >= 0 &&
          (Color)effective.theme_overrides.GetValue(recipe_q) == recipe_color,
          "shared runtime resolver selects exact appearance/domain/type/role recipe");
    Check(AsString(effective.GetProperty("icon_side", "Left")) != "Right",
          "shared runtime resolver excludes studio_preview");

    // Collection roles follow the same saved-recipe/local-override ownership.
    for(const char* type : {"UiList", "UiTree", "UiAccordion"}) {
        const UiDesignerControlSpec* role_spec = session.Catalog().Find(type);
        const String field = String(type) == "UiAccordion" ? "header_subtitle_color" : "selected_face";
        theme.SetActiveStyleTarget(String("Dark|control|") + type + "|Alert");
        Check(theme.Commit("studio." + field, recipe_color, "Author role recipe", error),
              String(type) + " Alert recipe commits");
        UiDesignerNode node;
        node.type = type;
        node.properties = role_spec->defaults;
        node.SetProperty("role", "Alert");
        auto resolved = UiDesignerResolveRuntimeThemedNode(node, theme.Get(), *role_spec);
        Check((Color)resolved.theme_overrides[field] == recipe_color,
              String(type) + " inherits saved role recipe");
        node.theme_overrides.Set(field, local_color);
        const auto locally_resolved = UiDesignerResolveRuntimeThemedNode(node, theme.Get(), *role_spec);
        Check((Color)locally_resolved.theme_overrides[field] == local_color,
              String(type) + " local override takes precedence over saved recipe");
    }

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
    Check(session.GenerateCode("ThemeWindow") == generated,
          "Designer code view matches exported theme-aware source");
    const String project_path = AppendFileName(temp, "saved.uidesign.json");
    Check(session.Save(project_path, error), "project with authored theme saves");
    UiDesignerSession restored;
    Check(restored.Load(project_path, error) && restored.Theme().Serialize(false) == theme.Serialize(false),
          "project reload restores preset, mode and complete authored theme");

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

    session.Select(session.Document().GetRootId());
    Check(session.InspectorModel().Find("startup_appearance") && session.InspectorModel().Find("project_theme"),
          "Window inspector exposes startup appearance and project theme");
    auto Authored = [&] { ValueMap v = UiDesignerDocumentToValue(session.Document()); v.RemoveKey("revision"); return AsJSON(v); };
    const String authored_before = Authored();
    Check(session.CommitProperty("startup_appearance", "Light", error), "Window appearance commits");
    String light_source = session.GenerateCode("ThemeWindow");
    Check(light_source.Find("UiThemeMode::Light") >= 0 && light_source.Find("Color(18, 52, 86)") < 0,
          "startup Light chooses Light recipes despite Dark editor preview");
    Check(theme.Get().mode == "Dark", "startup choice does not change editor appearance");
    Check(session.Undo() && Authored() == authored_before,
          "one Undo restores Window startup choice");
    Check(!session.CommitProperty("startup_appearance", "Invalid", error), "invalid startup appearance rejected");
    Check(session.CommitProperty("startup_appearance", "Dark", error), "explicit Dark startup commits");
    Check(theme.Commit("mode", "Light", "Preview Light", error) &&
          session.GenerateCode("ThemeWindow").Find("UiThemeMode::Dark") >= 0,
          "fixed startup appearance survives editor Light switch");
    Check(session.CommitProperty("window_title", "Library \"collection\"", error) &&
          session.CommitProperty("window_resizable", false, error) &&
          session.CommitProperty("window_start_maximized", true, error), "Window title and sizing commit");
    String fixed_source = session.GenerateCode("ThemeWindow");
    Check(fixed_source.Find("Title(\"Library \\\"collection\\\"\")") >= 0 &&
          fixed_source.Find(".Sizeable(false).Zoomable(false)") >= 0 && fixed_source.Find("\tMaximize();") < 0,
          "fixed dialog has escaped title, no maximise button and no maximised startup");
    Check(session.CommitProperty("window_resizable", true, error) &&
          session.GenerateCode("ThemeWindow").Find("\tMaximize();") >= 0,
          "resizable window can start maximised");
    Check(!session.CommitProperty("window_resizable", "yes", error), "invalid boolean rejected");
    UiDesignerDocument restored_window;
    Check(UiDesignerDeserialize(UiDesignerSerialize(session.Document(), false), restored_window, error) &&
          restored_window.GetProperty(restored_window.GetRootId(), "startup_appearance") == "Dark" &&
          restored_window.GetProperty(restored_window.GetRootId(), "window_title") == "Library \"collection\"",
          "Window options roundtrip through canonical JSON");
    Check(session.ResetProperty("window_title", error) &&
          session.GenerateCode("ThemeWindow").Find("Title(\"ThemeWindow\")") >= 0,
          "reset empty title uses generated class name");
    const int original_theme = session.GetActiveProjectTheme();
    Check(session.CommitProperty("project_theme", -2, error) && session.Theme().Get().preset == "Pill",
          "Window theme selector creates a project copy of a default");
    const int copied_count = session.GetProjectThemeCount();
    Check(session.CommitProperty("project_theme", original_theme, error) &&
          session.CommitProperty("project_theme", -2, error) && session.GetProjectThemeCount() == copied_count,
          "default theme reuse does not create repeated copies");
    Check(session.Document().GetProperty(session.Document().GetRootId(), "startup_appearance") == "Dark",
          "switching project theme retains explicit startup appearance");
    if(FindIndex(CommandLine(), String("library-fixture")) >= 0) {
        UiDesignerSession library;
        String base = AppendFileName(GetExeFolder(), "ai-designs");
        Check(library.Load(AppendFileName(base, "LiveLibrary/project.uidesign.json"), error), "load live Library fixture");
        library.Select(library.Document().GetRootId());
        Check(library.CommitProperty("startup_appearance", "Dark", error) &&
              library.CommitProperty("window_title", "Music Library - Dark startup", error), "configure Library startup");
        UiDesignerExportRequest fixture;
        fixture.destination = AppendFileName(base, "LiveLibraryDark");
        fixture.generation.package_name = "LiveLibraryDark";
        fixture.generation.class_name = "LiveLibraryDarkWindow";
        auto exported = UiDesignerExportService(library.Catalog()).Execute(library.Document(), library.Theme(), fixture);
        Check(exported.success, "export runnable dark Library fixture");
    }
    DeleteFolderDeep(temp);
    Cout() << "EXPORTED_THEME_CONTRACT checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
