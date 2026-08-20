# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Re-fetch current refs before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-20

STATUS: **THEME BUILDER PALETTE FOUNDATION + FIRST WORKSPACE SLICE PUBLISHED; PREVIEW AND THEME WINDOWS VALIDATION PENDING.**

Authoritative branch: `main`.

Current source checkpoints:

- `af05395787ce105fc3a03d2b26d4aa74bced51c9` — first Theme Builder workspace: six-colour Light/Dark palette editing through production `UiColorPicker`, separate Control Role and Panel Role previews, and representative controls below;
- `61a7ebf37f3d79ddac4113bbad75b00c10b0bdd3` — Theme Builder document foundation: schema-2 Light/Dark palettes, semantic role assignments, migration, undo/redo and focused ThemeDocument regression package;
- `0757a06f640f5353944874390fd8e157b39f0a10` — preserve authored Grid structure through preview reconstruction and make nested transparent layouts directly selectable from their interaction space/perimeter.

Concurrent/validation follow-up after the Preview source checkpoint:

- `c225eee336365a6ec69ebfa363747edab550c446` — test-only correction to probe the Box perimeter rail on the actual GroupPanel centreline; Preview implementation unchanged;
- `0449ac0cabe265df051515356563256b10440fa0` — `.gitignore` update.

Previously accepted Windows visual-readiness baseline:

- `580ced97e66e5304d73929e05e52fb116ca686ff` — preceding Designer visual-readiness/test-linkage state. This does **not** automatically validate the later Preview or Theme Builder checkpoints.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, source diagnosis, substantive implementation, reconciliation, diff review, publishing decisions and acceptance.

Gary's role is **Windows validation and minor mechanical follow-up only**:

- fetch and test the exact requested published SHA/ancestor;
- report compiler/runtime failures with exact diagnostics and reproduction;
- perform only genuinely mechanical build corrections if necessary;
- do not redesign Preview reconstruction, geometry selection, PropertyEditor, session/model ownership, Theme Builder, theme adapters or Designer data contracts.

Substantive failures return to the supervisor.

## PREVIEW RELIABILITY CHECKPOINT

### Grid reconstruction defect

Human reproduction before `0757a06f...`:

1. place a `UiGridLayout`;
2. author `Rows = 1`, `Columns = 3`;
3. Preview correctly becomes 1x3;
4. add a child;
5. structural preview rebuild visibly falls back to 2x2 while PropertyEditor/document still reports 1x3.

Root cause was Preview reconstruction, not durable document state. Live Grid property edits called `UiGridLayout::SetGridSize(columns, rows)`, but freshly rebuilt Grids passed through generic scalar replay and retained the runtime 2x2 constructor defaults. The same asymmetry existed for coupled minimum-cell dimensions.

`0757a06f...` adds a Grid-specific preview adapter that reconstructs rows, columns, minimum cell dimensions and layout state from authored document values.

The exact Grid failure was not found in `UiBoxLayout`'s main container fields: generic reconstruction already reapplies Box direction, wrapping, inset and gap. The regression fixture authors Box direction/wrap before another structural rebuild and verifies those values survive.

### Nested transparent-layout selection

For `UiGridLayout -> UiBoxLayout -> UiGroupPanel`, zero Grid inset/gap previously made the transparent Box impractical to select directly from Preview.

`0757a06f...` keeps normal deepest-child selection but first reserves layout-owned interaction space:

- inset regions select their layout;
- gap regions select their layout;
- a narrow 4-DPI perimeter rail keeps zero-inset/zero-gap layouts selectable;
- deepest layout wins when layout rails coincide;
- away from these regions the child remains the normal target.

`c225eee...` changes only the focused test probe to intersect the natural-height GroupPanel correctly; it does not change production hit testing.

### Focused Preview regression package

- `tests/PreviewLayoutRegressionTest/PreviewLayoutRegressionTest.upp`
- `tests/PreviewLayoutRegressionTest/main.cpp`

