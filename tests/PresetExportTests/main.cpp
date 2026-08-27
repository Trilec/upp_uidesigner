#include <Core/Core.h>
#include <UiDesigner/Services/UiDesignerServices.h>

using namespace Upp;

static String SafePackagePart(const String& value)
{
    String out;
    for(int i = 0; i < value.GetCount(); ++i)
        if(IsAlNum(value[i]))
            out.Cat(value[i]);
    return out.IsEmpty() ? String("Preset") : out;
}

static bool ExportSession(UiDesignerSession& session, const String& label,
                          const String& package_name,
                          const String& destination, String& error)
{
    UiDesignerExportRequest request;
    request.profile = UiDesignerExportProfile::CompleteCppPackage;
    request.destination = destination;
    request.generation.package_name = package_name;
    request.generation.class_name = package_name + "Window";
    request.generation.namespace_name = "Upp";
    request.generation.include_source_design = true;
    request.generation.include_theme = true;
    request.write.overwrite = UiDesignerOverwritePolicy::ReplaceAll;
    request.write.preserve_user_files = false;

    UiDesignerExportService service(session.Catalog());
    UiDesignerExportResult result = service.Execute(
        session.Document(), session.Theme(), request);
    if(!result.success) {
        error = label + ": " + result.diagnostic;
        return false;
    }

    const String upp = AppendFileName(destination, package_name + ".upp");
    const String generated_h = AppendFileName(
        destination, package_name + "Window.generated.h");
    const String generated_cpp = AppendFileName(
        destination, package_name + "Window.generated.cpp");
    if(!FileExists(upp) || !FileExists(generated_h) || !FileExists(generated_cpp)) {
        error = label + ": generated package is incomplete";
        return false;
    }

    Cout() << "EXPORTED " << label << " " << package_name << " "
           << destination << '\n';
    error.Clear();
    return true;
}

static bool ExportCreationPreset(const String& preset,
                                 const String& package_name,
                                 const String& root, String& error)
{
    UiDesignerSession session;
    session.NewDocument(preset);
    const String destination = AppendFileName(root, package_name);
    RealizeDirectory(destination);
    return ExportSession(session, "creation:" + preset,
                         package_name, destination, error);
}

static bool ExportCatalogPreset(const UiDesignerPreset& preset,
                                const String& root, String& error)
{
    UiDesignerSession session;
    session.NewDocument("blank");
    UiDesignerNodeId created = 0;
    if(!session.InsertPreset(preset.id, session.Document().GetRootId(), -1,
                             &created, error)) {
        error = "catalog:" + preset.id + ": " + error;
        return false;
    }
    if(!created) {
        error = "catalog:" + preset.id + ": preset did not create a root";
        return false;
    }

    const String package_name = "Preset" + SafePackagePart(preset.id) + "Generated";
    const String destination = AppendFileName(root, package_name);
    RealizeDirectory(destination);
    return ExportSession(session, "catalog:" + preset.id,
                         package_name, destination, error);
}

CONSOLE_APP_MAIN
{
    const Vector<String>& args = CommandLine();
    if(args.GetCount() != 2 || args[0] != "--export-all") {
        Cout() << "Usage: UiDesignerPresetExportTests --export-all <folder>\n";
        SetExitCode(2);
        return;
    }

    const String root = NormalizePath(args[1]);
    DeleteFolderDeep(root);
    RealizeDirectory(root);

    struct CreationFixture {
        const char *preset;
        const char *package_name;
    };
    const CreationFixture creation[] = {
        {"blank", "BlankFormGenerated"},
        {"three_pane", "ThreePaneGenerated"},
        {"dialog", "DialogGenerated"},
    };
    const int creation_count = (int)(sizeof(creation) / sizeof(creation[0]));

    String error;
    int exported = 0;
    for(const CreationFixture& fixture : creation) {
        if(!ExportCreationPreset(fixture.preset, fixture.package_name,
                                 root, error)) {
            Cout() << "FAIL: " << error << '\n';
            SetExitCode(1);
            return;
        }
        exported++;
    }

    // Use the live application catalog so every registered preset is covered.
    // Adding a preset to the Designer therefore automatically adds a generated
    // package to this acceptance path instead of requiring a second hand list.
    UiDesignerSession catalog_session;
    const Array<UiDesignerPreset>& presets = catalog_session.Catalog().GetPresets();
    if(presets.IsEmpty()) {
        Cout() << "FAIL: application catalog exposes no presets\n";
        SetExitCode(1);
        return;
    }
    for(const UiDesignerPreset& preset : presets) {
        if(!ExportCatalogPreset(preset, root, error)) {
            Cout() << "FAIL: " << error << '\n';
            SetExitCode(1);
            return;
        }
        exported++;
    }

    Cout() << "UI_DESIGNER_PRESET_EXPORT_SUMMARY creation="
           << creation_count << " catalog=" << presets.GetCount()
           << " total=" << exported << " failed=0\n";
}
