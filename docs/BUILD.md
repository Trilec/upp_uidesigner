# Build and validation

## Complete release-candidate validation

The Windows host must first refresh both authoritative dependencies:

```text
Trilec/upp_uidesigner main
Trilec/upp_Ui main
```

Record both exact SHAs before drawing conclusions. UiDesigner is deliberately validated against the **current** reusable-control main rather than a remembered dependency checkpoint.

From the UiDesigner repository root:

```powershell
powershell -ExecutionPolicy Bypass -File RunSupervisorValidation.ps1
```

Optional parameters select the `umk` path, assembly, configuration and output folder.
The canonical default output folder is `E:\apps\github\upp_uidesigner\build`; every executable is written directly at that root, without Debug or Release subdirectories.

The runner performs:

1. architecture guard;
2. PropertyEditorCore probe;
3. PropertyEditor tests;
4. original UiDesigner tests;
5. RegressionTests;
6. FoundationTests;
7. Theme structure-ownership regression;
8. Theme adapter live-schema coverage;
9. Dark integration regression;
10. Theme Builder contract regression;
11. UiSplitter catalog regression;
12. CLI build and catalog/schema smoke;
13. MCP build plus newline and Content-Length framing smoke;
14. canonical UiDesigner GUI build;
15. generated complete-package export/build/process smoke;
16. blank/three-pane/dialog preset generated-package builds.

Every deterministic executable is required to return process exit 0. Do not restore retired Theme fields or weaken a regression merely to reproduce an obsolete historical check count.

The automated runner is the source-level/release gate; a visible desktop session is still required for the final interactive audit.

## Package/link ownership

`UiDesigner/Services` remains a reusable headless application-service package and does **not** depend on `UiDesigner/Theme`, because Theme already reaches Services through Preview and that reverse dependency would create a cycle.

`UiDesigner/CLI` and `UiDesigner/MCP` are final executable link roots. They depend on both `UiDesigner/Services` and `UiDesigner/Theme` so the Theme adapter entry points used by `UiDesignerSession` are present at link time without duplicating state or creating a package cycle.

## Individual focused packages

```text
Utilities/PropertyEditorCoreProbe
Utilities/PropertyEditorTests
Tests
RegressionTests
FoundationTests
tests/ThemeStructureOwnershipTest
tests/ThemeAdapterCoverageTest
tests/ThemeDarkIntegrationTest
tests/ThemeBuilderContractTest
tests/UiSplitterCatalogTest
UiDesigner/CLI
UiDesigner/MCP
UiDesigner/UiDesigner
```

Dependent library packages build automatically.

Representative commands:

```powershell
E:\upp-18468\umk.exe github "tests/ThemeStructureOwnershipTest" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\UiDesignerThemeStructureOwnershipTest.exe"
E:\upp-18468\umk.exe github "tests/ThemeAdapterCoverageTest" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\UiDesignerThemeAdapterCoverageTest.exe"
E:\upp-18468\umk.exe github "tests/ThemeDarkIntegrationTest" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\UiDesignerThemeDarkIntegrationTest.exe"
E:\upp-18468\umk.exe github "tests/ThemeBuilderContractTest" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\UiDesignerThemeBuilderContractTest.exe"
E:\upp-18468\umk.exe github "UiDesigner/CLI" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\uidesigner_cli.exe"
E:\upp-18468\umk.exe github "UiDesigner/MCP" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\uidesigner_mcp.exe"
E:\upp-18468\umk.exe github "UiDesigner/UiDesigner" CLANGx64 -br +GUI "E:\apps\github\upp_uidesigner\build\UiDesigner.exe"
```

## Generated package proof

```powershell
powershell -ExecutionPolicy Bypass -File tests/FoundationTests/BuildGeneratedFixture.ps1
powershell -ExecutionPolicy Bypass -File tests/PresetExportTests/BuildPresetFixtures.ps1
```

Generated source fixtures stay in their temporary test directories; generated executables go directly to `build/`.

## Final interactive audit

After the automated gate passes, leave the newly built `build\UiDesigner.exe` running and verify at minimum:

- project icon and compact Designer header are correct;
- no title/card divider is reintroduced under `Designer`;
- shell/page spacing retains the intended ~4 px rhythm and catalog row gutter;
- Theme Studio top role/palette strip and sample layout remain compact/aligned;
- Light -> Dark -> Light preserves authored structural choices;
- toolbox/hierarchy/canvas add, move and reparent paths remain coherent;
- Inspector preview/commit/reset and Behavior Inspector work;
- save/load/export dialogs and user-code preservation work;
- generated code/package output remains compile-valid;
- no crash/assert during normal workspace/theme switching.

Only after this gate and the visual audit pass should the release candidate be accepted.
