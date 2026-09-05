# UiDesigner

**UiDesigner is a visual authoring environment for building Ultimate++ interfaces with the `upp_Ui` control library.**

It is a standalone application repository. It builds against `upp_Ui` as an external dependency rather than being bundled inside it.

## Purpose

- visually build interfaces using `upp_Ui` controls;
- maintain a hierarchy of controls;
- edit properties through the Inspector;
- preview responsive layouts;
- work with themes/styles;
- generate/export application UI definitions/code where supported.

The Designer is catalogue-centred: stable native `Ui` controls, composites and presets are registered in a typed catalog, the session keeps a canonical document/theme separation, and preview, export and code generation all derive from that single source of truth.

## Architecture

UiDesigner is its own application repository and consumes `upp_Ui` as an external dependency:

```text
upp_uidesigner
      ↓
    upp_Ui
```

Shared packages that remain in `upp_Ui` and are consumed through the assembly:

- `Ui` — the `upp_Ui` control library;
- `Utilities/PropertyEditor` and `Utilities/PropertyEditorCore` — Inspector/property-editor infrastructure.

The internal production packages are `UiDesigner/Core`, `UiDesigner/Commands`, `UiDesigner/Catalog`, `UiDesigner/Preview`, `UiDesigner/CodeGen`, `UiDesigner/ThemeCore`, `UiDesigner/Theme`, `UiDesigner/Services`, `UiDesigner/CLI`, `UiDesigner/MCP` and the `UiDesigner/UiDesigner` application. See `docs/README.md` for package responsibilities and current design authorities.

The assembly also lists `upp_animation` and `upp_statemachine`, which are transitive dependencies of the `Ui` library. No source from `upp_Ui` is vendored into this repository.

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

The application compiles entirely from this repository's `UiDesigner` sources plus the declared external dependencies; nothing is compiled from the old `upp_Ui\Utilities\UiDesigner` package.

CLI and MCP console builds:

```bat
E:\upp-18468\umk.exe github "UiDesigner/CLI" CLANGx64 -br E:\apps\github\upp_uidesigner\build\uidesigner_cli.exe
E:\upp-18468\umk.exe github "UiDesigner/MCP" CLANGx64 -br E:\apps\github\upp_uidesigner\build\uidesigner_mcp.exe
```

The CLI and MCP link roots include the Theme adapter package because `UiDesignerSession` exposes the same Theme-aware automation semantics as the graphical application. The canonical authoring/service state remains shared rather than duplicated in the console hosts.

## Validation

The complete release-candidate validation entry point is:

```powershell
powershell -ExecutionPolicy Bypass -File RunSupervisorValidation.ps1
```

It builds and runs the architecture guard, PropertyEditor probes/tests, Designer tests, RegressionTests, FoundationTests, the focused Theme ownership/coverage/dark/builder gates, UiSplitter catalog coverage, CLI/MCP smokes, generated-package builds and the canonical UiDesigner GUI executable.

Generated-package and preset fixture proofs are also available individually:

```bat
powershell -ExecutionPolicy Bypass -File tests/FoundationTests/BuildGeneratedFixture.ps1
powershell -ExecutionPolicy Bypass -File tests/PresetExportTests/BuildPresetFixtures.ps1
```

Do not treat historical migration-era failure counts as the current acceptance baseline. Current acceptance is the result of the complete validation script against the exact current `upp_Ui/main` dependency, followed by the interactive desktop audit described in `docs/ACTIVE_WORK.md`.

## Future embedded assistant

The post-RC assistant direction is documented in `docs/AI_ASSISTANT_ARCHITECTURE.md`.

The intended architecture embeds AgentFlow directly and reuses `UiDesignerAutomationService` as the canonical application-control surface. MCP remains the adapter for external chatbot/agent hosts; the embedded assistant does not route its own internal reasoning through MCP.

## Licence

See the repository `LICENSE`.
