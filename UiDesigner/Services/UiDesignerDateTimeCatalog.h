#ifndef _UiDesigner_Services_UiDesignerDateTimeCatalog_h_
#define _UiDesigner_Services_UiDesignerDateTimeCatalog_h_

#include <UiDesigner/Catalog/UiDesignerCatalog.h>

namespace Upp {
void RegisterUiDesignerDateTimeCatalog(UiDesignerCatalog& catalog);
void UiDesignerConfigureDateTimeEditor(const UiDesignerNode& node, PropertyEditorItem& item);
}

#endif
