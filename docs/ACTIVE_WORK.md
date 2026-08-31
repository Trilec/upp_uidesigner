# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkout: `E:\apps\github\upp_uidesigner`; reusable checkout: `E:\apps\github\upp_Ui`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical Designer executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.

## TASK
Close R7 theme-ownership cleanup. Theme/preset/role/Light-Dark changes may alter visual styling, but must not silently create, remove, move or change authored structural/configuration choices.

## PUBLISHED CHECKPOINT
- Required `upp_Ui/main`: `e7197e5698b7e1b38ab75935f598a1cf329d287e`.
- Designer implementation checkpoint: `389f5abc058ef1c6c1c7e29d59a9141254d7dc75`.
- This recovery-doc commit is expected to be a direct Designer descendant of that checkpoint.
- Retained dark-marker ancestor: `337d29993dc3e96537d6c429758e5ca573a4621e`.
- Retained themed-scrollbar ancestor: `d0589472daf7fefc458d62458f7caf231d2d8698`.

## OWNERSHIP CONTRACT
Theme may own palette/fill/ink, fonts/typography, frame appearance, radius, shadows, paint metrics, and colours used by an optional element when enabled.
Control/document configuration owns optional-element visibility, line/divider existence and mode, icon/media placement, orientation, requested Tab visual family, ScrollBar arrow presence/layout, List separator/badge modes, and equivalent structural choices.
Rule: Style fields may remain for local/custom-style compatibility, but Theme Studio must not become a second owner of structure.

## IMPLEMENTED — `upp_Ui`
- Public `UiTheme` enforces the structural contract at resolver boundaries for Button, ToolButton, Toggle, ScrollBar, GroupPanel, Dropdown, Tab, TitleCard and List.
- ToolButton uses its own centered structural defaults rather than Button defaults.
- Requested Tab visual is reapplied after role tuning so enum, geometry and paint mode agree.
- TitleCard keeps authored divider/media/text-placement structure; theme still provides colours for enabled dividers.
- Resolver implementation is intentionally isolated as `UiThemeResolverImpl.h`; temporary `UiThemeLegacy.h` naming is removed.
- Focused regression package: `Utilities/UiThemeStructureContractTest`.

## IMPLEMENTED — UIDESIGNER
- Structural Theme overrides removed for Button-family content placement, GroupPanel header mode, Tab visual/placement aliases, TitleCard/Accordion divider-media structure, and List separator/badge modes.
- Normal authored Appearance/configuration properties remain available where applicable.
- `ThemeAdapterCoverageTest` now checks the live Theme schema/adapters and explicit structural exclusions instead of retaining stale field inventory.
- Added `tests/ThemeStructureOwnershipTest`.
- `ThemeDarkIntegrationTest` now returns a failing process exit code when checks fail.
- Catalog base normalization is intentionally isolated as `UiDesignerInspectorCatalogImpl.inc`; temporary `Legacy` naming is removed.

## IMPORTANT NON-REGRESSIONS
- Do not restore theme-driven TitleCard card-line visibility for Standard/Accent roles.
- Do not globally hide the default TitleCard title divider; individual controls such as Designer brand may disable it explicitly.
- Do not reintroduce fake/nested UiSplitter limits; UiSplitter remains multi-pane and UiQuadSplitter remains max four.
- Do not move retired structural choices back into Theme Studio because a compatible Style member still exists.

## STATUS
IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING. Source and tests are published; this session cannot execute the Windows U++/CLANGx64 binaries.

## GARY NEXT
1. Pull both `main` branches with `--ff-only`; record exact SHAs. `upp_Ui` must be `e7197e5698b7e1b38ab75935f598a1cf329d287e` or a descendant; Designer must be `389f5abc058ef1c6c1c7e29d59a9141254d7dc75` or a descendant.
2. Build/run `Utilities/UiThemeStructureContractTest` Debug and Release; require exit 0 / failed=0.
3. Build/run Designer `tests/ThemeStructureOwnershipTest`, `tests/ThemeAdapterCoverageTest`, `tests/ThemeDarkIntegrationTest` and `tests/ThemeBuilderContractTest` Debug and Release; require exit 0 / failed=0. Coverage check count may change only because retired structural Theme fields are no longer part of the schema.
4. Build canonical UiDesigner Debug to `build\UiDesigner.exe`, launch it, and leave it running for Curt.
5. Only fix an obvious mechanical compile/include/signature/package-link issue locally if required; publish that small fix and report the new SHA. Any architecture/theme/state behavior issue comes back to supervisor.
6. Report exact heads, build/test results, clean/dirty tree, any source edit, and whether the app is left running. Do not broaden into unrelated validation or redesign.
