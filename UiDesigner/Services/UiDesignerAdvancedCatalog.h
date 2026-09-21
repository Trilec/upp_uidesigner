#ifndef _UiDesigner_Services_UiDesignerAdvancedCatalog_h_
#define _UiDesigner_Services_UiDesignerAdvancedCatalog_h_

#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {

// Application-level catalog additions. The reusable Catalog package remains
// independent of newer Ui controls; the Designer application opts into them
// here once the corresponding Ui runtime is available.
void RegisterUiDesignerAdvancedCatalog(UiDesignerCatalog& catalog);

// Projects a Scalar-capable control's catalog-bound data_property_id into the
// Data editor model (normally value; ChartRing binds segments). The authored
// property remains authoritative: no parallel node.data payload is maintained.
bool UiDesignerBuildScalarDataPropertyModel(
    const UiDesignerControlSpec& spec, const UiDesignerNode& node,
    PropertyEditorModel& model);

// Resolve an editor's domain from canonical configuration, shared by Inspector
// and Data. No second value or range is stored in the projection.
void UiDesignerConfigureValueEditor(const UiDesignerControlSpec& spec,
                                     const UiDesignerNode& node,
                                     PropertyEditorItem& item);

class UiDesignerApplicationCatalog : public UiDesignerCatalog {
public:
    UiDesignerApplicationCatalog();
};

}

#endif
