#ifndef _UiDesigner_Services_UiDesignerRuntimeTheme_h_
#define _UiDesigner_Services_UiDesignerRuntimeTheme_h_

#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/ThemeCore/UiDesignerTheme.h>

namespace Upp {

// Runtime Theme Studio recipes are inherited appearance. Active per-control
// theme overrides remain the final authority; disabled/saved overrides inherit
// the ThemeDocument recipe again.
bool UiDesignerUsesPanelThemeDomain(const UiDesignerControlSpec& spec);
String UiDesignerRuntimeThemeTarget(const UiDesignerThemeSnapshot& theme,
                                    const UiDesignerControlSpec& spec,
                                    const UiDesignerNode& node);
ValueMap UiDesignerRuntimeThemeRecipe(const UiDesignerThemeSnapshot& theme,
                                      const UiDesignerControlSpec& spec,
                                      const UiDesignerNode& node);
UiDesignerNode UiDesignerResolveRuntimeThemedNode(
    const UiDesignerNode& node,
    const UiDesignerThemeSnapshot& theme,
    const UiDesignerControlSpec& spec);
void UiDesignerApplyRuntimeThemeRecipes(
    UiDesignerDocument& document,
    const UiDesignerThemeSnapshot& theme,
    const UiDesignerCatalog& catalog);

}

#endif
