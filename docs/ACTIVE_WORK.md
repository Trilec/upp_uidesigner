# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
- Build/test executables stay directly under `build/`.

## ACTIVE OBJECTIVE
Finish the bounded R7 shell/layout correction and hand one fresh Debug executable to Curt for visual inspection.
Do not restart the broad R7 validation matrix until this visual gate passes.

## CURRENT RECOVERY BASE
- Designer visual-review base: `434cd73d89ae1bce8c7ede9764db1a2ca0af7b88`.
- Its parent icon checkpoint: `42d89a9dff06dd7457b5f12c43b16830ac377512`.
- Reusable controls continue independently on `upp_Ui/main`; fetch before Windows build.
- Required reusable dark-marker ancestor remains `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor remains `d0589472daf7fefc458d62458f7caf231d2d8698`.

## DURABLE CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects belong in `upp_Ui`, not Designer workarounds.
- One authoritative Dark toggle remains in the top-right application header.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- Inherited Theme Studio fields stay inherited; only authored values become local overrides.
- Header identity is project icon + Designer + version before Save/Load/Export.
- Header identity has no TitleCard face/frame/shadow/title line/card line.
- Side-column chrome sizing is independent from the center preview toolbar.
- Catalog rows must have a visible horizontal gutter inside their containing panel.
- `docs/ACTIVE_WORK.md` remains a short recovery document, not project history.

## LATEST CURT VISUAL RESULT
Curt visually checked the build based on `434cd73...` and it is still NOT accepted.
- PASS: the unwanted blue line below Designer is gone.
- FAIL: the project icon is still absent from the header identity.
- FAIL: the left/right side action strips still read too tall.
- FAIL: the catalog rows still appear flush against the panel edge; the requested inset is not visibly present.
- Theme Studio composition changes cannot be accepted from the Designer-only screenshot and remain pending visual confirmation.

## ROOT CAUSE OF THE MISSED VISUAL FIX
- The header used `Win32Icon(5555)` as an in-client `Image`; the screenshot proves that path did not provide visible media in this shell.
- The prior 4 px page-stack inset did not alter the actual full-width catalog row rectangles.
- Side-column height still reused the shared Designer toolbar metric rather than having its own compact metric.

## CURRENT CORRECTION
- Embed the existing package `icon.png` through `UiDesignerBrand.brc` and decode it with `PNGRaster` for the header/client image.
- Keep resource 5555 only as fallback/native resource compatibility.
- Add `plugin/png` to the UiDesigner package dependency slice.
- Add an independent `SideToolbarHeight()` of 49 px; retain the center Designer toolbar at 53 px.
- Use the side metric for open and closed side-column chrome.
- Increase side page inset to 8 px.
- Give every catalog row an explicit 6 px left/right gutter in `UiDesignerCatalogList::ItemRect()`.
- Preserve the existing footer inset and no-line header styling.
- Preserve the Theme Studio rebalancing already present in `434cd73...`: no Feedback duplication, Choices shortened/moved, Navigation raised, Table separated from Data.

## LATEST AUTOMATED EVIDENCE BEFORE THIS VISUAL CORRECTION
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 generated packages built, failed=0.

## STATUS
Source correction commit prepared as `1cedbc135a625773b225f3f0e7b6def48dcb1802`; Windows compile and Curt visual acceptance remain pending.

## NEXT ACTION
1. Verify `1cedbc135a625773b225f3f0e7b6def48dcb1802` is on Designer `main`.
2. Gary fetches current `upp_Ui/main` and `upp_uidesigner/main` and records both SHAs.
3. Gary builds Debug directly to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
4. Gary launches that exact executable and leaves it running.
5. Curt checks header icon, compact side strips, visible catalog gutter, footer inset, Theme Studio rebalancing, and Light -> Dark -> Light.
6. If visual PASS, resume R7 closure; if FAIL, record the exact remaining visual defect without broadening scope.
