# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
- Designer uses `main` only; generated executables go directly under `build/`.

## ACTIVE OBJECTIVE
Finish the current R7 visual-polish pass and hand one Debug executable to Curt for immediate inspection.
Gary only pulls current `main`, compiles, launches and reports mechanical failures; he does not redesign the UI.

## CURRENT RECOVERY BASE
- Designer base for this repair: `1a01daa72e55899f06bff4ffcc49a489806d6317`.
- Current reusable-controls head observed before this repair: `0fd8df299bca20d27e19d755693f65eff4dcdca6`.
- Required reusable dark-marker repair ancestor: `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor: `d0589472daf7fefc458d62458f7caf231d2d8698`.
- Always fetch both heads because `upp_Ui` advances independently.

## CURRENT CONTRACTS
- `UiDesigner -> upp_Ui`; reusable-control defects belong in `upp_Ui`, not Designer workarounds.
- One authoritative Dark toggle remains in the top-right application header.
- Theme Studio roles remain Standard/Subtle/Accent/Alert with independent Panel and Control axes.
- Inherited Theme Studio fields stay inherited; only authored values become local overrides.
- UiTable uses theme-aware `UiScrollBar`; PropertyEditor follows active `UiTheme`.
- UiDesigner branding uses the project-owned `UiDesigner/UiDesigner/icon.ico` and `icon.png`, never a generic shared brand icon.
- Header identity is icon + Designer + version before Save/Load/Export, with no title/card underline and no duplicate version icon.
- Side/tool strips use the compact toolbar height; page content remains inset within visible panel chrome.
- `docs/ACTIVE_WORK.md` stays 50-100 physical lines and contains current recovery state only.

## LATEST VERIFIED AUTOMATED EVIDENCE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0`.
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 generated packages built, failed=0.
- Retained ThemeDocument, Label, List/Edit, Dropdown/Accordion suites passed Debug/Release.

## LATEST CURT VISUAL REVIEW
- Dark resolver work is materially improved.
- Remaining shell regressions: missing/wrong header icon, residual line under Designer, oversized side/Theme Studio tool strips, catalog rows touching/crossing panel chrome, and footer text touching its border.
- Theme Studio composition also needs less duplication and more usable Data/Table space.

## CURRENT REPAIR
- Embed the existing project `icon.png` through a U++ `.brc`; keep `icon.ico` as the executable resource and use the same project artwork for the header/window image.
- Make the header identity visually flat: no TitleCard face/frame/title line/card line; keep version immediately after Designer.
- Reduce shared Designer toolbar height from 63 to 53 px.
- Inset side-column page stacks by 4 px and footer status text by 6 px.
- Remove visible FEEDBACK duplication by reusing that group shell as a dedicated TABLE sample.
- Keep DATA as List + Tree at half-width each.
- Move CHOICES below Inputs at 158 px high; move NAVIGATION to the top of the third column; place TABLE below it at full width.

## STATUS
Implementation checkpoint is being prepared for a single Windows compile and Curt visual review.
Do not restart the broad R7 matrix before this visual pass.

## NEXT ACTION
1. Publish and verify the coherent Designer checkpoint.
2. Gary pulls current `upp_Ui/main` and `upp_uidesigner/main`.
3. Gary builds Debug directly to `E:\apps\github\upp_uidesigner\build\UiDesigner.exe` and launches that exact file.
4. Curt checks header icon/line/version, compact panel strips, list containment/footer inset, Theme Studio Choices/Data/Navigation/Table layout, and Light -> Dark -> Light.
5. If visual PASS, close R7 bookkeeping; if FAIL, record only the exact remaining visual defect.
