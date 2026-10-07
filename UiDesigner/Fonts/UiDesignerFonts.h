#ifndef _UiDesigner_Fonts_h_
#define _UiDesigner_Fonts_h_
#include <Ui/UiFonts.h>
#include <UiDesigner/Core/UiDesignerCore.h>
#include <UiDesigner/ThemeCore/UiDesignerTheme.h>
namespace Upp {
// Import is an explicit authoring operation. The caller owns the embedding licence.
bool UiDesignerImportFont(UiDesignerDocument& document, const String& path,
                          const String& family_id, const String& license_text,
                          String& asset_id, String& error);
void UiDesignerLoadFonts(const UiDesignerDocument& document, UiFontCatalog& catalog);
UiTypography UiDesignerTypography(const ValueMap& fields);
void UiDesignerActivateFonts(const UiDesignerDocument& document,
                             const UiDesignerThemeSnapshot& theme);
}
#endif
