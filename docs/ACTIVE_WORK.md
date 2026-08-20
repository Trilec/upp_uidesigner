# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Re-fetch current refs before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-20

STATUS: **PREVIEW LAYOUT RELIABILITY FIX PUBLISHED; WINDOWS BUILD/RUNTIME VALIDATION PENDING.**

Authoritative branch: `main`.

Current source checkpoint:

- `0757a06f640f5353944874390fd8e157b39f0a10` — preserve authored Grid structure through preview reconstruction and make nested transparent layouts directly selectable from their interaction space/perimeter.

Previously accepted visual-readiness baseline:

- `580ced97e66e5304d73929e05e52fb116ca686ff` — Gary validated the preceding Designer visual-readiness/test-linkage state. That acceptance does **not** automatically validate `0757a06f...`.

Earlier important checkpoints:

- `95c27952e23ca553bd1ac353477a6f898a596d15` — align PropertyEditor shell and preserve `UiDesignerSession` event/model synchronization;
- `3a0c1f415f5b2a985118de63cb70f6a77fc5bd06` — fix UiList Designer-data downward moves and add `tests/ListDataAdapterTest`;
- `96bd99810aca9ae5d96afee3f7f059e588e3092c` — BLITZ state-symbol correction after normalized List/Edit/Dropdown/Accordion adapters.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, source diagnosis, substantive implementation, reconciliation, diff review, publishing decisions and acceptance.

Gary's role is **Windows validation and minor mechanical follow-up only**:

- fetch and test the exact published `main` SHA;
- report compiler/runtime failures with exact diagnostics and reproduction;
- perform only genuinely mechanical build corrections if necessary;
- do not redesign Preview reconstruction, geometry selection, PropertyEditor, session/model ownership, theme adapters or Designer data contracts.

Substantive failures return to the supervisor.

## CURRENT PREVIEW RELIABILITY FIX

### Reported Grid reconstruction defect

Human reproduction before `0757a06f...`:

1. place a `UiGridLayout`;
2. author `Rows = 1`, `Columns = 3`;
3. Preview correctly becomes 1x3;
4. add a child;
5. structural preview rebuild visibly falls back to 2x2 while PropertyEditor/document still reports 1x3.

Root cause was in Preview reconstruction, not durable document state. Live Grid property edits had special handling that called `UiGridLayout::SetGridSize(columns, rows)`, but a freshly rebuilt Grid passed through the generic property replay where `rows`/`columns` did not configure the new runtime Grid. The runtime therefore retained `UiGridLayout`'s 2x2 constructor defaults. The same asymmetry existed for the coupled minimum-cell dimensions.

`0757a06f...` adds a Grid-specific preview adapter whose preview runtime stores/replays the coupled authored fields:

- rows;
- columns;
- minimum cell width;
- minimum cell height;
- inset/gap/debug state.

The durable document remains authoritative. No Window/Session duplicate state was added and code generation is unchanged.

### Box / other layout replay audit

The exact Grid failure was not found in `UiBoxLayout`'s main container fields: generic reconstruction already reapplies Box direction, wrapping, inset and gap. The new regression fixture deliberately authors Box direction/wrap before another structural rebuild and verifies the rebuilt runtime still has those values.

Continue to treat any future property that exists only in a live `ApplyProperty()` branch but is not reconstructed from authored state as a parity defect. Do not patch such defects in the UI shell.

### Nested transparent-layout selection

Human reproduction:

`UiGridLayout -> UiBoxLayout -> UiGroupPanel`

With Grid inset/gap set to zero, the Box's spacing was visible but selecting the Box from Preview was impractical/impossible; Hierarchy had to be used.

The snapshot previously selected only the deepest rectangle under the pointer. `0757a06f...` keeps normal deepest-child selection but reserves transparent layout-owned interaction space first:

- explicit inset regions select their layout;
- explicit gap regions select their layout;
- a narrow 4-DPI-pixel perimeter rail keeps a zero-inset/zero-gap layout selectable even when a stretched child covers the whole body;
- when nested layout rails coincide, the deepest layout wins;
- away from those regions, the child remains the normal selection target.

This is Designer interaction only; runtime painting, document serialization and generated code are unchanged.

## FOCUSED REGRESSION PACKAGE

New package:

- `tests/PreviewLayoutRegressionTest/PreviewLayoutRegressionTest.upp`
- `tests/PreviewLayoutRegressionTest/main.cpp`

Required summary:

```text
PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=<n> failed=0
```

The fixture covers the actual reported failure chain:

- live Grid 1x3;
- structural insertion of Box;
- rebuilt Grid remains 1x3 in document, snapshot and runtime cell geometry;
- Grid inset/gap zero;
- Box authored Horizontal + Flow state;
- structural insertion of GroupPanel;
- second rebuild still preserves Grid 1x3;
- Box runtime still preserves authored direction/wrap;
- zero-inset stretched GroupPanel overlaps the Box perimeter;
- Box perimeter resolves to Box;
- GroupPanel centre still resolves to GroupPanel.

