# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Preserve unrelated concurrent advances and re-fetch current refs before any implementation or publication.

## CURRENT SUPERVISORY STATE — 2026-08-19

STATUS: **NORMALIZED THEME ADAPTER SOURCE PUBLISHED; CURRENT-HEAD WINDOWS ACCEPTANCE + DEMO CONVERGENCE PENDING.**

Authoritative branch is `main`.

Current published Designer checkpoint before this documentation refresh:

- `96bd99810aca9ae5d96afee3f7f059e588e3092c` — avoid BLITZ adapter state name collision.

Current cross-repository Ui checkpoint inspected during recovery:

- `Trilec/upp_Ui` `4a380c7071d8f0c641e01474544d648461fcb77d` — UiButton demo modernization checkpoint.

The newer Ui model/mutation work after the original four-control rollout base is a consumer-reconciliation concern, not permission to redesign Designer model architecture. Targeted recovery inspection found the Preview List/Tree/Table population paths using notifying public model operations rather than a proven stale in-place mutable-access path. No source correction is justified until a concrete incompatible mutation path is found.

## ACCEPTED LABEL REFERENCE

BASE: `155e51eb696537f8ac6a8a3af1629d2278513f66`.
TASK: `UIDESIGNER-LABEL-REFERENCE-ACCEPTANCE`.
STATUS: **LABEL REFERENCE ACCEPTED — INTERACTIVE QUADGRADIENT RESELECTION SMOKE DEFERRED.**

PUBLISHED:
- `155e51eb696537f8ac6a8a3af1629d2278513f66` — dedicated Label adapter, FillRecipe parity, focused test and coverage note.
- `59cb685219dc94bcad8d8cba82d1acc7528c6836` — final retired model-access migration.

VALIDATION:
- `LabelThemeAdapterTest` Debug + Release: 242/0.
- UiDesigner Debug + Release: PASS at the accepted Label checkpoint, zero warnings/errors; Release launch responsive.
- resource-aware Skin and manual QuadGradient reselect remain intentionally deferred.

Do not treat the accepted Label checkpoint as automatic Windows acceptance of later List/Edit/Dropdown/Accordion commits.

## FOUR-CONTROL OVERRIDE ROLLOUT

BASE: `e47c71d4073c9e0bb0b77bd39a8410b21f54ebf8` — Designer head before the rollout.
TASK: `UIDESIGNER-LIST-EDIT-DROPDOWN-ACCORDION-OVERRIDE-NORMALIZATION`.
STATUS: **FOUR ADAPTER IMPLEMENTATION PUBLISHED — CURRENT-HEAD WINDOWS VALIDATION + DEMO CONVERGENCE PENDING.**

PUBLISHED:
- `c27f499c8d51ad73037d9a60481bb73d870d38a7` — normalized dedicated UiList + UiBaseEdit adapters, registry seam and focused contract test.
- `ec02f1cbcc040f70ad55e656b98ec64640142cec` — normalized UiDropdown + UiAccordion adapters and shared legacy-Color FillRecipe bridge.
- `96bd99810aca9ae5d96afee3f7f059e588e3092c` — mechanical BLITZ translation-unit state-symbol correction in the List adapter; no contract redesign.

ORIGINAL CROSS-REPOSITORY UI BASE:
- `Trilec/upp_Ui` `d0579b8753748ca765710f6c29805d2859ddf6aa` — List owning-style renderer authority + focused contract package.

CURRENT UI RECOVERY BASE:
- `Trilec/upp_Ui` `4a380c7071d8f0c641e01474544d648461fcb77d`.
- Ui has advanced substantially in model notification, observer lifetime, sequential selection remapping and Tree update locality since `d0579b...`.
- The recovery comparison from `d0579b...` to current Ui shows the relevant List/Dropdown/Tree/Table header changes are model/binding-facing; no proven normalized-adapter style-surface break was found in the inspected slice.

TOUCHED BY THE PUBLISHED ROLLOUT:
- `UiDesigner/Theme/UiDesignerThemeAdapter.h`
- `UiDesigner/Theme/UiDesignerNormalizedThemeCommon.h`
- `UiDesigner/Theme/UiDesignerThemeAdapterRegistry.cpp`
- `UiDesigner/Theme/UiDesignerListThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerEditThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerDropdownThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerAccordionThemeAdapter.cpp`
- `UiDesigner/Theme/Theme.upp`
- `tests/ListEditThemeAdapterTest/`
- `tests/DropdownAccordionThemeAdapterTest/`
- `docs/THEME_OVERRIDE_COVERAGE.md`
- `docs/ACTIVE_WORK.md`

