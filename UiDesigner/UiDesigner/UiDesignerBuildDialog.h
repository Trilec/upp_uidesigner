#ifndef _UiDesigner_BuildDialog_h_
#define _UiDesigner_BuildDialog_h_
#include "UiDesignerWidgets.h"
#include <UiDesigner/Services/UiDesignerServices.h>

namespace Upp {

// Exports through the application service, then runs UMK without a shell.
// Machine settings are deliberately outside the authored design document.
class UiDesignerBuildDialog : public TopWindow {
public:
    typedef UiDesignerBuildDialog CLASSNAME;
    explicit UiDesignerBuildDialog(UiDesignerSession& session);
    ~UiDesignerBuildDialog();
    void Layout() override;
    bool Key(dword key, int count) override;
private:
    void Build();
    void Poll();
    void CloseBuild();
    void Launch();
    void SetBusy(bool busy);
    UiDesignerSession& session_;
    UiLabel labels_[7], status_;
    UiLineEdit fields_[7];
    UiMultiEdit log_;
    UiButton build_, launch_, close_;
    UiButton browse_[3];
    LocalProcess process_;
    String output_, text_;
    bool busy_ = false;
};
}
#endif
