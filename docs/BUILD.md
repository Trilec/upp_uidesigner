# Build and validation

## Complete validation sequence

From the repository root on the Windows U++ host:

```powershell
powershell -ExecutionPolicy Bypass -File RunSupervisorValidation.ps1
```

Optional parameters select the `umk` path, assembly, configuration and output folder.
The canonical default output folder is `E:\apps\github\upp_uidesigner\build`; every executable is written directly at the root of that folder, without Debug or Release subdirectories.

The runner performs:

1. architecture guard;
2. PropertyEditorCore probe;
3. PropertyEditor tests;
4. original UiDesigner tests;
5. UiDesigner FoundationTests;
6. CLI and MCP builds;
7. UiDesigner GUI build;
8. CLI catalog/schema smoke;
9. MCP newline and Content-Length framing smoke;
10. generated-package export, `umk` build and process smoke.

Interactive visual and drag/drop validation still requires a visible desktop session.

## Individual build order

```text
Utilities/PropertyEditorCoreProbe
Utilities/PropertyEditorTests
Tests
FoundationTests
UiDesigner/CLI
UiDesigner/MCP
UiDesigner/UiDesigner
```

The dependent library packages build automatically.

Representative commands:

```powershell
E:\upp-18468\umk.exe github "Tests" CLANGx64 -br +GUI "E:\apps\github\upp_uidesigner\build\UiDesignerTests.exe"
E:\upp-18468\umk.exe github "FoundationTests" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\UiDesignerFoundationTests.exe"
E:\upp-18468\umk.exe github "UiDesigner/CLI" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\uidesigner_cli.exe"
E:\upp-18468\umk.exe github "UiDesigner/MCP" CLANGx64 -br "E:\apps\github\upp_uidesigner\build\uidesigner_mcp.exe"
E:\upp-18468\umk.exe github "UiDesigner/UiDesigner" CLANGx64 -br +GUI "E:\apps\github\upp_uidesigner\build\UiDesigner.exe"
```

## Generated package proof

```powershell
powershell -ExecutionPolicy Bypass -File tests/FoundationTests/BuildGeneratedFixture.ps1
```

The fixture is deleted after success and retained under `.generated-smoke` after failure.

Creation/catalog preset package proof uses:

```powershell
powershell -ExecutionPolicy Bypass -File tests/PresetExportTests/BuildPresetFixtures.ps1
```

Generated source fixtures stay under their temporary test directories; generated executables go directly to the root of `build/`.
