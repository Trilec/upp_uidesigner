# ACTIVE WORK
Remote GitHub `main` is authoritative. Refresh both repositories before acting; never force-update `main`.

## SOURCE OF TRUTH
- Designer: `Trilec/upp_uidesigner`, branch `main`.
- Reusable controls: `Trilec/upp_Ui`, branch `main`.
- Windows checkouts: `E:\apps\github\upp_uidesigner`, `E:\apps\github\upp_Ui`.
- U++: `E:\upp-18468\umk.exe`, assembly `github`, `CLANGx64`.
- Canonical Designer executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.

## ACTIVE TASK
Finish the UiDesigner release candidate against the actual current `upp_Ui/main`. Do not start the embedded AI assistant implementation until this gate and Curt's final visual acceptance pass.

## CURRENT CHECKPOINTS
- Current reusable source inspected: `8b8f6c3c8c776814c0d9ceda99456c3931840505`.
- Gary's earlier UiGraph R10A Windows PASS was at `b701107fe02cb9bbd0181519d548c18d2dfcc1e1`; current Ui main has advanced beyond it.
- Designer release-cleanup source checkpoint before this recovery update: `5d64e991b7a26771716a7a80866185e14a978427`.
- Theme-ownership implementation ancestor: `389f5abc058ef1c6c1c7e29d59a9141254d7dc75`.

## COMPLETED SOURCE WORK
- Theme changes no longer own structural choices such as TitleCard divider visibility, icon/media placement, requested Tab visual, ScrollBar arrow layout or List separator/badge modes.
- Designer brand explicitly disables its own TitleCard dividers; default TitleCard title divider remains ON and card divider OFF.
- Compact shell/page rhythm restored to ~4 px; catalog rows keep their local gutter.
- Project icon/header and Theme Studio sample rebalancing are retained.
- CLI/MCP final link roots now include `UiDesigner/Theme`; `UiDesigner/Services` remains cycle-free/headless.
- `RunSupervisorValidation.ps1` now also gates current `upp_Ui` PropertyEditor override-commit and UiTheme structure contracts, then Regression/Foundation, Designer Theme ownership/coverage/Dark/Builder, UiSplitter, CLI/MCP smokes, generated-package proof and canonical GUI build.
- Root/build documentation no longer treats old migration-era failure counts or CLI/MCP link failure as current acceptance state.
- Current Designer source audit found no direct use of retired UiGraph shape enum names or old ProgressRing cap API.

## CURRENT UPP_UI INTEGRATION RISK
Since the previous Designer dependency checkpoint, `upp_Ui` has advanced through UiGraph hierarchy/render/shape work, UiGeometry/UiShapePath/UiShapes, UiChartRing, PropertyEditor fixes and other control updates.
Source inspection found no obvious direct R10 shape-API break in Designer, but only a real Windows build/test against current main closes this integration risk.

## AI ASSISTANT — POST-RC DIRECTION
Authority: `docs/AI_ASSISTANT_ARCHITECTURE.md`.
- Embed AgentFlow directly when the assistant is part of UiDesigner.
- Reuse `UiDesignerAutomationService` as the canonical application-control/tool surface.
- Embedded agent calls that service directly; it does not route itself through MCP.
- MCP remains the transport for an external ChatGPT/Codex/Claude-style host operating UiDesigner.
- External applications/services later use AgentFlow capability broker / Agent Bridge.
- Recommended first UI: Assistant icon -> collapsible/resizable bottom drawer, optional modeless pop-out later.
- Conversation edits must follow revision check -> preview/plan -> typed command -> undo/history -> canonical projection refresh.
- No second document model, arbitrary JSON mutation, hidden provider session authority or private chain-of-thought UI.
- DeepSeek Harness is an architectural reference for provider/tool/session capability seams, not a UiDesigner dependency.

## RELEASE ACCEPTANCE
Automated:
1. Refresh both mains; record exact SHAs.
2. Run `RunSupervisorValidation.ps1`; every deterministic executable must exit 0.
3. If a failure is mechanical, fix the smallest dependency/include/signature/package issue. Architecture/state/theme issues return to supervisor.

Interactive:
- icon/header/no Designer divider;
- 4 px shell rhythm and catalog gutter;
- compact/aligned Theme Studio;
- Light -> Dark -> Light preserves authored structure;
- toolbox/hierarchy/canvas add/move/reparent;
- Inspector preview/commit/reset and Behavior Inspector;
- save/load/export/user-code preservation;
- normal workspace/theme switching without crash/assert.

## STATUS
SOURCE/RELEASE PREP COMPLETE — CURRENT UPP_UI WINDOWS VALIDATION PENDING.

## AFTER PASS
Record exact validated heads/results, clean stale release-status prose, accept/tag the RC, then begin Assistant A1: direct embedded tool gateway over `UiDesignerAutomationService`.
