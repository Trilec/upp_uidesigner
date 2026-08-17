# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`.

## ACCEPTED LABEL REFERENCE

BASE: `155e51eb696537f8ac6a8a3af1629d2278513f66` — Label FillRecipe reference checkpoint before the final model-access migration.

TASK: `UIDESIGNER-LABEL-REFERENCE-ACCEPTANCE`.

STATUS: **LABEL REFERENCE ACCEPTED — INTERACTIVE QUADGRADIENT RESELECTION SMOKE DEFERRED.**

PUBLISHED:
- `155e51eb696537f8ac6a8a3af1629d2278513f66` — dedicated Label adapter, FillRecipe parity, focused test and coverage note.
- `59cb685219dc94bcad8d8cba82d1acc7528c6836` — migrated remaining Designer preview/test callers from retired `GetInternalModel()` to canonical `Model()`.

VALIDATION:
- `tests/LabelThemeAdapterTest` Debug + Release: `checks=242 failed=0`.
- UiDesigner Debug + Release: PASS, zero warnings/errors; Release launch responsive.
- `git diff --check`: PASS.
- no `Face/Skin` group in Designer by design; resource-aware Skin remains deferred.
- full custom-control QuadGradient reselect smoke remains deferred because Windows UI Automation cannot reach U++ descendants.

## FOUR-CONTROL OVERRIDE ROLLOUT

BASE: `e47c71d4073c9e0bb0b77bd39a8410b21f54ebf8` — Designer Label acceptance documentation head before List/Edit work.

TASK: `UIDESIGNER-LIST-EDIT-DROPDOWN-ACCORDION-OVERRIDE-NORMALIZATION`.

STATUS: **LIST + EDIT IMPLEMENTATION STAGED FOR PUBLISH — WINDOWS VALIDATION PENDING.**

CROSS-REPOSITORY BASE:
- `Trilec/upp_Ui` List runtime authority checkpoint: `d0579b8753748ca765710f6c29805d2859ddf6aa`.

TOUCHED IN LIST + EDIT SLICE:
- `UiDesigner/Theme/UiDesignerThemeAdapter.h`
- `UiDesigner/Theme/UiDesignerNormalizedThemeCommon.h`
- `UiDesigner/Theme/UiDesignerThemeAdapterRegistry.cpp`
- `UiDesigner/Theme/UiDesignerListThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerEditThemeAdapter.cpp`
- `UiDesigner/Theme/Theme.upp`
- `tests/ListEditThemeAdapterTest/`
- `docs/ACTIVE_WORK.md`

IMPLEMENTATION:
- the 251 KB legacy adapter implementation remains byte-for-byte intact and is compiled through a small registry wrapper;
- normalized List/Edit adapters replace only the `list` and `edit` registry decisions; every unaffected legacy adapter still resolves through the existing registry;
- List distinguishes outer viewport Face/Frame/Ink from `Rows/Layout`, `Rows/State`, `Content`, `Badge` and `Drag` domains;
- List outer Face states use shared `FillRecipe`; authored QuadGradient values are resolved and generated without lossy UiFill reverse-conversion;
- Edit preserves existing authored ids (`face_normal`, `text_normal`, `underline_normal`, etc.) while normalizing visible groups to General, Face, Frame, Ink, Typography, Content Margin, Editing, Underline, Whitespace, Focus, Shadow and Highlight;
- Edit Face states use shared `FillRecipe`; new common content-margin/focus/shadow/highlight and live whitespace fields are covered;
- resource-backed Skin remains intentionally deferred for both controls until the adapter receives document-resource resolution.

VALIDATION:
- source review and focused test construction complete; Windows U++ compile/runtime pending.
- focused package: `tests/ListEditThemeAdapterTest` — require emitted `LIST_EDIT_THEME_ADAPTER_SUMMARY ... failed=0` in Debug + Release.
- build UiDesigner Debug + Release after this checkpoint; no adapter-registry duplicate-definition errors are acceptable.

NEXT:
1. publish and verify the List/Edit Designer checkpoint;
2. normalize dedicated UiDropdown + UiAccordion adapters through the same registry seam;
3. normalize the four control demos without replacing large files from snippets;
4. run focused tests at each meaningful checkpoint, then give Gary one final accumulated Windows/manual validation task.
