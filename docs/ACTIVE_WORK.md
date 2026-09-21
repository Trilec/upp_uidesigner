# ACTIVE WORK

Remote main is authoritative. Fetch before work and immediately before publishing.
No feature branch: Curt requested sequential coding on main, then local senior validation.

BASE: `09f281f80036e8e9c4c6a3a246be957b410244ac` / main
TASK: **UID-CHARTRING-01 — ChartRing Designer integration**
TOUCHED: Core segment contract; Catalog/data binding; Editors; Preview/Theme adapters;
CodeGen collection output; application Data wiring; package manifests;
`tests/ChartRingIntegrationTest`; `RunSupervisorValidation.ps1`; this file and
`docs/CHARTRING_INTEGRATION.md` (complete path inventory there).
STATUS: **IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING**
PUBLISHED: commit containing this recovery entry; resolve with `git log -1 -- docs/ACTIVE_WORK.md`.
VALIDATION: source/API/diff/package review only. No Windows compilation, execution or visual acceptance in this remote session.
NEXT ACTION: local senior runs the focused ChartRing Debug gate, fixes genuine in-scope defects,
then completes Designer-only RC/Theme/export closure and the consolidated Release gate.

## Implemented source

- ChartRing is registered separately from ProgressRing with one authored ordered
  `segments` property, optional total and centre text.
- Inspector/Data share a typed segment editor: draft add/remove/reorder, automatic
  or explicit colour, Apply as one existing command; Cancel leaves data unchanged.
- Real UiChartRing Preview, independent Theme adapter, typed persistence and actual
  collection API code generation. No upp_Ui or UiGraph source changes.
- Focused integration suite plus complete/component generated-runtime verifier.
- No new permanent Theme Studio sample tile; the existing curated gallery is unchanged.

## Existing RC evidence / unfinished acceptance

PropertyEditor's prior reported Debug/Release results: 77/0; override-commit 6/0.
Foundation semantic Tab fixture/parenting was repaired at b9bbebb...; duplicate
Accordion test identifiers were repaired at 234367.... Latest prior recovery was
09f281f.... Successful final Designer acceptance after those fixes is not recorded.
Do not label them currently failing without reproducing, or passed without evidence.

The local senior has implementation authority for bounded Designer/ChartRing and
Theme/export defects, with regression tests. Ordinary failures should be diagnosed,
fixed and retested rather than bounced to Gary one assertion at a time.

## Consolidated acceptance

1. Pull current main in Designer and upp_Ui; record exact revisions/toolchain.
2. Run `tests/ChartRingIntegrationTest/BuildGeneratedFixture.ps1 -DebugBuild`.
3. Build/run affected Designer Tests, RegressionTests, FoundationTests,
   ExportedThemeContractTest and CurrentUiIntegrationTest in Debug.
4. Run `RunSupervisorValidation.ps1` fully (Release; now includes ChartRing).
5. Exercise the real Designer: segment editor, property/data and Theme overrides,
   undo/redo, save/load, export, generated application visual parity, foreign CWD,
   ComponentOnly and user-code preservation.
6. Record actual outputs, screenshots/log locations, tested SHAs and remaining work.

The graph project's features and 10k/performance validation are NOT Designer gates.
A dependency compile blocker matters only because it prevents the Designer build.

## Boundaries retained

- Canonical Document and separate Theme; commands own durable edits and undo.
- Tab/Page/content and Accordion/Section/content semantics remain unchanged.
- Theme preset/mode and explicit style recipes compile into runtime output.
- Active instance override wins; disabled/reset inherits; studio_preview stays sample-only.
- Global palette/role-slot/metric metadata is not a new production style mapping.
- Remaining control integrations: UiDateTime, UiColorMatrix, UiMatrixSelector, UiGallery.
- Embedded AgentFlow/assistant implementation is not part of this checkpoint.
