# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Re-fetch current refs before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-20

STATUS: **VISUAL-READINESS SOURCE CORRECTIONS PUBLISHED; WINDOWS BUILD/RUNTIME ACCEPTANCE PENDING.**

Authoritative branch: `main`.

Current source checkpoint:

- `95c27952e23ca553bd1ac353477a6f898a596d15` — align the Designer PropertyEditor shell and preserve UiDesignerSession event/model synchronization.

Immediate preceding data-integrity checkpoint:

- `3a0c1f415f5b2a985118de63cb70f6a77fc5bd06` — fix UiList Designer-data downward moves and add `tests/ListDataAdapterTest`.

Earlier normalized-adapter production checkpoint:

- `96bd99810aca9ae5d96afee3f7f059e588e3092c` — mechanical BLITZ List-adapter state-symbol correction after the List/Edit/Dropdown/Accordion normalization rollout.

No Windows acceptance is claimed for the current source checkpoint yet.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, source diagnosis, substantive implementation, reconciliation, diff review, publishing decisions and acceptance.

Gary's role for this checkpoint is **validation and minor mechanical follow-up only**:

- fetch and test the exact published `main` SHA;
- report compiler/runtime failures with exact diagnostics;
- perform only genuinely mechanical build corrections if necessary;
- do not redesign PropertyEditor, model ownership, event synchronization, theme adapters or Designer data contracts.

Substantive failures return to the supervisor for diagnosis and source correction.

## VISUAL-READINESS CORRECTIONS

### PropertyEditor shell

`UiDesigner/UiDesigner/UiDesignerWindow.cpp` now uses the production PropertyEditor consistently across:

- Inspector;
- Theme Overrides;
- model/domain Data editor;
- Events & Actions;
- Theme Studio Inspector.

The five panes now explicitly use `SetLabelRatio(38)` instead of leaving PropertyEditor in Auto label mode while merely storing a ratio in the style. This gives a stable label/value divider for visual comparison across controls and panes.

The existing light Designer palette is retained in Light mode. Dark mode derives its PropertyEditor palette from `PropertyEditorStyle::System()` after `UiDesignerApplyGlobalTheme()`, and the styles are reapplied on theme/preset changes. A Light -> Dark -> Light smoke must confirm there is no stranded light-only PropertyEditor palette.

### Event/model ownership

`UiDesignerSession` remains the authority for document -> model/projection synchronization.

The Designer window now appends presentation observers with `<<` instead of replacing session-owned `Document().WhenChanged`, `Theme().WhenChanged` and `Theme().WhenPreviewChanged` handlers.

The window no longer sends the same document change set directly to `preview_canvas_`; the attached projection is updated once by the session. This preserves Inspector and Theme Override synchronization and avoids duplicate preview application.

A remaining explicit window-side `RefreshCode()` after a document change is potentially redundant because the session also emits `WhenCodeChanged`; it is a minor efficiency observation, not a visual-readiness correctness blocker. Do not churn the large window source solely for that micro-optimization before Windows acceptance.

### UiList Designer data integrity

`UiDesignerListDataAdapter::MoveItem()` now reconstructs the authored `ValueArray` correctly for both upward and downward moves. The previous positive-delta branch could omit the moved item.

Focused regression package:

- `tests/ListDataAdapterTest/ListDataAdapterTest.upp`
- `tests/ListDataAdapterTest/main.cpp`

Required summary:

```text
LIST_DATA_ADAPTER_SUMMARY checks=8 failed=0
```

The test covers move up, move down, a multi-position downward move, preservation of all authored rows, and rejected/out-of-range moves.

## MODEL / PROPERTYEDITOR CONTRACT

The current Designer architecture is intentionally retained:

- `UiDesignerDocument` is the durable authored state;
- PropertyEditor models are projections of Designer/session state rather than parallel application state;
- preview edits use the transient overlay;
- durable property/theme/data edits go through command services;
- generated code resolves from the same committed document/theme state;
- List/Tree preview population uses notifying public model operations; do not add `Touch()` unless a concrete consumer actually mutates through mutable `Get(...)` access.

For modern `upp_Ui` model-backed demos, the same principle applies: Data should edit the same active production model used by the preview, not a demo-only mirror.

## CANONICAL PROPERTY GROUP LANGUAGE

`UiLabel` remains the common override reference:

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

Composite controls extend that vocabulary only where runtime ownership requires it:

- UiList: `Rows/Layout`, `Rows/State`, `Content`, `Badge`, `Drag`;
- UiBaseEdit: `Editing`, `Underline`, `Whitespace` in addition to common groups;
- UiDropdown: nested `Popup/*` domains remain separate from the collapsed control;
- UiAccordion: outer Accordion groups plus nested `Header/*` and `Body/*` domains.