Required summary:

```text
PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=<n> failed=0
```

The fixture covers live Grid 1x3, structural insertion, second reconstruction, Box direction/wrap replay, direct Box perimeter selection and ordinary child selection.

## THEME BUILDER — ACTIVE IMPLEMENTATION

The user explicitly authorized Theme Builder implementation to proceed while the earlier Preview Windows gate is still running. Do not revert to the old sequencing note that Theme work must wait.

### Product goal

Theme Studio is becoming a practical **Theme Builder**, not an undifferentiated control gallery.

Required direction:

- a deliberately engineered palette is a first-class authored asset;
- exactly six authored colours for Light and six for Dark in the current design;
- use the production `UiColorPicker` palette/generator workflow, including complementary/triad/analogous generation and its existing drag transfer into primary slots;
- keep **Control Roles** separate from **Panel Roles**;
- use representative/category-filtered controls for visual testing;
- save/reload themes for later tweaks and reuse;
- a loaded/authored theme must be usable in the normal Designer preview, not only in Theme Builder;
- project save/export, standalone theme JSON and runtime/code generation must converge on one authored theme contract rather than UI-only mirror state.

### Theme document foundation — `61a7ebf...`

`UiDesignerThemeSnapshot` now owns two six-colour palettes:

- `light_palette`;
- `dark_palette`.

It also owns semantic palette-slot assignments:

Control Roles:

- Standard;
- Subtle;
- Accent;
- Alert.

Panel Roles:

- Surface;
- Subtle;
- Strong.

The existing `accent` field remains temporarily as a compatibility projection derived from the active appearance palette and assigned Control Accent slot. It is not a second authority.

Standalone theme serialization is schema 2 and round-trips palettes and role assignments. Schema-1 themes migrate by carrying their old single Accent colour into both Light and Dark Accent slots so appearance switching does not silently lose the authored colour.

Theme preview/commit/undo/redo and the existing PropertyEditor model now support all palette slots and role assignments.

Existing Designer project persistence already stores `theme_.Get().ToValue()`, and export already supports `theme.json`; therefore these new authored fields travel through the existing project/export path rather than a parallel file format.

### Theme document regression package

New package:

- `tests/ThemeDocumentTest/ThemeDocumentTest.upp`
- `tests/ThemeDocumentTest/main.cpp`

Required summary:

```text
THEME_DOCUMENT_SUMMARY checks=<n> failed=0
```

It covers:

- independent Light/Dark palette editing;
- separate Control and Panel role assignments;
- active Accent compatibility projection;
- schema-2 round-trip;
- schema-1 migration;
- transient preview/cancel;
- undo/redo;
- Theme Inspector exposure of both palette domains and both role domains.

### First Theme Builder workspace — `af053957...`

The Theme Gallery now begins with a Theme Builder workspace containing:

1. **Theme Palette**
   - Light/Dark palette selector;
   - production `UiColorPicker`;
   - exactly six primary theme slots;
   - generator count six;
   - palette generator/stash/internal drag workflow retained;
   - a slot change commits directly to `UiDesignerThemeDocument`.

2. **Control Roles**
   - Standard, Subtle, Accent and Alert shown separately;
   - each preview identifies the assigned palette slot.

3. **Panel Roles**
   - Surface, Subtle and Strong shown separately using real `UiPanelRole` semantics;
   - each preview identifies the assigned palette slot.

4. Existing representative controls remain below for the moment. They will be reduced/reorganized into intentional category views rather than retained as a large control zoo.

The role swatches directly visualize the authored palette now. Full arbitrary-palette propagation through every `upp_Ui` runtime resolver is **not yet implemented** and must not be claimed as complete.

### Next Theme Builder slices

