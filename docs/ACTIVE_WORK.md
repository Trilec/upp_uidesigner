# ACTIVE WORK

Remote main is authoritative. Fetch before work and immediately before publishing.
No feature branch: Curt requested sequential coding on main, then local senior validation.

## Local closure — UID-LOCAL-CLOSURE-02

BASE: Designer `6034445839be439ce763e13fb0a52a0632224021`; reusable Ui
`57e8d38167cde7cee2bc62b0093979af86ca91ca` (held fixed during validation).
TASK: **UID-LOCAL-CLOSURE-02 — ChartRing and existing Designer release acceptance**
TOUCHED: Tests/RegressionTests GUI harnesses and fixtures; Core sizing migration;
Session preset insertion; Hierarchy column geometry; Theme structural ownership;
MCP Windows framing; ChartRing dialog background; supervisor process evidence.
STATUS: Automated Debug and consolidated Release gates PASS after bounded repairs.
Manual acceptance PARTIAL; this is not full release acceptance.
PUBLISHED: resolve the commit containing this entry with `git log -1 -- docs/ACTIVE_WORK.md`.
VALIDATION: github.var resolves the local repositories and build output;
E:/upp-18468/umk.exe, CLANGx64, bundled Clang 21.1.1. ChartRing Debug 82/0,
complete application compiled, complete/component runtime verifiers failed=0
from foreign CWD without theme.json. Tests 1292/0 exit 0; RegressionTests 79/0
exit 0; Foundation 73/0, ExportedTheme 24/0, CurrentUiIntegration 64/0, all exit 0.
Additional Debug: ownership 89/0, coverage 9177/0, builder 57/0, closure catalog 37/0.
ChartRing final Debug and Release each 82/0 with both runtime verifiers exit 0.
Consolidated Release runner completed exit 0, including all 12 preset packages,
CLI/MCP and generated application process smoke. Canonical UiDesigner.exe rebuilt.
NEXT ACTION: investigate remaining dark hierarchy/Preview surface observations;
finish visible ChartRing/ProgressRing, Theme and generated-application acceptance.
See UID_LOCAL_CLOSURE_02_REPORT.md for exact completed and pending boundaries.

Causes repaired: two leaked test buttons triggered U++ heap-leak reporting and
a shutdown access violation; GUI-linked aggregates lacked GUI lifecycle entry
points; preset insertion rejected the non-catalog Window root; legacy alias
removal reused an index invalidated by the first removal; hierarchy header/hit
rectangles omitted the Tree's inner padding. Regression fixtures now explicitly
author tested icons, preserve actual initial sizing, and drop into an empty root.
The supervisor runner now explicitly waits and checks each test's exit code,
retaining stdout/stderr under build/supervisor-<timestamp>.
Retired Button/GroupPanel/Tab structural Theme aliases are no longer exposed;
Tab preview/generated styles use the authored visual family (five-family coverage).
Coverage now verifies Cardinal4 projection explicitly. Gallery column and Data
heading assertions were stale relative to published 434cd73 and 6034445 changes.
MCP Windows text mode doubled CR in framed headers; binary stdin/stdout repairs it.
ChartRing dialog now paints the resolved panel surface instead of a light default.

Evidence: build/UID-CHARTRING-01-Debug-20260921-223536;
build/UID-LOCAL-CLOSURE-02-*; build/*-closure.*.log;
build/Tests-owned-fixtures.*.log; build/Regression-contracts.*.log;
build/UID-LOCAL-CLOSURE-02-supervisor-complete.log;
build/UID-CHARTRING-01-Release-20260921-231551;
build/UID-CHARTRING-01-Debug-20260921-231657.

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
