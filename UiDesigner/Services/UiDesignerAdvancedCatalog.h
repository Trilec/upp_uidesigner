#ifndef _UiDesigner_Services_UiDesignerAdvancedCatalog_h_
#define _UiDesigner_Services_UiDesignerAdvancedCatalog_h_

#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {

void RegisterUiDesignerAdvancedCatalog(UiDesignerCatalog& catalog);

class UiDesignerAdvancedCatalogBinder {
public:
    explicit UiDesignerAdvancedCatalogBinder(UiDesignerCatalog& catalog)
        : catalog_(&catalog)
    {
        Ensure();
    }

    void Ensure()
    {
        if(catalog_)
            RegisterUiDesignerAdvancedCatalog(*catalog_);
    }

private:
    UiDesignerCatalog *catalog_ = nullptr;
};

}

#endif
