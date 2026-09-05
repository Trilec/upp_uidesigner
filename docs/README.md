# UiDesigner Greenfield System

UiDesigner is now a standalone application repository under `upp_uidesigner`.
It consumes `upp_Ui` as an external dependency.

## Packages

- `UiDesigner/Core` — canonical persistent document, legacy import, typed changes and transient preview overrides
- `UiDesigner/Commands` — atomic writes, rollback, undo/redo and saved checkpoints
- `UiDesigner/Catalog` — every stable native Ui control, composites, presets and stock U++ controls
- `UiDesigner/Preview` — stable runtime instances, localized projection, subtree rebuilds and overlays
- `UiDesigner/CodeGen` — deterministic U++ C++, package and JSON generation
- `UiDesigner/ThemeCore` — separate headless Theme Studio document and independent history
- `UiDesigner/Theme` — the authored Theme Studio gallery plus automatic complete native Ui inventory
- `UiDesigner/Services` — session, PropertyEditor integration, save/load/export and headless automation
- `UiDesigner/CLI` — headless validation, schema, migration, editing and generation commands
- `UiDesigner/MCP` — stdio MCP/JSON-RPC host over the same application services
- `UiDesigner/UiDesigner` — the authored three-pill/two-pill graphical application shell
- `tests/Tests` — architecture and behavior checks
- `tests/FoundationTests` — deterministic Spacer, drop, behavior and export regression checks
- `tests/RegressionTests` — regression validation package
- `tests/PresetExportTests` — blank, three-pane and dialog generated package fixture tests

The generic `PropertyEditorCore` and Ui-backed `PropertyEditor` remain sibling utilities
in `upp_Ui`, so they are reusable outside UiDesigner.

## Current design / recovery authorities

- `ACTIVE_WORK.md` — compact current recovery checkpoint and release gate.
- `IMPLEMENTATION_STATUS.md` — broader implemented/pending system status.
- `AI_ASSISTANT_ARCHITECTURE.md` — post-RC embedded AgentFlow assistant direction. The existing `UiDesignerAutomationService` remains the canonical application-control surface; embedded intelligence calls it directly, while MCP remains an external-host transport adapter.
