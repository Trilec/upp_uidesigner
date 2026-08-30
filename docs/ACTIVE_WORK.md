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
- Designer base before TitleCard ownership repair: `4309e40350383e5983496c73cbe27b578d8fafe0`.
- Reusable-controls base before TitleCard layout fix: `1780907a1aa50b3a64a4baf70d573718ebe45161`.
- Published reusable TitleCard layout fix: `1e6907c4fe6e15adeb9daed06d5a12fd884e3f39`.
- Required dark-marker ancestor remains `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor remains `d0589472daf7fefc458d62458f7caf231d2d8698`.

## DURABLE CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects belong in `upp_Ui`, not Designer workarounds.
- `UiTitleCard::ShowTitleLine(false)` alone hides the title divider; line length/style remain configuration for when enabled.
- `UiTitleCard::ShowCardLine(false)` independently hides the card divider.
- Card-line geometry changes must invalidate layout because they affect minimum size/content-cell placement.
- Header identity is project icon + Designer + version before Save/Load/Export.
- Designer brand keeps Accent theme styling but overrides both TitleCard visibility booleans to false.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- One application Dark toggle remains authoritative.
- Catalog rows keep their explicit 6 px horizontal paint gutter.
- Shell/page spacing uses one compact 4 px outer rhythm; do not stack larger container insets around it.
- Side-column and center/Theme action strips remain compact and independently tunable.

## TITLECARD AUDIT RESULT
- Paint correctly checks `title_line` and `card_line` independently in both content-cell and no-content-cell paths.
- The visible Designer line was not evidence that `ShowTitleLine(false)` itself was broken.
- Minimal/role Accent TitleCard resolution intentionally enables the card divider.
- `ApplyThemeToShell()` reapplies that generic Accent recipe during workspace/theme actions, which can overwrite the Designer instance override.
- Prior defensive changes that set line lengths to `NONE` and replace the title font were therefore the wrong ownership fix.

## CURRENT FIXES
- `upp_Ui`: `SetCardLine`, `SetCardLineSide`, and `ShowCardLine` now call `RefreshLayout()` before repaint; visibility semantics are unchanged.
- UiDesigner: the brand keeps normal resolver line lengths/styles and only sets `title_line=false` and `card_line=false`.
- UiDesigner reasserts the brand instance override after Designer/Theme mode, preset, Dark-toggle, Theme Inspector, and theme-model refresh paths.
- Existing project icon PNG/resource handling is unchanged.

## SPACING REGRESSION ROOT CAUSE
- The offset was Designer-owned, not a UiTheme inset change.
- A prior pass raised the shared shell `Gap()` from 4 to 8 px.
- It also raised the side-page inset from 4 to 8 px while Theme Studio already had its own inner spacing.
- Those values stacked, pushing Theme Studio content down and making action/content panels read oversized or crowded.
- Current shell `Gap()` and side-page inset are back to 4 px; catalog-row gutter remains a separate 6 px local paint inset.

## PRE-FIX AUTOMATED BASELINE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 packages built, failed=0.

## STATUS
Reusable TitleCard fix is published. Designer TitleCard ownership repair is being published for a fresh Windows Debug build and Curt visual review.

## NEXT ACTION
1. Gary fetches current `upp_uidesigner/main` and `upp_Ui/main`, recording both SHAs.
2. Build Debug directly to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe` and launch that exact file.
3. Curt checks that the Designer header has no title or card divider through Designer/Theme/Dark switches.
4. Curt rechecks compact Theme Studio spacing and role/palette strip alignment.
5. If visual PASS, resume R7 closure validation. If FAIL, record only the exact remaining visual defect.
