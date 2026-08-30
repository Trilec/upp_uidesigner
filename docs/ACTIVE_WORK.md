# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.

## ACTIVE OBJECTIVE
Finish the bounded R7 visual-polish pass. Do not restart the broad matrix until Curt accepts the shell geometry visually.

## CURRENT RECOVERY BASE
- Designer base for this visual correction: `97e3edf437bdf805ec1e05a48762488bd239fe95`.
- Prior visible shell source repair: `796be63309285a03df30582ad5cf0f0d1c90953d`.
- Reusable-controls head observed for this correction: `1780907a1aa50b3a64a4baf70d573718ebe45161`.
- Required dark-marker ancestor remains `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor remains `d0589472daf7fefc458d62458f7caf231d2d8698`.

## DURABLE CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects belong in `upp_Ui`, not Designer workarounds.
- Header identity is project icon + Designer + version before Save/Load/Export.
- Header identity must have no title/card/font underline.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- One application Dark toggle remains authoritative.
- Catalog rows keep their explicit 6 px horizontal paint gutter.
- Shell/page spacing uses a compact 4 px rhythm; do not stack larger container insets around it.
- Side-column and center/Theme action strips remain compact and independently tunable.

## LATEST CURT VISUAL REVIEW
The `97e3edf...` build is materially improved but still NOT accepted.
- PASS: project icon is now visible and Designer catalog containment is much cleaner.
- FAIL: a blue underline still appears directly below `Designer`.
- FAIL: Theme Studio role/palette strip remains too tall.
- FAIL: shell/page spacing is too loose; the 8 px shell gap plus 8 px side-page inset over-expanded the composition.
- Theme Studio sample placement consequently starts too low and reads crowded against neighbouring action chrome.

## CURRENT CORRECTION
- Reduce shared Designer/Theme toolbar height from 53 to 49 px; side toolbar stays 49 px.
- Restore outer shell gap from 8 to 4 px.
- Restore side page inset from 8 to 4 px while keeping the independent 6 px catalog-row gutter.
- Harden the Designer identity against every UiTitleCard line path: disable title/card lines, set both line lengths to NONE, and replace the resolver title font with a plain bold non-underlined font after every theme refresh.
- Preserve the icon PNG resource path and all accepted Theme Studio sample rebalancing.

## PRE-CORRECTION AUTOMATED BASELINE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 packages built, failed=0.

## STATUS
Implementation checkpoint is being published for a fresh Windows Debug build and Curt visual review.

## NEXT ACTION
1. Gary fetches current `upp_uidesigner/main` and `upp_Ui/main`, recording both SHAs.
2. Verify this correction commit is an ancestor of Designer `main`.
3. Build Debug directly to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe` and launch that exact file.
4. Curt checks: no Designer underline, compact 4 px shell spacing, Theme role strip height, Designer side content alignment, Theme Studio panel start/alignment, and Light -> Dark -> Light.
5. If visual PASS, resume R7 closure validation. If FAIL, record only the exact remaining visual defect.
