# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

Historical Theme acceptance and earlier Designer recovery checkpoints remain in Git history. This file is the current recovery checkpoint.

## R7 DARK THEME INTEGRATION — 2026-08-29

BASE:

- Tested `upp_uidesigner/main`: `2f9838fdc7bbda7c3773acdfb9d7807e84d31ef9`.
- Tested `upp_Ui/main`: `9ca0c26ad1e46aacd2bcd67578bf9e2c9c5b1c4c`.

TASK:

- Repair the Light/Dark transition path for Designer-owned chrome,
  PropertyEditor surfaces, and Theme Studio sample controls.

ROOT CAUSE / REPAIR:

- `PropertyEditorStyle::System()` used the platform paper colour when the
  active UiTheme's Subtle panel was intentionally transparent. It now falls
  back to the semantic Surface panel, so the PropertyEditor gets an opaque
  dark surface in Dark mode while retaining the existing Light behaviour.
- The reusable `UiTheme::ResolveTable()` resolver returned its default white
  table surface in Dark mode. It now resolves table/header/row/selection
  chrome from the same semantic Surface, Subtle, List and role palettes used
  by the live table control.
- Basic Theme Studio samples now rebuild their style from the active theme
  resolver instead of copying stale custom state from the previous mode. This
  closes the dropdown/slider mixed-palette path, including non-normal states.
- Designer shell page surfaces, side-column tool chrome, reference pills and
  gallery scrolling are reapplied from the active theme after every mode or
  theme edit. The Theme Studio toolbar no longer shows a duplicate mode icon;
  mode ownership remains with the single header toggle.

TOUCHED:

- `upp_Ui/Utilities/PropertyEditor/PropertyEditorBase.cpp`
- `upp_Ui/Ui/UiTheme.h`
- `UiDesigner/Theme/UiDesignerThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerThemeBuilderV2.cpp`
- `UiDesigner/UiDesigner/UiDesignerSideWidgets.cpp`
- `UiDesigner/UiDesigner/UiDesignerWindow.cpp`
- `tests/ThemeDarkIntegrationTest/`
- `docs/ACTIVE_WORK.md`

VALIDATION:

- `ThemeDarkIntegrationTest` Debug and Release: `checks=18 failed=0`.
- `ThemeBuilderContractTest` Debug and Release: `checks=57 failed=0`.
- `UiThemeSurfaceRegressionTest` Debug and Release: `Checks: 13, Fails: 0`.
- UiDesigner Debug and Release both linked successfully.
- Automated coverage confirms Light/Dark PropertyEditor, dropdown, slider and
  table surfaces are materially different and return correctly after a
  repeated mode switch.
- A final interactive visual pass was not repeated after the last rebuild
  because the desktop Computer Use session was stopped with the physical
  Escape key. The built Debug executable is available for the supervisor's
  visual check.

STATUS: **DARK THEME INTEGRATION REPAIR COMPLETE — SUPERVISOR VISUAL REVIEW PENDING.**

PUBLISHED:

- Local commits are prepared only after review; nothing from this repair has
  been pushed by this task.

## DESIGNER R7 SPLITTER PRESET REPAIR — 2026-08-29

BASE:

- UiDesigner base: `b3d21c131c03b7a4fba58e8927924dec13bbbe5a`.
- Dependency used for focused validation: `upp_Ui 9d6d8240f197f9d3227bd564429f1c7290db0c73`.

TASK:

- Remove the stale two-pane UiSplitter Designer restriction and prove the
  complete generated preset-package path.

TOUCHED:

- `UiDesigner/Catalog/UiDesignerCatalog.cpp`
- `UiDesigner/Services/UiDesignerDrop.cpp`
- `tests/UiSplitterCatalogTest/main.cpp`
- `tests/UiSplitterCatalogTest/UiSplitterCatalogTest.upp`
- `tests/PresetExportTests/main.cpp`
- `tests/PresetExportTests/.generated-presets/` removed from source control
- `.gitignore`
- `docs/ACTIVE_WORK.md`

STATUS: **REPAIR COMPLETE — BROADER R7 VALIDATION REMAINS SEPARATE.**

PUBLISHED:

- Local commit pending supervisor review; not pushed.

VALIDATION:

- `UiSplitterCatalogTest` Debug and Release: `checks=24 failed=0`.
- Preset export: creation `3`, catalog `9`, total `12`, `failed=0`.
- All 12 generated preset packages compiled and linked successfully.
- The successful harness removed `.generated-presets`.

NEXT ACTION:

- Review the local repair commit, then run the broader R7 validation matrix
  separately. Do not treat this focused repair as full Designer acceptance.

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
