# UiDesigner

**UiDesigner is a visual authoring environment for building Ultimate++ interfaces with the `upp_Ui` control library.**

It is a standalone application repository. It builds against `upp_Ui` as an
external dependency rather than being bundled inside it.

## Purpose

- visually build interfaces using `upp_Ui` controls;
- maintain a hierarchy of controls;
- edit properties through the Inspector;
- preview responsive layouts;
- work with themes/styles;
- generate/export application UI definitions/code where supported.

The Designer is catalogue-centred: stable native `Ui` controls, composites and
presets are registered in a typed catalog, the session keeps a canonical
document/theme separation, and preview, export and code generation all derive
from that single source of truth.

## Architecture

UiDesigner is its own application repository and consumes `upp_Ui` as an
external dependency:

```text
upp_uidesigner
      ↓
    upp_Ui
```

Shared packages that remain in `upp_Ui` and are consumed through the assembly:

- `Ui` — the `upp_Ui` control library;
- `Utilities/PropertyEditor` and `Utilities/PropertyEditorCore` — Inspector/property-editor infrastructure.

The internal production packages are `UiDesigner/Core`, `UiDesigner/Commands`,
`UiDesigner/Catalog`, `UiDesigner/Preview`, `UiDesigner/CodeGen`,
`UiDesigner/ThemeCore`, `UiDesigner/Theme`, `UiDesigner/Services`,
`UiDesigner/CLI`, `UiDesigner/MCP` and the `UiDesigner/UiDesigner` application.
See `docs/README.md` for the package responsibilities.

The assembly also lists `upp_animation` and `upp_statemachine`, which are
transitive dependencies of the `Ui` library. No source from `upp_Ui` is vendored
into this repository.

## Repository layout

```text
UiDesigner/    production packages
tests/         Designer validation packages
examples/      Designer examples
docs/          design and architecture documentation
build/         ignored build output
github.var     U++ assembly definition
```

## Build instructions

The local assembly is `github.var`. From the repository root:

```bat
E:\upp-18468\umk.exe github "UiDesigner/UiDesigner" CLANGx64 -br +GUI E:\apps\github\upp_uidesigner\build\UiDesigner.exe
```

The application compiles entirely from this repository's `UiDesigner` sources
plus the declared external dependencies; nothing is compiled from the old
`upp_Ui\Utilities\UiDesigner` package.

CLI and MCP console builds:

```bat
E:\upp-18468\umk.exe github "UiDesigner/CLI" CLANGx64 -br E:\apps\github\upp_uidesigner\build\uidesigner_cli.exe
E:\upp-18468\umk.exe github "UiDesigner/MCP" CLANGx64 -br E:\apps\github\upp_uidesigner\build\uidesigner_mcp.exe
```

Note: at the current source HEAD the `CLI` and `MCP` packages compile but fail to
link because the theme-adapter entry points they reference live in the
`UiDesigner/Theme` package, which is not in their dependency graph. This is a
pre-existing condition in the original `upp_Ui` checkout as well, reproduced
unchanged by this migration.

## Tests

Build the migrated test packages:

```bat
E:\upp-18468\umk.exe github "Tests" CLANGx64 -br +GUI E:\apps\github\upp_uidesigner\build\UiDesignerTests.exe
E:\upp-18468\umk.exe github "RegressionTests" CLANGx64 -br +GUI E:\apps\github\upp_uidesigner\build\UiDesignerRegressionTests.exe
E:\upp-18468\umk.exe github "FoundationTests" CLANGx64 -br E:\apps\github\upp_uidesigner\build\UiDesignerFoundationTests.exe
E:\upp-18468\umk.exe github "PresetExportTests" CLANGx64 -br E:\apps\github\upp_uidesigner\build\UiDesignerPresetExportTests.exe
```

Run the deterministic suites:

```bat
E:\apps\github\upp_uidesigner\build\UiDesignerTests.exe
E:\apps\github\upp_uidesigner\build\UiDesignerFoundationTests.exe
```

Current deterministic outcomes (identical to the original `upp_Ui` baseline):

- `Tests`: 1324 checks, 12 fails;
- `FoundationTests`: 72 checks, 18 fails.

`RegressionTests` currently does not compile at source HEAD (a pre-existing
`ordered_legacy` redefinition), reproduced unchanged by this migration.

The generated-package and preset fixture proofs are driven by:

```bat
powershell -ExecutionPolicy Bypass -File tests/FoundationTests/BuildGeneratedFixture.ps1
powershell -ExecutionPolicy Bypass -File tests/PresetExportTests/BuildPresetFixtures.ps1
```

The complete automated validation sequence is `RunSupervisorValidation.ps1`
(see `docs/BUILD.md`).

## Licence

See the repository `LICENSE`.
