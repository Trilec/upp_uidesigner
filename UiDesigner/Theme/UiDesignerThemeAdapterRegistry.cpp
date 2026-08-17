#include "UiDesignerThemeAdapter.h"

#define UiDesignerFindThemeAdapter UiDesignerFindThemeAdapterLegacy
#define UiDesignerGetThemeAdapter UiDesignerGetThemeAdapterLegacy
#define UiDesignerThemeAdapterSupports UiDesignerThemeAdapterSupportsLegacy
#include "UiDesignerThemeAdapter.cpp"
#undef UiDesignerFindThemeAdapter
#undef UiDesignerGetThemeAdapter
#undef UiDesignerThemeAdapterSupports

namespace Upp {
namespace {

bool IsEditRuntimeKind(UiDesignerRuntimeKind kind)
{
    return kind == UiDesignerRuntimeKind::UiLineEdit ||
           kind == UiDesignerRuntimeKind::UiIntEdit ||
           kind == UiDesignerRuntimeKind::UiFloatEdit ||
           kind == UiDesignerRuntimeKind::UiPasswordEdit ||
           kind == UiDesignerRuntimeKind::UiMultiEdit ||
           kind == UiDesignerRuntimeKind::UiMaskEdit;
}

}

const UiDesignerThemeAdapter* UiDesignerFindThemeAdapter(const String& id)
{
    if(id == "list")
        return &UiDesignerListThemeAdapterInstance();
    if(id == "edit")
        return &UiDesignerEditThemeAdapterInstance();
    if(id == "dropdown")
        return &UiDesignerDropdownThemeAdapterInstance();
    if(id == "accordion")
        return &UiDesignerAccordionThemeAdapterInstance();
    return UiDesignerFindThemeAdapterLegacy(id);
}

const UiDesignerThemeAdapter* UiDesignerFindThemeAdapter(UiDesignerRuntimeKind kind)
{
    if(kind == UiDesignerRuntimeKind::UiList)
        return &UiDesignerListThemeAdapterInstance();
    if(IsEditRuntimeKind(kind))
        return &UiDesignerEditThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiDropdown)
        return &UiDesignerDropdownThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiAccordion)
        return &UiDesignerAccordionThemeAdapterInstance();
    return UiDesignerFindThemeAdapterLegacy(kind);
}

const UiDesignerThemeAdapter* UiDesignerGetThemeAdapter(const UiDesignerControlSpec& spec)
{
    if(!spec.theme_adapter_id.IsEmpty())
        return UiDesignerFindThemeAdapter(spec.theme_adapter_id);
    return UiDesignerFindThemeAdapter(spec.runtime_kind);
}

bool UiDesignerThemeAdapterSupports(const UiDesignerControlSpec& spec)
{
    const UiDesignerThemeAdapter* adapter = UiDesignerGetThemeAdapter(spec);
    return adapter && adapter->Supports(spec.runtime_kind);
}

} // namespace Upp