Do not flatten these domains into generic appearance buckets and do not reintroduce demo-only/manual property-row frameworks.

## ACCEPTED LABEL REFERENCE

Historical accepted Label checkpoint:

- `155e51eb696537f8ac6a8a3af1629d2278513f66` — dedicated Label adapter / FillRecipe reference.
- `59cb685219dc94bcad8d8cba82d1acc7528c6836` — final retired model-access migration.

Historical validation at that checkpoint:

- `LabelThemeAdapterTest` Debug + Release: 242/0;
- UiDesigner Debug + Release: PASS at that checkpoint.

This historical result is not current-head acceptance.

## FOUR-CONTROL NORMALIZED OVERRIDE ROLLOUT

Published production line:

- `c27f499c8d51ad73037d9a60481bb73d870d38a7` — UiList + UiBaseEdit normalized adapters;
- `ec02f1cbcc040f70ad55e656b98ec64640142cec` — UiDropdown + UiAccordion normalized adapters;
- `96bd99810aca9ae5d96afee3f7f059e588e3092c` — BLITZ symbol correction.

Key retained rules:

- absent overrides inherit the runtime theme;
- authored overrides alone enter local custom state, except where semantic role resolution requires an explicit resolved style;
- `None` is explicit authored no-surface, not absence;
- FillRecipe bridges legacy Color values to Solid recipes;
- preview and generated C++ start from the same inherited runtime style and apply the same authored values;
- resource-backed Skin/custom glyph editing remains deferred until Designer has a document-resource resolver shared by preview and codegen.

Do not substitute raw filesystem paths for that deferred resource contract.

## CROSS-REPOSITORY UI STATE

Last inspected `upp_Ui` recovery checkpoint was:

- `Trilec/upp_Ui` `4a380c7071d8f0c641e01474544d648461fcb77d`.

That repository must be refreshed again before any new cross-repository implementation claim because `upp_Ui/main` may have advanced since the recovery inspection.

At the recovery checkpoint:

- UiButton was the new PropertyEditor demo-modernization pilot, with Windows validation pending;
- current remote UiListDemo was still the transitional `BuilderDemoSupport + Config + manual rows` generation;
- newer Ui List/Tree/Table model notification/remap changes did not reveal a proven Designer stale mutable-access path in the inspected preview slice.

Gary's previously reported local UiList demo work remains separate until exact published/local evidence is reconciled; do not overwrite or duplicate it blindly.

## CURRENT VALIDATION GATE

Gary should validate the exact current `main` SHA, without redesigning source.

1. `tests/ListDataAdapterTest` — Debug + Release
   - require `LIST_DATA_ADAPTER_SUMMARY checks=8 failed=0`.
2. `tests/ListEditThemeAdapterTest` — Debug + Release
   - require its summary with `failed=0`.
3. `tests/DropdownAccordionThemeAdapterTest` — Debug + Release
   - require its summary with `failed=0`.
4. Build UiDesigner — Debug + Release.
5. Focused GUI smoke:
   - Light -> Dark -> Light keeps every PropertyEditor pane readable;
   - Inspector / Theme Overrides / Data / Events & Actions / Theme Studio use a visibly consistent label/value divider;
   - UiLabel common groups remain canonical;
   - UiList shows Rows/Layout, Rows/State, Content, Badge and Drag ownership;
   - UiLineEdit shows Editing / Underline / Whitespace extensions;
   - UiDropdown preserves nested Popup groups;
   - UiAccordion preserves Header/* and Body/* nesting;
   - inherited/local override switching updates Preview;
   - ordinary property commits update Preview;
   - generated code follows committed values;
   - UiList Data Add/Edit/Remove/Move Up/Move Down preserves every authored row.

Substantive runtime/model/preview failures stop validation and return to the supervisor. Do not patch around them in the validator pass.

## DEFERRED RESOURCE CONTRACT

Resource-backed Skin and custom glyph/image fields remain intentionally deferred in normalized Designer adapters. Designer needs a real `UiDesignerDocument::resources` resolver shared by preview and generated code first.

## NEXT

1. Obtain current-head Windows evidence for the validation gate above.
2. Supervisor reviews any failures and performs substantive source corrections if needed.
3. Separately refresh `upp_Ui/main` and reconcile Gary's UiList demo work before overlapping demo implementation.
4. Accept the UiButton modernization on Windows before using it as the rollout template for additional full demos.
5. After those gates are green, continue model-backed demo convergence incrementally using production PropertyEditor and same-model Data pages.
