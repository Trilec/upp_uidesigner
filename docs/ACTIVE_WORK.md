# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`.

## ACCEPTED LABEL REFERENCE

BASE: `155e51eb696537f8ac6a8a3af1629d2278513f66`.
TASK: `UIDESIGNER-LABEL-REFERENCE-ACCEPTANCE`.
STATUS: **LABEL REFERENCE ACCEPTED — INTERACTIVE QUADGRADIENT RESELECTION SMOKE DEFERRED.**

PUBLISHED:
- `155e51eb696537f8ac6a8a3af1629d2278513f66` — dedicated Label adapter, FillRecipe parity, focused test and coverage note.
- `59cb685219dc94bcad8d8cba82d1acc7528c6836` — final retired model-access migration.

VALIDATION:
- `LabelThemeAdapterTest` Debug + Release: 242/0.
- UiDesigner Debug + Release: PASS, zero warnings/errors; Release launch responsive.
- resource-aware Skin and manual QuadGradient reselect remain intentionally deferred.

## FOUR-CONTROL OVERRIDE ROLLOUT

BASE: `e47c71d4073c9e0bb0b77bd39a8410b21f54ebf8` — Designer head before the rollout.
TASK: `UIDESIGNER-LIST-EDIT-DROPDOWN-ACCORDION-OVERRIDE-NORMALIZATION`.
STATUS: **FOUR ADAPTER IMPLEMENTATION COMPLETE — WINDOWS VALIDATION + DEMO NORMALIZATION PENDING.**

PUBLISHED:
- `c27f499c8d51ad73037d9a60481bb73d870d38a7` — normalized dedicated UiList + UiBaseEdit adapters, registry seam and focused contract test.
- current follow-on commit adds UiDropdown + UiAccordion adapters and the shared legacy-Color FillRecipe bridge; use repository history to identify its exact SHA until the next checkpoint records it.

CROSS-REPOSITORY UI BASE:
- `Trilec/upp_Ui` `d0579b8753748ca765710f6c29805d2859ddf6aa` — List owning-style renderer authority + focused contract package.

TOUCHED:
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

IMPLEMENTATION:
- the legacy 251 KB theme-adapter implementation remains byte-for-byte intact and is compiled through a small registry wrapper; unaffected adapters still resolve through the legacy registry;
- UiList separates viewport chrome from Rows/Layout, Rows/State, Content, Badge and Drag; outer Face states use FillRecipe;
- UiBaseEdit preserves existing serialized IDs while normalizing General, Face, Frame, Ink, Typography, Content Margin, Editing, Underline, Whitespace, Focus, Shadow and Highlight;
- UiDropdown preserves the six legacy generic IDs while adding complete collapsed-control and nested Popup/Layout, Face, Frame, Items, Marker, Badge and Drag domains;
- UiAccordion preserves existing `style_*` IDs while exposing real Header/Face, Header/Frame, Header/Ink, Header/Typography, Header/Content Margin, Header/Chevron, Header/Drag, Body/Face, Body/Frame, Body/Content Margin and Body/Line composition;
- shared FillRecipe normalization accepts legacy plain Color values for fields promoted from Color to FillRecipe, so existing documents do not degrade to None;
- generated C++ reconstructs theme-derived bases and applies the same authored values as preview resolution;
- image-backed Skin and custom glyph/image resources remain deferred until the adapter receives document-resource resolution; no raw-path workaround is introduced.

VALIDATION:
- source/static review complete for the published List/Edit slice and staged composite slice;
- Windows U++ compile/runtime pending.
- `tests/ListEditThemeAdapterTest`: require `LIST_EDIT_THEME_ADAPTER_SUMMARY ... failed=0` Debug + Release.
- `tests/DropdownAccordionThemeAdapterTest`: require `DROPDOWN_ACCORDION_THEME_ADAPTER_SUMMARY ... failed=0` Debug + Release.
- build UiDesigner Debug + Release after each accepted checkpoint; duplicate registry symbols, generated-code compile failures or warning regressions are stop conditions.

NEXT:
1. publish/verify the Dropdown + Accordion checkpoint;
2. normalize UiList, UiBaseEdit, UiDropdown and UiAccordion demos without blind replacement of the large List/Dropdown source files;
3. fold the rollout state into `upp_Ui/docs/ACTIVE_WORK.md` without overwriting the concurrent UiGraph validation record;
4. run focused Windows tests/builds and four GUI demo smokes; stop on substantive failures rather than weakening tests.
