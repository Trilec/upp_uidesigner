#ifndef _UiDesigner_Services_UiDesignerAdvancedCatalog_h_
#define _UiDesigner_Services_UiDesignerAdvancedCatalog_h_

#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {

// Application-level catalog additions. The reusable Catalog package remains
// independent of newer Ui controls; the Designer application opts into them
// here once the corresponding Ui runtime is available.
void RegisterUiDesignerAdvancedCatalog(UiDesignerCatalog& catalog);

class UiDesignerApplicationCatalog : public UiDesignerCatalog {
public:
    UiDesignerApplicationCatalog();
};

}

#endif
