# UiDesigner Theme Override Coverage

This document records the current typed Theme Adapter coverage used by the Designer preview, validation, and code generation paths.

| Designer type | Runtime type | Adapter ID | Inherited resolver | Completed field groups | Role handling | No-override behavior | Deferred fields | Validation |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| UiButton | UiButton | `button` | `UiTheme::ResolveButton(role)` | typography, face, frame, text ink, icon ink, shadow, additional | Role-only path keeps standard inherited; non-standard role uses resolved style | inherited unless authored | none currently | automated + manual GUI validation |
| UiToolButton | UiToolButton | `tool_button` | `UiTheme::ResolveToolButton(role)` | same as UiButton plus tool-button defaults | same as UiButton | inherited unless authored | none currently | automated + manual GUI validation |
| UiLabel | UiLabel | `label` | `UiTheme::ResolveLabel(role)` | General, Face, Frame, Ink, Icon, Typography, Content Margin, Focus, Shadow, Highlight | Standard/no overrides remains inherited; non-standard role resolves a custom style | no overrides clears custom style | `Face/Skin` resource editing | `LabelThemeAdapterTest` + GUI build/smoke |
| UiTree | UiTree | `tree` | `UiTheme::ResolveTree()` | layout, visibility, glyph, icon render mode, ink, face, line | no role property | no overrides clears custom style | future tree-specific fields | automated tests |
| UiList | UiList | `list` | `UiTheme::ResolveList()` | General, Face, Frame, Ink, Icon, Typography, Content Margin, Focus, Shadow, Highlight, Rows/Layout, Rows/State, Content, Badge, Drag | no role property | no overrides clears custom style | `Face/Skin` resource editing and custom drag image selection | `ListEditThemeAdapterTest` + Ui List style contract |
| UiMenu | UiMenu | `menu` | `UiTheme::ResolveMenu()` | layout, popup, visibility, palette, separators | no role property | no overrides clears custom style | future menu-specific fields | automated tests |
| UiLineEdit / UiIntEdit / UiFloatEdit / UiPasswordEdit / UiMultiEdit / UiMaskEdit | UiBaseEdit family | `edit` | `UiTheme::ResolveEdit(context, role)` | General, Face, Frame, Ink, Typography, Content Margin, Editing, Underline, Whitespace, Focus, Shadow, Highlight | shared role path; legacy IDs retained | Standard/no overrides remains inherited | `Face/Skin` resource editing; side-control composition remains control-owned | `ListEditThemeAdapterTest` |
| UiDropdown | UiDropdown | `dropdown` | `UiTheme::ResolveDropdown(context, role)` | General, Face, Frame, Ink, Typography, Content Margin, Layout, Indicator, Focus, Shadow, Highlight, Popup/Layout, Popup/Face, Popup/Frame, Popup/Items/*, Popup/Marker, Popup/Badge, Drag | role-aware collapsed control; popup style derives from the same resolved style | Standard/no overrides remains inherited | main/popup image Skin and custom indicator/marker/drag glyph resources | `DropdownAccordionThemeAdapterTest` |
| UiAccordion | UiAccordion | `accordion` | runtime Accordion theme projection (`Surface` panel + Accent title-card header) | General, Face, Frame, Ink, Shadow, Highlight, Layout, Section, Header/*, Body/*, Behaviour, Animation | no role property on Accordion; Header uses Accent theme base | no overrides clears custom style and returns to runtime Accordion projection | outer/Header/Body image Skin and custom chevron/drag glyph resources | `DropdownAccordionThemeAdapterTest` |
| UiTab | UiTab | `tab` | `UiTheme::ResolveTab(role, visual)` | content/strip faces, frames, ink, active indicator, spacing, extent, gaps | role-aware | inherited unless authored | visual-specific palette/font/padding fields | automated tests + GUI build |
| UiPanel / UiGroupPanel / UiScrollPanel | panel family | none | control-level `UiTheme::Resolve*` | none in Inspector yet | role-aware fallback | inherited runtime style | typed panel-family schemas | runtime coverage only |

## Shared rules

- Absent overrides inherit the selected runtime theme. An authored override is the only reason to enter custom-style mode, except a non-standard semantic role where the runtime control requires an explicit resolved style.
- `None` is an explicit authored no-surface value. It is not equivalent to an absent override.
- Face state rows use the shared `PropertyEditorKind::FillRecipe` contract where the runtime field is a `UiFill`. Solid and QuadGradient recipes are consumed by preview and emitted by generated C++ from the same authored value.
- Fields that were historically plain `Color` but are now promoted to `FillRecipe` accept legacy Color values and normalize them to a Solid recipe. Existing documents therefore retain their authored colour rather than degrading to `None`.
- Image recipes refer to `UiDesignerDocument::resources`. Theme adapters currently have no document-resource resolver, so normalized adapters do not expose fake raw-path Skin or custom-glyph fields.
- Group paths represent runtime ownership. Composite controls retain domains such as Dropdown `Popup/*` and Accordion `Header/*` / `Body/*`; they are not flattened into generic Surface/Layout buckets.
- Existing serialized IDs are retained when a field already had a Designer contract. Presentation group/label/editor kind can normalize without silently invalidating authored documents.
- Preview resolution and generated C++ start from the same inherited runtime style and apply only authored fields.
- Ordinary control properties remain outside the theme layer and are applied through the normal property pipeline.

## Accepted Label reference

`UiLabel` remains the canonical reference for common override presentation. Its shared order is General -> Face -> Frame -> Ink -> Icon -> Typography -> Content Margin -> Focus -> Shadow -> Highlight, with Skin nested under Face when resource-aware editing becomes available.

The four Face states use FillRecipe and preserve authored Solid/QuadGradient data through Inspector refresh and code generation. `LabelThemeAdapterTest` is the reference parity test.

## Four-control rollout

UiList, UiBaseEdit, UiDropdown and UiAccordion extend the Label convention only where their runtime composition requires it:

- UiList keeps outer viewport chrome separate from row-owned state, content, badge and drag styling. The Ui runtime now projects the owning List style into its built-in row renderer so these authored row fields are authoritative rather than decorative Inspector rows.
- UiBaseEdit keeps the common outer surface and adds Editing, Underline and Whitespace domains while preserving its established field IDs.
- UiDropdown keeps the common collapsed-control surface and nests popup geometry/chrome/item/marker/badge styling under `Popup/*`.
- UiAccordion mirrors its real composition: outer Accordion chrome, `UiTitleCard` Header styling, `UiPanel` Body styling, section geometry, Behaviour and Animation.

Resource-backed Skin remains the deliberate shared gap until the adapter preview/codegen contract can resolve document resources. Do not work around that gap with raw filesystem paths.
