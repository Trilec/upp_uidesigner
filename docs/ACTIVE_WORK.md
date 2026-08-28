# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

Historical Theme acceptance and earlier Designer recovery checkpoints remain in Git history. This file is the current recovery checkpoint.

## DESIGNER CLOSURE R7 — VALIDATION REPAIR — 2026-08-28

BASE:

- Source repair base: `95accf9714dd9713af23ba2623c6774111767fb3`.
- Current source repair checkpoint: `d4e83031bd0d949aa3aaf554398ec253feabfd43`.
- Required reusable PropertyEditor working-range ancestor: `44a7b1af0d26730a1f1744b80d4bea319d561bd5`.
- Current observed `upp_Ui/main`: `b096a9c9ad64ec2435731ddcfeb0cdaf1b6cb138`.
- Compare from `44a7b1af...` to `b096a9c9...` confirms the working-range checkpoint remains the merge-base/ancestor. Intervening Ui work is UiGraph route editing plus additive UiProgressRing source/package/demo/tests/docs; it does not change PropertyEditor, Theme, GroupPanel, UiDoc or RangeSlider contracts used by this closure.

TASK:

Resolve the first R7 validation deviations without weakening acceptance, then resume the Windows/U++ matrix.

TOUCHED BY `d4e83031...`:

- `UiDesigner/UiDesigner/UiDesignerWindowClosure.cpp`
- `tests/PreviewLayoutRegressionTest/main.cpp`
- `tests/ThemeBuilderContractTest/main.cpp`

STATUS: **IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING.**

PUBLISHED:

- `f680416b99bee979af1e363f888f42340f7f9fe2` — complete Designer closure contracts.
- `d9c94b2cf37a4b5fe71492cf0be93d431858f123` — ToolButton icon-only default.
- `689fe825ead08da4d9bb66c598f65921e23032ea` — generated preset-package coverage.
- `eca0683e6e650707050cb717a80aa3c25f68bef6` — closure-test Theme linkage.
- `d4e83031bd0d949aa3aaf554398ec253feabfd43` — preset activation/root placement + Theme contract synchronization repair.

VALIDATION EVIDENCE BEFORE `d4e83031...`:

- Tested Designer head: `95accf9714dd9713af23ba2623c6774111767fb3`.
- Validator observed Ui head during that run: `85415c13d843f9f4e0a54b8ee4335922ddad1688`; Ui has since advanced again to `b096a9c9...`.
- Required ancestors: confirmed.
- `git diff --check`: PASS in both repositories.
- Worktrees: clean.
- PropertyEditorWorkingRange Debug: `checks=10 failed=0`.
- PropertyEditorWorkingRange Release: `checks=10 failed=0`.
- DesignerClosureCatalog Debug: `checks=35 failed=0`.
- DesignerClosureCatalog Release: `checks=35 failed=0`.
- ThemeBuilderContract Debug: `checks=56 failed=0`.
- ThemeBuilderContract Release: `checks=56 failed=0`.
- PreviewLayoutRegression Debug: `checks=39 failed=4`; failures were all downstream of Demo preset insertion rejecting the special Window root.
- Remaining suites/build/manual smoke were correctly stopped.

ROOT CAUSE / REPAIR:

1. Preset activation / Demo root insertion
   - Catalog preset rows intentionally carry `preset:<id>` for drag identity.
   - The old special Presets activation callback passed that envelope directly to the legacy `InsertPreset()` path and that path also requires a normal catalog container spec, so the document `Window` root was rejected.
   - `UiDesignerWindowClosure` now owns Presets activation/drag callbacks and routes click activation through canonical location-aware `InsertPresetAt`, stripping the `preset:` envelope first.
   - Preset drag/drop now uses the same normal `TrackCatalogDrag` / `FinishCatalogDrag` pipeline as other catalog items.
   - `PreviewLayoutRegressionTest` now exercises Demo insertion through `InsertPresetAt(... target=0 ...)`, matching the user-facing activation/root contract. Expected total remains `39/0`.
   - Explicit hierarchy insertion remains on its existing target/container path.

