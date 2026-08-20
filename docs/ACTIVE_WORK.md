# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Refresh the current remote HEAD before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-20

STATUS: **THEME BUILDER INTERACTION / MATRIX SLICE PUBLISHED; WINDOWS COMPILE + VISUAL VALIDATION PENDING.**

Authoritative branch: `main`.

Current source/test checkpoint immediately before this documentation update:

- `0caee55a1f1a9046cd1a523f4e02afd602ac2151` — focused Theme Builder compile/behavior gate added on top of the completed interaction/matrix slice.

Important earlier checkpoints retained:

- `0757a06f640f5353944874390fd8e157b39f0a10` — Grid preview reconstruction and nested transparent-layout selection fix;
- `580ced97e66e5304d73929e05e52fb116ca686ff` — preceding accepted Windows visual-readiness baseline. It does not validate later Preview or Theme Builder work.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, diagnosis, substantive source changes, review and publishing.

Gary is the Windows validator/helper:

- fetch the exact requested `main`;
- build/run the focused gates;
- report exact compiler/runtime diagnostics;
- make only genuinely mechanical build corrections if explicitly appropriate;
- do not redesign Theme Builder, PropertyEditor, Preview, session/model ownership or theme contracts.

Substantive failures return to the supervisor.

## THEME BUILDER — CURRENT DESIGN CONTRACT

The user corrected the earlier embedded-palette interpretation. Do **not** restore a full-time `UiColorPicker` inside Theme Studio.

The intended Theme Builder top work area is compact:

1. **Controls / Containers** — the only two preview modes.
2. **Panel Role** — independent role used by preview hosting surfaces.
3. **Control Role** — independent role used by representative controls.
4. **Light / Dark** — active preview appearance.
5. **Six Light swatches** and **six Dark swatches** — persistent working palettes.

The Panel Role and Control Role selectors are deliberately independent so controls can be judged against a different surface role.

### Six-colour palette editing

The twelve top-bar swatches are the persistent palette work area. There is no permanent palette laboratory in the preview.

Clicking any Light swatch:

- opens a temporary production `UiColorPicker` dialog;
- loads all six current Light colours into its primary slots;
- selects the clicked slot;
- keeps six generator slots and the existing analogous/complementary/triad, palette and User Stash workflows available;
- accepting commits the complete six-colour Light palette as one undoable theme history step.

Dark behaves identically and remains independent.

The authored theme document remains the authority. `UiDesignerThemeDocument::CommitPalette()` performs the six-colour atomic commit; project/theme serialization remains schema 2 and round-trippable.

### Palette drag / clipboard workflow

The persistent Theme Builder swatches are colour drag sources. They publish standard colour text (`#RRGGBB`) rather than a Designer-private payload.

`Utilities/PropertyEditor` in `Trilec/upp_Ui` now provides the reusable destination behavior:

- `PropertyEditorKind::Color` rows accept text colour drops;
- the exact value cell is visibly highlighted while a valid colour drop is targeted;
- a valid drop commits through the PropertyEditor model and normal `WhenCommit` pipeline;
- inactive override rows are activated first and the colour commit is deferred until the rebuilt model is editable;
- `Ctrl+C` on a selected colour row copies `#RRGGBB`;
- `Ctrl+V` parses clipboard colour text through production `UiColorPicker::ParseColorText()` and commits normally.

Direct drop onto a rendered control is deliberately **not** implemented because Face/Frame/Ink/etc. would be ambiguous. Drag from the temporary picker directly into the editor is also not required for this slice; accept the palette first, then drag from the persistent top swatches.

Current `upp_Ui/main` containing this reusable PropertyEditor support:

- `1088011f5b806ec7a21d58774b8a80ab1bace6b7`.

## THEME BUILDER PREVIEW MATRIX

The broad old control zoo has been replaced by two intentional views built from the production Ui layout controls.

### Controls view

- outer `UiGridLayout`: 3 columns × 1 row;
- each column contains a vertical `UiBoxLayout` stack;
- representative families are grouped into practical surfaces rather than one enormous flow;
- examples include Buttons, Choices, Numeric & Sliders, Inputs, Data, Navigation and Feedback;
- a plain `UiPanel` reference is included so panel treatment can be judged independently of `UiGroupPanel` chrome;
- group panels use the selected Panel Role while representative controls use the selected Control Role.

### Containers view

The same 3-column / vertical-stack structure shows container chrome both alone and with representative content. Current samples intentionally pair:

- plain `UiPanel` without controls;
- plain `UiPanel` with button/check content;
- quiet `UiGroupPanel`;
- form `UiGroupPanel` with edit/action;
- numeric `UiPanel` with slider/int edit;
- choice `UiGroupPanel` with dropdown/toggle.