IMPLEMENTATION CONTRACT:
- the legacy theme-adapter implementation remains compiled through the registry wrapper; unaffected adapters continue through the legacy registry;
- UiList separates viewport chrome from Rows/Layout, Rows/State, Content, Badge and Drag; outer Face states use FillRecipe;
- UiBaseEdit preserves existing serialized IDs while normalizing General, Face, Frame, Ink, Typography, Content Margin, Editing, Underline, Whitespace, Focus, Shadow and Highlight;
- UiDropdown preserves the legacy generic IDs while adding the complete collapsed-control and nested Popup/Layout, Face, Frame, Items, Marker, Badge and Drag domains;
- UiAccordion preserves existing `style_*` IDs while exposing real Header/Face, Header/Frame, Header/Ink, Header/Typography, Header/Content Margin, Header/Chevron, Header/Drag, Body/Face, Body/Frame, Body/Content Margin and Body/Line composition;
- shared FillRecipe normalization accepts legacy plain Color values for fields promoted from Color to FillRecipe, so existing documents do not degrade to None;
- generated C++ reconstructs theme-derived bases and applies the same authored values as preview resolution;
- image-backed Skin and custom glyph/image resources remain deferred until the adapter receives document-resource resolution; no raw-path workaround is allowed.

CURRENT RECOVERY INSPECTION:
- `upp_Ui` current demo guide still defines `UiLabelDemo` as the canonical full-demo reference and production `PropertyEditor` as mandatory for modernized full demos;
- `UiButtonDemo` is now the first new-generation modernization pilot and uses production PropertyEditor models as authoritative authored state;
- current remote `UiListDemo` remains the older transitional `BuilderDemoSupport + Config + manual rows` ownership-reference generation;
- no List-specific Gary implementation or validation commit is published on remote `main`, and no List-named remote branch was found during recovery;
- Gary's reported local List work therefore remains unpublished/unknown and must be reconciled before assigning overlapping List implementation;
- targeted inspection of `UiDesignerPreview.cpp` found List population using `UiListModel::Clear/Add`, Tree population using `Clear/Set/AddChild`, and Table initialization using `SetSize`; these paths already notify through current public model APIs and do not require `Touch()` merely because the Ui models now expose explicit mutable-access notification.

VALIDATION:
- source/static review remains complete for the published normalized-adapter line;
- no current-head Windows U++ compile/runtime PASS is claimed for `96bd998...` or this documentation-only successor;
- `tests/ListEditThemeAdapterTest`: require `LIST_EDIT_THEME_ADAPTER_SUMMARY ... failed=0` Debug + Release;
- `tests/DropdownAccordionThemeAdapterTest`: require `DROPDOWN_ACCORDION_THEME_ADAPTER_SUMMARY ... failed=0` Debug + Release;
- build UiDesigner Debug + Release after focused suites; duplicate registry symbols, generated-code compile failures or warning regressions are stop conditions;
- re-run Label only when shared adapter/FillRecipe infrastructure or dependency changes warrant it.

## DEMO CONVERGENCE

Current `upp_Ui` direction supersedes the older instruction to normalize four demos mechanically in one sweep.

Canonical full-demo language is:

1. Header;
2. generous real-control preview;
3. right rail with Inspector;
4. Theme Overrides;
5. optional Data for model/domain-backed controls;
6. Code.

PropertyEditor models are authored state. A model-backed demo Data page edits the same active production model used by the preview. Do not retain a demo-only Config mirror as final authority and do not introduce a shared replacement demo framework.

For UiList specifically, the published `NormalizedDemo.cpp` is transitional. Do not polish it indefinitely if Gary has already produced a canonical PropertyEditor/Data implementation locally. Recover Gary's exact diff/SHA/evidence first.

## DEFERRED RESOURCE CONTRACT

Resource-backed Skin and custom glyph/image fields remain intentionally deferred in normalized Designer adapters.

Designer needs a real document-resource resolver shared by preview and generated code before these fields can be normalized. A standalone demo may own an application-specific provider, but Designer must not substitute raw filesystem-path rows.

## NEXT

1. Recover Gary's current UiList demo work/evidence before any overlapping List implementation.
2. Accept the published `upp_Ui` UiButton modernization on Windows before treating it as the rollout template for further demos.
3. Run current-head Designer focused Windows acceptance: `ListEditThemeAdapterTest` Debug+Release, `DropdownAccordionThemeAdapterTest` Debug+Release, then UiDesigner Debug+Release and focused GUI Inspector/Theme Override/codegen smoke.
4. If a current Ui model compatibility failure appears, diagnose the concrete consumer path first. Use notifying setters/mutation APIs, or `Touch()` only after actual mutable `Get(...)` access; do not add duplicate synchronization.
5. After Button acceptance and Gary reconciliation, migrate model-backed demos incrementally to the canonical PropertyEditor shell with a same-model Data page where useful.
6. Keep resource-aware Skin deferred until a proper Designer resource-resolution contract exists.