2. ThemeBuilder 56 versus required 57
   - The test contained 56 real checks; no product failure was reported.
   - Acceptance was not lowered and no filler assertion was added.
   - A missing useful contract is now covered: after cycling all four universal roles, Theme toolbar and gallery must remain synchronized on both independent Panel Role and Control Role axes.
   - Expected ThemeBuilderContract total is now `57/0`.

IMPLEMENTATION CONTRACTS THAT REMAIN IN FORCE:

- Sizing editors: legal authored bounds through 10,000; practical slider working range 0–500; max zero = No limit; typed values above 500 remain valid.
- Real `UiRangeSlider` and real `UiNodeGraph` catalog/Preview/codegen integration.
- ToolButton Text is editable and defaults empty/icon-only.
- GroupPanel subtitle/icon metadata with real Preview and generated set/clear icon behavior.
- UiDoc has one canonical scalar `value` shared by Data pane, Preview, undoable Session commits and generated `SetData`.
- Nested Preview selection and existing-node reparenting remain enabled.
- Hierarchy double-click rename releases the manual-drag gesture first.
- Demo preset is a 2x2 four-GroupPanel fixture exercising RangeSlider, UiDoc, UiNodeGraph and ToolButton.
- Catalog/filter painting follows active Ui theme.
- Theme Studio has independent Standard/Subtle/Accent/Alert Panel Role and Control Role axes and isolated `panel|...` / `control|...` style recipes.
- DATA/CHOICES placement and Save containment fixes remain active.
- App/header/version badge use the main brand artwork; visible version is `v1.0.1-RC2`.
- Preset export coverage enumerates all creation/catalog presets and compiles all generated packages.

NEXT ACTION — WINDOWS / U++ VALIDATION:

1. Refresh both repos. Confirm `d4e83031bd0d949aa3aaf554398ec253feabfd43` is an ancestor of current `upp_uidesigner/main` and `44a7b1af0d26730a1f1744b80d4bea319d561bd5` is an ancestor of current `upp_Ui/main`. Inspect intervening changes if either moved.
2. Run `git diff --check` in both repos and require clean worktrees.
3. Because `upp_Ui/main` advanced after the previous working-range runs, rerun PropertyEditorWorkingRange Debug + Release and require `10/0` in both.
4. Rerun DesignerClosureCatalog Debug + Release and require `35/0` in both.
5. Rerun ThemeBuilderContract Debug + Release and require `57/0` in both.
6. Rerun PreviewLayoutRegression Debug + Release and require `39/0` in both.
7. If those focused gates pass, continue `PresetExportTests/BuildPresetFixtures.ps1`; require export `failed=0` and every discovered generated preset package to compile and produce its executable.
8. Continue ThemeAdapterCoverage Debug + Release (`failed=0`, report observed check count), retained ThemeDocument/Label/List-Edit/Dropdown-Accordion suites, principal suites, then UiDesigner Debug + Release builds.
9. Manual Debug smoke remains required for preset activation/drag, four-panel Grid geometry/selectability, nested selection/reparenting, hierarchy rename, ToolButton Text, GroupPanel icon, sizing working range, UiDoc Data, RangeSlider/NodeGraph, Dark catalog, Theme roles/recipe isolation, DATA/CHOICES/Save containment, branding/version.
10. Stop on first substantive failure. Gary may make only bounded mechanical build/link/harness corrections; runtime/architecture/Theme/schema failures return to the supervisor.

HYGIENE:

Temporary refs created during connector staging (`__ignore`, `__ignore2`, and earlier known temporary refs) contain no intended unique Designer work. Branch cleanup remains deferred until final acceptance and must be done only after containment proof with a Git client/API that supports ref deletion.
