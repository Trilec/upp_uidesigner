# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical executable location: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
- Only `main` remains as a Designer remote branch.

## ACTIVE OBJECTIVE
Finish R7 Dark-mode visual integration, then let Curt perform the final Light -> Dark -> Light visual check.
The application has one authoritative Dark-mode control: the top-right header toggle.
Do not restore a second Theme Studio mode toggle.

## CURRENT RECOVERY BASE
- Designer dark-integration baseline: `2167d1c942fdb2003d9b26771cbbaff0d7ac334f`.
- Required reusable dark-theme ancestor: `45caf4b7e32d451dd8bb3139a582f8201ba09d6a`.
- Required UiTable themed-scrollbar ancestor: `d0589472daf7fefc458d62458f7caf231d2d8698`.
- Always fetch current heads because both repositories may advance concurrently.

## CURRENT CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects are fixed in `upp_Ui`, not hidden by Designer workarounds.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- Theme recipe identities remain `panel|...` and `control|...`.
- PropertyEditor follows active `UiTheme`; no separate Designer dark palette.
- `UiTable` uses theme-driven `UiScrollBar` internally; no native light scrollbar state.
- UiSplitter supports multiple direct panes; UiQuadSplitter remains capped at four.
- Generated preset packages are temporary and never tracked.
- Build/test executables go directly under `build/`; do not create `out/`, `build/out/`, Debug, or Release subdirectories.

## LATEST AUTOMATED EVIDENCE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 generated packages built, failed=0.
- Retained ThemeDocument, Label, List/Edit, Dropdown/Accordion suites passed in Debug and Release.
- UiDesigner Debug and Release linked before the final visual corrections.

## LAST MANUAL RESULT
Curt confirmed the first Dark repair improved the UI but R7 remained blocked:
- root/application client background still exposed light areas;
- a visible dropdown remained light/stale;
- UiTable scrollbar remained light.
The duplicate Theme Studio Dark toggle should remain removed.

## CURRENT REPAIR
- Paint the UiDesigner root client from the active semantic Surface role.
- Reapply the header theme dropdown from `UiTheme::ResolveDropdown` on theme changes.
- Reusable UiTable scrollbar ownership moved from CtrlLib H/V scrollbars to `UiScrollBar`.
- Validation/build scripts now target the root `build/` directory.
- `docs/ACTIVE_WORK.md` is intentionally compact and must remain 50-100 physical lines.

## TOUCHED SLICE
- `upp_Ui/Ui/UiTable.h`.
- `UiDesigner/UiDesigner/UiDesignerWindow.h`.
- `UiDesigner/UiDesigner/UiDesignerWindowClosure.cpp`.
- `RunSupervisorValidation.ps1`.
- `tests/FoundationTests/BuildGeneratedFixture.ps1`.
- `tests/PresetExportTests/BuildPresetFixtures.ps1`.
- `docs/BUILD.md`.
- `.gitignore`.
- `docs/ACTIVE_WORK.md`.

## STATUS
Implementation checkpoint published for Windows compile and immediate supervisor visual validation.
Do not rerun the complete R7 matrix before the visual check.

## NEXT ACTION
1. Pull current `main` in both repositories.
2. Remove obsolete local `upp_uidesigner\out` only after confirming it contains build products only.
3. Build Debug UiDesigner to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
4. Launch that exact executable and leave it running for Curt.
5. Curt checks Light -> Dark -> Light: root background, header dropdown and popup, Theme Studio dropdown/editor surfaces, table and table scrollbar, and absence of light islands.
6. If visual PASS, finish remaining R7 acceptance bookkeeping; if FAIL, report only the exact remaining visual defect.
