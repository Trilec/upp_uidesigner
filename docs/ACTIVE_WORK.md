# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Refresh the current remote HEAD before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-21

STATUS: **SELECTION-AWARE THEME STUDIO SOURCE SLICE COMPLETE; CURRENT WINDOWS COMPILE + VISUAL VALIDATION PENDING.**

Authoritative branch: `main`.

Current substantive source/test checkpoint immediately before this documentation update:

- `d7d819c60f989eb586a4b05f1d8dd199b6e3b89a` — typed Theme Studio style JSON persistence gate;
- `9110a948d6dcb9f3495b4c5af02f3cd3afe04ae7` — recursive typed-value persistence for Theme Studio style recipes;
- `6f96d0573615490a79fa04b5700e47770d617a98` — selectable Theme Studio samples and live adapter-backed property editing;
- `13b9b325272d6bfcd076ce010289552ae75bbedd` — durable style-recipe persistence/history contract;
- `84a6e3dc61b24647b832e8720240974f9a056bd8` — Theme Studio style-document foundation.

Current `upp_Ui/main` dependency checkpoint:

- `bcd1278462bb0bf3a6125dd7224acc5d823f9aea` — PropertyEditor colour-drop highlight uses `DrawFatFrame` and is the last known current `upp_Ui/main`.

Important retained Preview checkpoint:

- `0757a06f640f5353944874390fd8e157b39f0a10` — Grid preview reconstruction and nested transparent-layout selection fix.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, diagnosis, substantive source changes, review and publication.

Gary is the Windows validator/helper: fetch the exact requested `main`, build/run focused gates, report exact diagnostics, and make only genuinely mechanical build corrections when appropriate. Substantive design/source failures return to the supervisor.

## THEME STUDIO — CURRENT PRODUCT CONTRACT

Theme Studio is a theme-focused editing workspace, not a static control gallery and not a second Designer document.

The top work area is intentionally compact:

- Controls / Containers are the two preview modes;
- Panel Role is independent and uses the real `UiPanelRole` vocabulary: Surface, Subtle, Strong;
- Control Role uses Standard, Subtle, Accent, Alert;
- Light and Dark each retain six persistent palette swatches;
- toolbar layout is one compact horizontal line: `Light [six swatches]    Dark [six swatches]`, with labels vertically centred beside their swatches;
- clicking a swatch opens the temporary production `UiColorPicker`; accepting commits all six colours atomically;
- persistent swatches remain colour drag sources for PropertyEditor colour rows.

The palette laboratory must not become a permanent embedded control in Theme Studio.

## SELECTION-AWARE PROPERTY EDITING

Theme Studio samples backed by a real catalog Theme adapter are selectable. Current representative selectable types include Panel, GroupPanel, Label, Button, ToolButton, SplitButton, CheckBox, RadioButton, Toggle, Dropdown, integer/float/text edits, Slider, ProgressBar, List, Tree, Tab and Accordion, plus corresponding container samples.

Selection uses the existing catalog/theme-adapter contract rather than a fake Theme Studio property schema:

1. selecting a sample records its type and whether it is a panel/control sample;
2. the right PropertyEditor is rebuilt from that type's real `UiDesignerThemeOverrideSpec` rows;
3. inherited values resolve through the real `UiDesignerThemeAdapter` for the selected semantic role;
4. edits use `studio.<field>` ids but preview/commit/reset still route through the single session-owned `UiDesignerThemeDocument`;
5. transient edits remain transient; committed edits enter ThemeDocument history and support undo/redo;
6. the selected sample is immediately restyled through the same real adapter, so edits are visible while authoring;
7. switching Light/Dark, Control Role or Panel Role selects a separate durable style target rather than overwriting another target.

Non-theme-adapter controls must not acquire fake theme properties merely to make them clickable. If a control is presented as an editable Theme Studio sample, it must be backed by a real adapter.

## DURABLE STYLE RECIPE CONTRACT

`UiDesignerThemeSnapshot` now owns durable `style_overrides` keyed by:

```text
<Light|Dark>|<control|panel>|<type>|<role>
```

Example:

```text
Light|control|UiButton|Accent
```

Each target contains only authored adapter fields. Reset removes the authored field and restores the inherited adapter value; an empty target recipe is removed.

