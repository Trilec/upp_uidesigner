# UiDesigner Theme Override Coverage

This document records the typed Theme Adapter surface used by normal Designer preview/code generation and Theme Studio. Remote `main` is authoritative.

## Common presentation contract

For common `StyledPalette` / `StyledMetrics` ownership, Theme Studio uses one vocabulary:

- General
- Face
- Frame
- Text Ink
- Icon Ink
- Typography
- Content Margin
- Focus
- Shadow
- Highlight
- Additional

Control-owned composition is then nested under its real API names, for example `Indicator`, `Track`, `Thumb`, `Fill`, `Arrow`, `Popup`, `Header`, `Body`, `Rows`, `Badge`, `Drag`, and `Tab`.

`UiFill` fields use `PropertyEditorKind::FillRecipe`; they retain Solid / None / gradient editing rather than being collapsed to a single colour. Resource-backed `StyledSkin` and image/function fields remain deliberately deferred until Theme adapters receive a real `UiDesignerDocument` resource resolver. Do not expose raw filesystem placeholders. When that resource contract lands, the user-facing skin label is **Skin (Nine Slice)**.

## Current adapter coverage

| Designer type | Adapter | Runtime resolver / ownership | Main covered domains |
| --- | --- | --- | --- |
| `UiLabel` | `label` | `UiTheme::ResolveLabel(...)` | General, Face, Frame, Ink/Icon Ink, Typography, Content Margin, Focus, Shadow, Highlight |
| `UiButton`, `UiSplitButton` | `button` | `UiTheme::ResolveButton(role)` | Face, Frame, Text/Icon Ink, Typography, Content Margin, Focus, Shadow, Highlight, button-specific fields |
| `UiToolButton` | `tool_button` | `UiTheme::ResolveToolButton(role)` | same common vocabulary with tool-button defaults |
| `UiCheckBox` | `check` | `UiTheme::ResolveCheckBox(role, visual)` | body common style, Indicator common style, typography, alignment, indicator geometry/render mode |
| `UiRadioButton` | `radio` | `UiTheme::ResolveRadioButton(role, visual)` | body common style, Indicator common style, typography, indicator geometry |
| `UiToggle` | `toggle` | `UiTheme::ResolveToggle(role)` | Track, Thumb, labels/ink, geometry and control-specific appearance |
| `UiProgressBar` | `progress` | `UiTheme::ResolveProgressBar(role)` | Track and Fill complete palette/metrics, including both radii, typography/content/focus/shadow/highlight |
| `UiSlider` | `slider` | `UiTheme::ResolveSlider()` | Track and Thumb complete palette/metrics, ticks, geometry, ring/additional fields |
| `UiScrollBar` | `scroll_bar` | `UiTheme::ResolveScrollBar()` | Track, Thumb and Arrow palette/metrics, arrow layout, expansion/fade, grip and insets |
| `UiPanel` | `panel` | `UiTheme::ResolvePanel(role)` | full panel palette/metrics, transparent mode, stable frame recipe |
| `UiScrollPanel` | `scroll_panel` | `UiTheme::ResolveScrollPanel(role)` | full panel palette/metrics plus scroll-panel surface ownership |
| `UiGroupPanel` | `group_panel` | `UiTheme::ResolveGroupPanel(role)` | panel common style plus Header ink/typography/layout/insets/spacing and Body inset |
| `UiTab` | `tab` | `UiTheme::ResolveTab(role, visual)` + `UiTab::SetVisual()` | Body common style, Tab common style, typography, strip/tab padding, spacing, icon placement, indicator, active-frame/open-corner and visual family |
| `UiLineEdit`, `UiIntEdit`, `UiFloatEdit`, `UiPasswordEdit`, `UiMultiEdit`, `UiMaskEdit` | `edit` | `UiTheme::ResolveEdit(...)` | common edit chrome, Typography, Content Margin, Editing, Underline, Whitespace, Focus, Shadow, Highlight |
| `UiDropdown` | `dropdown` | `UiTheme::ResolveDropdown(role)` | collapsed-control common style plus Popup layout/chrome/items/marker/badge/drag |
| `UiList` | `list` | `UiTheme::ResolveList(...)` | common viewport style plus Rows, Content, Badge and Drag |
| `UiTree` | `tree` | `UiTheme::ResolveTree()` | tree layout/visibility/glyph/icon/ink/face/line domains |
| `UiAccordion` | `accordion` | Accordion surface + header/body component styles | outer chrome, Section, Header, Body, Behaviour and Animation |

Theme Studio only makes adapter-backed samples selectable. Passive reference controls remain context rather than pretending to have a richer adapter than the runtime API supports.

## Theme Studio identity and preview presentation

Selecting a sample produces a read-only **Identity** section containing:

- Control
- Type
- Light/Dark Appearance
- semantic Role
- Scope (`Control role recipe` or `Panel role recipe`)

Durable theme fields appear beneath **Appearance** and its nested runtime-owned groups. The authored/inherited indicator is state only; it never replaces a PropertyEditor affordance.

Sample-only presentation remains separate from the durable theme recipe under `studio_preview`. Where the normal Designer catalog supplies them, Theme Studio reuses the exact same metadata and Preview adapter for icon chooser/render mode, icon side, icon dimensions, scale-to-content, content gap and alignment. This preserves Cardinal4, numeric slider-toggle, colour, icon and other shared editors.

## Durable rules

- Absent overrides inherit the selected runtime theme/role.
- Reset removes the authored field and returns to the adapter-resolved value.
- Undo restores authored values through `UiDesignerThemeDocument` history.
- Light and Dark targets are independent.
- Role targets are independent.
- Existing serialized field IDs are retained when an older Designer document already owns them; labels/groups/editor kinds may become clearer without invalidating saved work.
- Preview and generated C++ begin from the same runtime resolver and apply authored fields only.
- Tab visual family is special because `UiTab` stores it as runtime instance state as well as in `Style`; the runtime bridge emits/applies `SetVisual(...)` and resolves custom style deltas from the same visual family.
- Ordinary control/content/layout properties remain normal Designer properties, not duplicate theme state.

## Regression gates

`tests/ThemeAdapterCoverageTest` is the focused API/catalog/adapter gate. It verifies:

- every Theme Studio selectable type resolves a supporting adapter;
- every exposed `UiDesignerThemeOverrideSpec` is owned by that adapter;
- common Face fields remain `FillRecipe`;
- ProgressBar Track/Fill radii are present;
- Slider and ScrollBar Track/Thumb/Arrow radii are present;
- Panel/GroupPanel/ScrollPanel container fields are present;
- Tab Body/Tab/Indicator/Typography/layout coverage is present;
- Cardinal4 and numeric slider-toggle PropertyEditor metadata survive projection;
- generated Tab setup applies `UiTab::SetVisual(...)`;
- unresolved fake Skin fields are not exposed.

It must pass Debug and Release with:

```text
THEME_ADAPTER_COVERAGE_SUMMARY checks=<n> failed=0
```

Retain the existing `ThemeBuilderContractTest`, `ThemeDocumentTest`, `PreviewLayoutRegressionTest`, Label/List/Edit/Dropdown/Accordion adapter tests, and the shared `upp_Ui` PropertyEditor suites as complementary gates.
