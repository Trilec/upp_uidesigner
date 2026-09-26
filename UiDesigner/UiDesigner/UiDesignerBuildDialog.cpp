#include "UiDesignerBuildDialog.h"

namespace Upp {
namespace {
const char* build_keys[] = {"umk", "nests", "method", "folder", "package", "class", "executable"};
}

UiDesignerBuildDialog::UiDesignerBuildDialog(UiDesignerSession& session) : session_(session)
{
    Title("Build generated application").Sizeable().Zoomable();
    SetRect(0, 0, DPI(820), DPI(620));
    SetMinSize(Size(DPI(680), DPI(490)));
    const char* captions[] = {"UMK executable", "Source nests (comma separated)", "Build method", "Package folder", "Package name", "Window class", "Output executable"};
    String root = GetFileFolder(GetExeFolder());
    String nests, umk;
    // Development installs already describe their dependencies in github.var.
    // A standalone install asks for paths instead of assuming this machine's drive.
    for(const String& line : Split(LoadFile(AppendFileName(root, "github.var")), '\n')) {
        if(!TrimLeft(line).StartsWith("UPP")) continue;
        int first = line.Find('"'), last = line.ReverseFind('"');
        if(first >= 0 && last > first) nests = Join(Split(line.Mid(first + 1, last - first - 1), ';'), ",");
    }
    for(const String& nest : Split(nests, ',')) {
        String candidate = AppendFileName(GetFileFolder(nest), "umk.exe");
        if(FileExists(candidate)) umk = candidate;
    }
    String defaults[] = {umk, nests,
        "CLANGx64", AppendFileName(root, "build/GeneratedUi"), "GeneratedUi", "GeneratedUiWindow",
        AppendFileName(root, "build/GeneratedUi.exe")};
    Value saved;
    try { saved = ParseJSON(LoadFile(ConfigFile("uidesigner-build.json"))); }
    catch(...) {}
    for(int i = 0; i < 7; ++i) {
        Add(labels_[i]); Add(fields_[i]);
        labels_[i].SetText(captions[i]);
        fields_[i].SetData(saved.Is<ValueMap>() && saved[build_keys[i]].Is<String>()
                          ? AsString(saved[build_keys[i]]) : defaults[i]);
        fields_[i].WhenChange = [=] { launch_.Disable(); };
    }
    fields_[3].Tip("The folder name must match the package name. Generated files are replaced; user .h/.cpp and main.cpp are preserved.");
    fields_[1].Tip("Include the Ui repository and U++ uppsrc. The exported package's parent folder is added automatically.");
    for(int i = 0; i < 3; ++i) {
        Add(browse_[i]); browse_[i].SetText("Browse...");
        browse_[i].WhenAction = [=] {
            FileSel select;
            int field = i == 0 ? 0 : i == 1 ? 3 : 6;
            select.Set(AsString(fields_[field].GetData()));
            bool chosen = false;
            if(i == 1) chosen = select.ExecuteSelectDir("Select package folder");
            else {
                select.Type("Executable", "*.exe");
                chosen = i == 0 ? select.ExecuteOpen("Select UMK") : select.ExecuteSaveAs("Output executable");
            }
            if(chosen) { fields_[field].SetData(~select); launch_.Disable(); }
        };
    }
    Add(log_); Add(status_); Add(build_); Add(launch_); Add(close_);
    log_.SetReadOnly();
    log_.SetCustomStyle(UiDesignerReadOnlyEditStyle());
    build_.SetText("Export + build"); build_.WhenAction = [=] { Build(); };
    launch_.SetText("Run application"); launch_.Disable(); launch_.WhenAction = [=] { Launch(); };
    close_.SetText("Close"); close_.WhenAction = [=] { CloseBuild(); };
    WhenClose = [=] { CloseBuild(); };
    status_.SetText("Build verifies compilation. Run opens the application for interaction testing.");
}

UiDesignerBuildDialog::~UiDesignerBuildDialog() { KillTimeCallback(); }

void UiDesignerBuildDialog::SetBusy(bool busy)
{
    busy_ = busy;
    build_.Enable(!busy);
    for(auto& field : fields_) field.Enable(!busy);
    for(auto& button : browse_) button.Enable(!busy);
    close_.SetText(busy ? "Stop build" : "Close");
}

void UiDesignerBuildDialog::Build()
{
    String values[7];
    for(int i = 0; i < 7; ++i) values[i] = TrimBoth(AsString(fields_[i].GetData()));
    launch_.Disable();
    if(!FileExists(values[0]) || values[1].IsEmpty() || values[2].IsEmpty() || values[6].IsEmpty()) {
        status_.SetText("Choose an existing UMK, source nests, build method and output executable."); return;
    }
    if(GetFileName(values[3]) != values[4]) {
        status_.SetText("Package folder's final name must match Package name."); return;
    }
    if(ToLower(NormalizePath(values[6])) == ToLower(NormalizePath(GetExeFilePath()))) {
        status_.SetText("Choose an output executable other than UiDesigner itself."); return;
    }
    UiDesignerExportRequest request;
    request.profile = UiDesignerExportProfile::CompleteCppPackage;
    request.destination = values[3];
    request.generation.package_name = values[4];
    request.generation.class_name = values[5];
    UiDesignerExportService service(session_.Catalog());
    auto result = service.Execute(session_.Document(), session_.Theme(), request);
    if(!result.success) { status_.SetText(result.diagnostic); return; }
    ValueMap settings;
    for(int i = 0; i < 7; ++i) settings.Add(build_keys[i], values[i]);
    SaveFile(ConfigFile("uidesigner-build.json"), AsJSON(settings));
    output_ = values[6];
    RealizeDirectory(GetFileFolder(output_));
    Vector<String> args;
    args.Add(GetFileFolder(values[3]) + "," + values[1]);
    args.Add(values[4]); args.Add(values[2]); args.Add("-br"); args.Add("+GUI"); args.Add(output_);
    args.Add("--out-dir"); args.Add(AppendFileName(GetFileFolder(output_), ".umk-cache"));
    text_ = "Exported " + values[3] + "\nBuilding with UMK (Release, GUI)...\n";
    log_.SetData(text_);
    if(!process_.Start(values[0], args)) { status_.SetText("Could not start UMK."); return; }
    SetBusy(true);
    status_.SetText("Compiling. The window remains responsive; Stop build cancels UMK.");
    SetTimeCallback(-100, [=] { Poll(); });
}

void UiDesignerBuildDialog::Poll()
{
    String chunk;
    // Read once per tick so a noisy compiler cannot monopolise the UI thread.
    process_.Read(chunk);
    if(!chunk.IsEmpty()) { text_ += chunk; log_.SetData(text_.Right(128 * 1024)); }
    if(process_.IsRunning()) return;
    process_.Read(chunk); text_ += chunk;
    KillTimeCallback(); SetBusy(false);
    const int exit = process_.GetExitCode();
    text_ += Format("\nUMK exit code: %d\n", exit);
    log_.SetData(text_.Right(128 * 1024));
    SaveFile(output_ + ".build.log", text_);
    const bool success = exit == 0 && FileExists(output_);
    launch_.Enable(success);
    status_.SetText(success ? "Build passed. Run application to test it. No event behaviour is added by building."
                            : "Build failed. See compiler errors above and the .build.log beside the executable.");
}

void UiDesignerBuildDialog::Launch()
{
    LocalProcess app;
    Vector<String> args;
    if(app.Start(output_, args, nullptr, GetFileFolder(output_))) {
        app.Detach(); status_.SetText("Application launched for manual testing.");
    }
    else status_.SetText("Could not launch the generated application.");
}

void UiDesignerBuildDialog::CloseBuild()
{
    if(busy_) {
        process_.Kill(); KillTimeCallback(); SetBusy(false);
        status_.SetText("Build stopped. Exported source files are retained.");
        return;
    }
    Break(IDCANCEL);
}

bool UiDesignerBuildDialog::Key(dword key, int count)
{
    if(key == K_ESCAPE) { CloseBuild(); return true; }
    return TopWindow::Key(key, count);
}

void UiDesignerBuildDialog::Layout()
{
    const int m = DPI(12), h = DPI(30), gap = DPI(8), label = DPI(215);
    int y = m;
    for(int i = 0; i < 7; ++i) {
        labels_[i].SetRect(m, y, label, h);
        fields_[i].SetRect(m + label, y, max(0, GetSize().cx - 2*m - label), h);
        if(i == 0 || i == 3 || i == 6) {
            int button = i == 0 ? 0 : i == 3 ? 1 : 2;
            fields_[i].SetRect(m + label, y, max(0, GetSize().cx - 2*m - label - DPI(88)), h);
            browse_[button].SetRect(GetSize().cx - m - DPI(80), y, DPI(80), h);
        }
        y += h + gap;
    }
    int bottom = GetSize().cy - m - h;
    log_.SetRect(m, y, max(0, GetSize().cx - 2*m), max(0, bottom - y - h - 2*gap));
    status_.SetRect(m, bottom - h - gap, max(0, GetSize().cx - 2*m), h);
    build_.SetRect(m, bottom, DPI(140), h);
    launch_.SetRect(m + DPI(148), bottom, DPI(140), h);
    close_.SetRect(GetSize().cx - m - DPI(110), bottom, DPI(110), h);
}
}