1. add a clean standalone Theme Save/Load interaction around `UiDesignerThemeDocument::Serialize/Deserialize`, so authored themes can be reused independently of a project;
2. synchronize loaded theme preset/mode state cleanly with the normal Designer shell;
3. define the real `upp_Ui` custom semantic-palette runtime contract rather than maintaining hardcoded/Designer-only colour overrides;
4. make Designer preview and generated/runtime output consume that same contract;
5. replace the remaining broad gallery with bounded category-filtered representative control sets;
6. if palette-to-role drag is still desirable after role assignment UX is evaluated, expose a proper public drag contract rather than reaching into `UiColorPicker` internals.

## MODEL / PROPERTYEDITOR CONTRACT

Retained architecture:

- `UiDesignerDocument` is durable authored UI state;
- `UiDesignerThemeDocument` is durable authored theme state;
- PropertyEditor models are projections, not parallel application state;
- transient previews remain transient;
- durable UI/theme/data edits go through their owning model/command services;
- generated output resolves from committed document/theme state;
- `UiDesignerSession` remains the document -> projection/model synchronization authority;
- Window observers append with `<<` and remain presentation-only.

For model-backed controls, Data edits the same production model represented by the document/preview contract; do not introduce demo-only mirrors or duplicate synchronization layers.

## PROPERTYEDITOR VISUAL LANGUAGE

The Designer PropertyEditors use an explicit 38% label/value divider and theme-aware Light/Dark styling.

Canonical common override grouping remains based on UiLabel: General, Face, Frame, Ink, Icon, Typography, Content Margin, Focus, Shadow, Highlight. Control-specific domains such as List Rows/*, Dropdown Popup/* and Accordion Header/* / Body/* remain nested and must not be flattened.

## CROSS-REPOSITORY UI STATE

Last refreshed `Trilec/upp_Ui/main` during the Theme Builder audit:

- `1c239c68c504919e60859955db4faf9ea537d181`.

Relevant runtime facts at that checkpoint:

- `UiThemeContext` currently contains only preset + mode;
- `UiRole` provides Standard/Subtle/Accent/Alert;
- `UiPanelRole` separately provides Surface/Subtle/Strong;
- current resolver palettes are predominantly hard-coded by preset/mode, so a real custom authored-palette bridge still needs to be designed;
- `UiColorPicker` already exposes six-slot editing primitives, 2-12 swatch generation, harmony modes including analogous/complementary/triad, User Stash and internal grouped drag transfer.

Refresh `upp_Ui/main` again before cross-repository implementation.

## CURRENT WINDOWS VALIDATION

The Preview validation requested from Gary was started before Theme Builder commits advanced `main`. Treat its result as validation of the requested Preview ancestor, not automatic validation of the new Theme Builder source.

Preview gate still requires:

1. `tests/PreviewLayoutRegressionTest` Debug + Release — summary `failed=0`;
2. `tests/ListDataAdapterTest` Debug + Release — `checks=8 failed=0`;
3. `tests/ListEditThemeAdapterTest` Debug + Release — summary `failed=0`;
4. `tests/DropdownAccordionThemeAdapterTest` Debug + Release — summary `failed=0`;
5. UiDesigner Debug + Release;
6. focused Grid 1x3 / nested Box direct-selection GUI smoke.

Once the Theme Builder checkpoint is ready for Windows validation, add `tests/ThemeDocumentTest` Debug + Release plus focused Theme Builder UI smoke; do not conflate that with the older Preview gate.

On substantive failures Gary stops and reports exact SHA, mode, diagnostic and reproduction. Architectural/source fixes return to the supervisor.

## NEXT

1. Continue the Theme Builder in bounded slices: standalone Theme Save/Load, Designer-shell synchronization, then runtime palette contract.
2. Review and fix any Windows failure reported from the earlier Preview gate.
3. Publish and validate Theme Builder increments frequently; do not wait for a monolithic redesign.
4. Keep the runtime/editor/export theme contract single-source and round-trippable throughout.