This avoids another mode switch merely to see “with controls / without controls”; both are visible in the Containers view for direct comparison.

## WINDOW INTEGRATION

Theme Builder uses one `UiDesignerThemeToolbar` bound directly to:

- the session-owned `UiDesignerThemeDocument`;
- the Theme Gallery preview.

The previous Theme Studio pill/button construction path is retained only through small compatibility shims while the large `UiDesignerWindow.cpp` source is mechanically migrated. Those old construction targets are inert and are not visible or authoritative. The visible toolbar owns Controls/Containers, role selectors, appearance and palette swatches.

There is no duplicate theme state in the Window.

## FOCUSED THEME TESTS

### `tests/ThemeDocumentTest`

Existing schema/role/palette test. Required:

```text
THEME_DOCUMENT_SUMMARY checks=<n> failed=0
```

Covers schema-2 persistence, schema-1 migration, Light/Dark independence, role assignments, transient preview, undo/redo and atomic six-colour palette history.

### `tests/ThemeBuilderContractTest`

New compile/behavior gate. It deliberately depends on the complete `UiDesigner/Theme` package and `Utilities/PropertyEditor`, so building it compiles and links the toolbar, swatch drag source, palette popup, matrix preview and PropertyEditor colour-transfer implementation.

Required:

```text
THEME_BUILDER_CONTRACT_SUMMARY checks=<n> failed=0
```

Its runtime assertions cover atomic six-colour commit/undo/redo, Light/Dark independence, schema round-trip, shared hex parsing and the PropertyEditor Color-domain contract.

Building the actual `UiDesigner` package is still required because that is the compile gate for final Window integration.

## PREVIEW RELIABILITY RETAINED

The earlier Preview fix remains part of the required regression line:

- authored Grid `Rows=1`, `Columns=3` must survive structural reconstruction;
- coupled Grid minimum-cell dimensions are reconstructed from authored state;
- Box direction/wrap/inset/gap survive structural rebuilds;
- nested transparent layouts reserve inset/gap/perimeter interaction space so a zero-inset Box can be selected directly without making child selection unusable.

Focused package:

```text
PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=<n> failed=0
```

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

The Designer PropertyEditors retain the explicit 38% label/value divider and theme-aware Light/Dark styling.

## KNOWN BOUNDARY AFTER THIS SLICE

The current six-colour palettes are authored/reusable working palettes and palette-slot role assignments. `upp_Ui` runtime theme resolvers still predominantly use their preset/mode colour recipes. Do not claim that arbitrary six-colour Theme Builder palettes already replace every runtime resolver colour automatically.

The user explicitly wants palette colours to be conveniently **applied while authoring** through the PropertyEditor drag/clipboard workflow, which is implemented in this slice.

Standalone project save already preserves the theme and Theme JSON export exists. A dedicated standalone Theme JSON **import/reuse** interaction remains a subsequent bounded feature; do not create a second theme authority to implement it.

## CURRENT WINDOWS VALIDATION GATE

Validate the current final `main`, not an older Theme Builder ancestor.

Minimum gate:

1. `tests/ThemeBuilderContractTest` Debug + Release — summary `failed=0`;
2. `tests/ThemeDocumentTest` Debug + Release — summary `failed=0`;
3. `tests/PreviewLayoutRegressionTest` Debug + Release — summary `failed=0`;
4. build `UiDesigner` Debug + Release;
5. launch Debug UiDesigner and leave it open for Curt after a short Theme Builder smoke.

Visual smoke should confirm:

- no embedded permanent `UiColorPicker` in Theme Studio;
- only Controls / Containers preview mode buttons;
- independent Panel Role and Control Role selectors;
- Light/Dark appearance switch;
- six Light + six Dark compact swatches;
- clicking a swatch opens the temporary six-slot picker preloaded from that palette;
- Controls view is a readable three-column stacked matrix;
- Containers view switches to plain/populated container examples;
- role selectors visibly change their respective preview domain;
- ordinary Theme Inspector remains readable.

Colour drag/drop should be checked if convenient: drag a persistent palette swatch onto a Color row in a PropertyEditor and confirm the row highlights and commits the new colour.

On substantive failure, Gary stops and reports exact SHA, configuration, diagnostic and reproduction. Source fixes return to the supervisor.

## NEXT AFTER WINDOWS/VISUAL ACCEPTANCE

1. Correct any compile/runtime issue exposed by the focused gate.
2. Refine matrix spacing/content from Curt's visual pass rather than redesigning the structure blindly.
3. Add dedicated standalone Theme JSON import/reuse around the existing single theme document contract.
4. Only then consider the broader custom semantic-palette bridge into every `upp_Ui` runtime resolver if the authored-theme workflow requires it.
