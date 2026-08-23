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
    if(id == "button")
        return &UiDesignerButtonThemeAdapterV2Instance();
    if(id == "tool_button")
        return &UiDesignerToolButtonThemeAdapterV2Instance();
    if(id == "check")
        return &UiDesignerCheckThemeAdapterInstance();
    if(id == "radio")
        return &UiDesignerRadioThemeAdapterInstance();
    if(id == "toggle")
        return &UiDesignerToggleThemeAdapterInstance();
    if(id == "progress")
        return &UiDesignerProgressThemeAdapterInstance();
    if(id == "slider")
        return &UiDesignerSliderThemeAdapterInstance();
    if(id == "scroll_bar")
        return &UiDesignerScrollBarThemeAdapterInstance();
    if(id == "panel")
        return &UiDesignerPanelThemeAdapterInstance();
    if(id == "group_panel")
        return &UiDesignerGroupPanelThemeAdapterInstance();
    if(id == "scroll_panel")
        return &UiDesignerScrollPanelThemeAdapterInstance();
    if(id == "tab")
        return &UiDesignerTabThemeAdapterInstance();
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
    if(kind == UiDesignerRuntimeKind::UiButton ||
       kind == UiDesignerRuntimeKind::UiSplitButton)
        return &UiDesignerButtonThemeAdapterV2Instance();
    if(kind == UiDesignerRuntimeKind::UiToolButton)
        return &UiDesignerToolButtonThemeAdapterV2Instance();
    if(kind == UiDesignerRuntimeKind::UiCheckBox)
        return &UiDesignerCheckThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiRadioButton)
        return &UiDesignerRadioThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiToggle)
        return &UiDesignerToggleThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiProgressBar)
        return &UiDesignerProgressThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiSlider)
        return &UiDesignerSliderThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiScrollBar)
        return &UiDesignerScrollBarThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiPanel)
        return &UiDesignerPanelThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiGroupPanel)
        return &UiDesignerGroupPanelThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiScrollPanel)
        return &UiDesignerScrollPanelThemeAdapterInstance();
    if(kind == UiDesignerRuntimeKind::UiTab)
        return &UiDesignerTabThemeAdapterInstance();
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