## MODEL / PROPERTYEDITOR CONTRACT

The retained architecture remains:

- `UiDesignerDocument` is durable authored state;
- PropertyEditor models are projections of Designer/session state, not parallel application state;
- transient preview edits use the overlay;
- durable property/theme/data edits go through command services;
- generated code resolves from the same committed document/theme state;
- `UiDesignerSession` owns document -> projection/model synchronization;
- Window observers are presentation-only and append with `<<` rather than replacing session listeners.

For model-backed controls, Data edits the same production model represented by the document/preview contract; do not introduce demo-only mirrors or duplicate synchronization layers.

## PROPERTYEDITOR VISUAL LANGUAGE

The five Designer PropertyEditors use an explicit 38% label/value divider and theme-aware Light/Dark styling:

- Inspector;
- Theme Overrides;
- Data;
- Events & Actions;
- Theme Studio Inspector.

Canonical common override grouping remains based on UiLabel:

1. General
2. Face
3. Frame
4. Ink
5. Icon
6. Typography
7. Content Margin
8. Focus
9. Shadow
10. Highlight

Extensions remain control-owned:

- UiList: `Rows/Layout`, `Rows/State`, `Content`, `Badge`, `Drag`;
- UiBaseEdit: `Editing`, `Underline`, `Whitespace`;
- UiDropdown: nested `Popup/*` domains;
- UiAccordion: outer groups plus nested `Header/*` and `Body/*`.

Do not flatten these domains into generic appearance buckets.

## THEME STUDIO / THEME BUILDER — NEXT DESIGN OBJECTIVE

Do **not** broadly implement this until the current Preview reliability checkpoint is Windows-validated.

The intended redesign direction from the current visual review is:

- Theme Studio should become a practical Theme Builder rather than a generic control gallery;
- treat **Panel Role** separately from **Control Role**;
- provide representative, category-filtered control previews rather than an undifferentiated gallery;
- expose editable **Light and Dark palettes**;
- preserve semantic control roles such as Standard/Subtle/Accent/Alert while separately representing panel/surface roles;
- preview and exported theme data must consume the same authored theme contract.

Current `UiDesignerThemeSnapshot` is still shallow (preset/mode/accent/metrics/shadow values) and is not yet a full Light/Dark role-palette document. Do not fake a palette UI that cannot round-trip through the real runtime theme contract.

Resource-backed Skin/custom glyph/image editing remains deferred until Designer has one document-resource resolver shared by Preview and code generation.

## CROSS-REPOSITORY UI STATE

Last refreshed `Trilec/upp_Ui/main` during this audit:

- `1c239c68c504919e60859955db4faf9ea537d181`.

Refresh it again before any cross-repository implementation because remote main may advance.

Relevant current runtime facts at that checkpoint:

- `UiGridLayout` constructor defaults are 2 columns x 2 rows and 10x10 minimum cells;
- `UiBoxLayout` exposes runtime `GetDirection()` / `GetWrapMode()` used by the new regression fixture;
- `UiTheme` already distinguishes general control roles from `UiPanelRole`, which supports the planned Theme Builder separation.

## CURRENT VALIDATION GATE

Gary should refresh `Trilec/upp_uidesigner/main` and validate the exact current SHA/ancestor containing `0757a06f640f5353944874390fd8e157b39f0a10`.

Required validation:

1. `tests/PreviewLayoutRegressionTest` — Debug + Release
   - require `PREVIEW_LAYOUT_REGRESSION_SUMMARY ... failed=0`.
2. `tests/ListDataAdapterTest` — Debug + Release
   - require `LIST_DATA_ADAPTER_SUMMARY checks=8 failed=0`.
3. `tests/ListEditThemeAdapterTest` — Debug + Release
   - require summary `failed=0`.
4. `tests/DropdownAccordionThemeAdapterTest` — Debug + Release
   - require summary `failed=0`.
5. UiDesigner — Debug + Release build.
6. Focused GUI reproduction:
   - Grid: set 1 row / 3 columns, then add a child; it must remain visibly 1x3 and PropertyEditor must remain 1/3;
   - nested `Grid -> Box -> GroupPanel`: set Grid inset/gap zero; Box must be selectable directly from its spacing/perimeter without using Hierarchy;
   - clicking the GroupPanel away from the narrow Box perimeter must still select the GroupPanel;
   - change Box direction/wrap/inset/gap, cause a structural rebuild, and confirm those authored Box values remain reflected in Preview;
   - ordinary selection, drag/drop and PropertyEditor editing must remain functional.

On a substantive failure Gary stops and reports exact SHA, mode, compiler/runtime diagnostic and reproduction. Architectural/source fixes return to the supervisor.

## NEXT

1. Obtain Windows validation for the `0757a06f...` Preview reliability checkpoint.
2. Supervisor reviews any failure and fixes substantive source directly.
3. Once green, return to Theme Builder architecture: role model, Light/Dark palette document, representative category previews, interaction flow and export/runtime parity.
4. Only then implement the Theme Studio redesign in bounded slices.