Standalone Theme JSON is now schema 3. Schemas 1 and 2 remain accepted for migration.

Theme style values may contain typed U++ `Value`s such as `Color`, including nested values in compound recipes. Theme serialization therefore recursively encodes typed colours as explicit JSON objects and decodes them on load. Do not replace this with raw `AsJSON(style_overrides)`: raw runtime `Color` Values are not a safe JSON contract.

Project Save already embeds `theme.Get().ToValue()`. Theme JSON export and generated-package theme metadata use the same authoritative ThemeDocument serialization, so the new style recipes travel with existing project/export paths. Dedicated standalone Theme JSON import/reuse UI remains a later bounded feature.

## FOCUSED TESTS

### `tests/ThemeBuilderContractTest`

Must build/link the complete Theme module and pass in Debug and Release:

```text
THEME_BUILDER_CONTRACT_SUMMARY checks=<n> failed=0
```

Current assertions cover:

- atomic six-colour Light palette commit/undo/redo;
- Light/Dark independence;
- durable selected-control style commit;
- transient style preview;
- valid JSON with typed Color style values;
- schema-3 round-trip restoring typed Color values;
- reset + undo behavior and empty target cleanup;
- shared colour parsing and PropertyEditor colour-domain contract;
- public Theme Gallery / toolbar link visibility.

### `tests/ThemeDocumentTest`

Must pass in Debug and Release:

```text
THEME_DOCUMENT_SUMMARY checks=<n> failed=0
```

Covers palette/role state, schema 3, schema-1 migration, preview, history and generic ThemeDocument projection.

### `tests/PreviewLayoutRegressionTest`

Retained Preview regression gate:

```text
PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=<n> failed=0
```

## MODEL / PROPERTYEDITOR CONTRACT

Retained architecture:

- `UiDesignerDocument` is durable authored UI state;
- `UiDesignerThemeDocument` is durable authored theme state;
- PropertyEditor models are projections, never parallel application state;
- transient previews remain transient;
- durable edits go through their owning document/command service;
- catalog Theme adapters remain the authority for control-specific theme fields and preview application;
- Window remains presentation/wiring, not another state owner;
- Designer PropertyEditors retain the explicit 38% label/value divider and theme-aware styling.

## CURRENT WINDOWS VALIDATION GATE

Validate current final `main`, not any earlier ancestor.

1. Fetch exact `upp_Ui/main` and `upp_uidesigner/main`; report both SHAs.
2. `ThemeBuilderContractTest` Debug + Release — `failed=0`.
3. `ThemeDocumentTest` Debug + Release — `failed=0`.
4. `PreviewLayoutRegressionTest` Debug + Release — `failed=0`.
5. Build `UiDesigner` Debug + Release.
6. Launch Debug UiDesigner and leave it open for Curt after smoke testing.

Theme Studio visual smoke:

- Light label is immediately beside six Light swatches; Dark immediately beside six Dark swatches; both groups share one vertically centred row;
- clicking Button selects it and changes the Theme Studio Inspector to Button adapter fields;
- changing an obvious field such as Face normal or Radius visibly changes that sample;
- clicking Panel/GroupPanel, CheckBox, Toggle, Edit, Dropdown, List/Tree and other adapter-backed representatives switches the Inspector to that type and an obvious edit is visible;
- Panel Role and Control Role remain independent;
- Light/Dark and role-specific edits remain isolated from one another;
- Reset restores inherited appearance and undo restores the authored edit;
- Theme JSON export succeeds after at least one Color style edit.

If a substantive failure occurs, stop and report exact SHA/configuration/diagnostic/reproduction. Do not redesign the architecture in validation.

## KNOWN BOUNDARY / NEXT AFTER ACCEPTANCE

Theme Studio now authors and exports durable per-type/per-role style recipes, but arbitrary Theme Studio recipes are not yet a replacement for every global `upp_Ui` resolver in normal generated/runtime applications. Do not claim that broader runtime semantic-palette bridge is complete.

After this slice is Windows/visual accepted, the next bounded work is dedicated Theme JSON import/reuse and then, if required, the shared `upp_Ui` runtime theme-consumption bridge so an authored Theme Studio theme can be applied globally outside the studio without Designer-specific mirrors.
