# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical Designer executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.

## ACTIVE OBJECTIVE
Close the R7 theming-ownership cleanup, then resume the retained validation matrix. Theme changes must remain visual; authored structure/configuration must not silently change with a preset, role, Light/Dark switch, or Theme Studio refresh.

## CURRENT CHECKPOINT
- `upp_Ui` implementation head before platform validation: `27d98c3936c1b63bf582ed2bfd5288304363e610`.
- UiDesigner implementation/test head before this recovery-doc update: `ca140403e1dc6d5bca35bc694c208f2d0e9615e0`.
- Required reusable TitleCard layout-fix ancestor: `1e6907c4fe6e15adeb9daed06d5a12fd884e3f39`.
- Required dark-marker ancestor: `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Required UiTable themed-scrollbar ancestor: `d0589472daf7fefc458d62458f7caf231d2d8698`.

## THEME OWNERSHIP CONTRACT
Theme may own:
- palette/fill/ink and state colours;
- fonts and theme typography;
- frame appearance, radius, shadows and paint metrics;
- colours used to style an optional element when that element is enabled.

Control/document configuration owns:
- optional element visibility;
- divider/line existence and authored geometry/mode;
- icon identity and icon/media placement;
- orientation and requested tab visual family;
- scrollbar arrow presence/layout;
- list separator/badge presentation modes;
- equivalent structural choices on future controls.

Rule: a theme may style an element, but must not create, remove, move or change the authored mode of that element.

## IMPLEMENTED IN `upp_Ui`
- Public `UiTheme` now fronts the retained resolver implementation and reapplies the structural ownership contract at resolver boundaries.
- Covered resolver families: Button, ToolButton, Toggle, ScrollBar, GroupPanel, Dropdown, Tab, TitleCard and List.
- UiTab reapplies the caller-requested visual recipe after role tuning so style geometry agrees with the requested visual, not only the enum value.
- UiTitleCard theme resolution keeps its real defaults: title divider ON, card divider OFF; title/card line geometry, media/text placement and media-auto-fit stay authored configuration.
- Theme-provided line colours remain available to style dividers when the caller enables them.
- Focused package added: `Utilities/UiThemeStructureContractTest`.

## IMPLEMENTED IN UIDESIGNER
- TitleCard structural fields that already have normal authored Appearance properties are removed from the Theme override surface.
- The same removal propagates to semantic Accordion sections that reuse TitleCard appearance.
- List row-separator visibility and right-text badge mode are no longer exposed as Theme overrides.
- Existing normal TitleCard authoring paths remain unchanged.
- Focused package added: `tests/ThemeStructureOwnershipTest`.
- Catalog package membership records the retained inspector-catalog implementation checkpoint used by the small ownership wrapper.

## IMPORTANT NON-REGRESSIONS
- Do not restore theme-driven `UiTitleCard` card-line visibility for Standard/Accent roles.
- Do not hide the default TitleCard title divider globally; Designer's brand header intentionally disables both dividers as an instance choice.
- Do not reintroduce nested/fake UiSplitter restrictions; UiSplitter remains multi-pane and UiQuadSplitter remains max four.
- Do not move authored structural choices back into Theme Studio merely because their Style fields still exist for local/custom style compatibility.

## PRE-CLEANUP AUTOMATED BASELINE
- ThemeDarkIntegrationTest Debug/Release: `18/0`.
- ThemeBuilderContractTest Debug/Release: `57/0`.
- UiThemeSurfaceRegressionTest Debug/Release: `13/0`.
- ThemeAdapterCoverageTest Debug/Release: `3757/0` (count may legitimately reduce because retired structural Theme fields no longer participate).
- UiSplitterCatalogTest Debug/Release: `24/0`.
- Preset export: 12/12 packages built, failed=0.

## STATUS
IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING. GitHub source changes are on `main`; this session cannot run the Windows U++ 18468/CLANGx64 binaries.

## NEXT ACTION
1. Fetch current `upp_Ui/main` and `upp_uidesigner/main`; record exact SHAs and verify the Designer dependency contains `27d98c3936c1b63bf582ed2bfd5288304363e610` or a descendant.
2. Build/run `Utilities/UiThemeStructureContractTest` Debug and Release; expect failed=0.
3. Build/run `tests/ThemeStructureOwnershipTest` Debug and Release; expect failed=0.
4. Re-run ThemeAdapterCoverageTest and ThemeBuilderContractTest Debug/Release; a lower coverage check count is acceptable only from the deliberately retired Theme fields, with failed=0.
5. Build the canonical UiDesigner Debug executable and visually switch preset, role, Designer/Theme workspace and Light/Dark. Confirm authored divider/media/tab/list structure does not change.
6. If focused validation passes, resume the retained R7 closure matrix rather than broadening this fix into unrelated demos.
