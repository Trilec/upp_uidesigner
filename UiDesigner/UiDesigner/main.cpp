#include "UiDesignerApplication.h"

using namespace Upp;

GUI_APP_MAIN
{
    // Keep the distributable bin directory separate from writable user settings.
    const String root = GetFileFolder(GetExeFolder());
    SetConfigName("UiDesigner");
    UseHomeDirectoryConfig();
    const String settings = GetConfigFolder();
    RealizeDirectory(settings);
    for(const char* name : {"uidesigner-assistant.json", "uidesigner-recent.json",
                           "uidesigner-theme-library.json", "uidesigner-build.json"}) {
        String destination = AppendFileName(settings, name);
        String previous = AppendFileName(AppendFileName(root, "build"), name);
        if(!FileExists(destination) && FileExists(previous)) FileCopy(previous, destination);
    }
    UiDesignerApplication().Run();
}
