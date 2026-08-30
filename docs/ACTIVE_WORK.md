# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
- Only `main` remains as a Designer remote branch.

## ACTIVE OBJECTIVE
Finish R7 Dark-mode visual integration and let Curt perform the immediate Light -> Dark -> Light visual check.
The application has one authoritative Dark-mode control: the top-right header toggle.
Do not restore a second Theme Studio mode toggle.

## LATEST ICON REPAIR
- The native window and header now use the existing UiDesigner `icon.ico` through
  Windows resource 5555; the shared Ui library brand icon is not substituted.
- Header identity remains icon + Designer + version, with no title/card line and
  no duplicate version icon.

## CURRENT RECOVERY BASE
- Designer repair base: `05447e54570a63fd3977011e01482f2dd4b7d342`.
- Required reusable marker/theme repair: `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor: `d0589472daf7fefc458d62458f7caf231d2d8698`.
- Always fetch current heads because both repositories may advance concurrently.

## CURRENT CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects are fixed in `upp_Ui`, not hidden by Designer workarounds.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- Theme recipe identities remain `panel|...` and `control|...`.
- Inherited Theme Studio fields remain inherited; only explicit authored fields become local style overrides.
- PropertyEditor follows active `UiTheme`; no separate Designer dark palette.
- `UiTable` uses theme-driven `UiScrollBar` internally; no native light scrollbar state.
- Generated preset packages are temporary and never tracked.
- Build/test executables go directly under `build/`; do not create Debug/Release subdirectories.

## LATEST AUTOMATED EVIDENCE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 generated packages built, failed=0.
- Retained ThemeDocument, Label, List/Edit, Dropdown/Accordion suites passed in Debug and Release.
- UiDesigner Debug and Release linked before the latest visual corrections.

## LAST MANUAL RESULT
Curt confirmed the root/client and general Dark integration improved, but three visual defects remained:
- the Theme Studio CHOICES `UiDropdown` sample stayed Light in Dark mode;
- checkbox selected ticks and PropertyEditor inherited/override markers were too dark;
- the header Designer identity showed a line/clipping regression and the version had a redundant icon/location.

## ROOT CAUSE / REPAIR
- `UiDropdown` already has a proper Dark resolver. Theme Studio was materializing every inherited resolver field into `theme_overrides`, converting inherited values into stale custom style. Samples now pass only explicitly authored recipe fields to adapters.
- Reusable `UiCheckBox` default check artwork now uses the resolved indicator ink via mono tint instead of raw icon colour.
- PropertyEditor reset/inherited/override markers now use palette-driven mono tint.
- PropertyEditor recreates embedded value editors when its palette changes so creation-time checkbox/dropdown custom styles cannot survive a Light/Dark transition.
- Header Accent `UiTitleCard` could re-enable its card line during theme refresh. The final shell hook now reasserts no title/card line, tightens icon/title spacing, removes the version icon and orders identity as icon + Designer + version before Save/Load/Export.

## TOUCHED SLICE
- `upp_Ui/Ui/UiCheckBox.cpp`.
- `upp_Ui/Utilities/PropertyEditor/PropertyEditorBase.cpp`.
- `upp_Ui/Utilities/PropertyEditor/PropertyEditorPaint.cpp`.
- `UiDesigner/Theme/UiDesignerThemeBuilderV2.cpp`.
- `UiDesigner/UiDesigner/UiDesignerWindow.cpp`.
- `UiDesigner/UiDesigner/UiDesignerWindowClosure.cpp`.
- `docs/ACTIVE_WORK.md`.

## STATUS
Implementation checkpoint prepared/published for Windows compile and immediate supervisor visual validation.
Do not rerun the complete R7 matrix before Curt's visual check.

## NEXT ACTION
1. Pull current `main` in both repositories.
2. Build Debug UiDesigner to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
3. Launch that exact executable and leave it running for Curt.
4. Curt checks Theme Studio CHOICES dropdown face/popup, checkbox selected tick, PropertyEditor markers, header identity/version ordering and Light -> Dark -> Light restoration.
5. If visual PASS, finish remaining R7 acceptance bookkeeping; if FAIL, report only the exact remaining visual defect.
