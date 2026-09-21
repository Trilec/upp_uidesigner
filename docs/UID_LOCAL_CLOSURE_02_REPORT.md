# UID-LOCAL-CLOSURE-02 checkpoint

Classification: **PARTIAL — automated closure passed; full visible acceptance pending.**

Designer base: `6034445839be439ce763e13fb0a52a0632224021`.
Reusable Ui tested, unchanged: `57e8d38167cde7cee2bc62b0093979af86ca91ca`.
Both remotes were refreshed before publication; neither had advanced.
Published repair: the main commit containing this report (resolve with git log).

## Toolchain

Inspected github.var: repository, examples, tests, upp_Ui, upp_animation,
upp_statemachine and E:/upp-18468/uppsrc; OUTPUT is the repository build directory.
E:/upp-18468/umk.exe, assembly github, CLANGx64, bundled Clang 21.1.1.
Executables remain directly under build, including canonical UiDesigner.exe.
The canonical application is Release (-br); focused Debug uses -b.

## Causes and repairs

- Tests printed 1292/0 but crashed during shutdown (C0000005). LLDB identified
  heap-leak reporting during static destruction. Two unowned heap buttons are
  now stack fixtures. GUI test aggregates use GUI lifecycle entry points.
- Legacy sizing migration removed an entry then used a stale second map index.
- Preset insertion incorrectly rejected the non-catalog Window root before
  catalog occupancy validation could run.
- Hierarchy W/H hit rectangles omitted themed Tree padding; accessory clicks
  could arm a drag. Correct geometry and two new no-drag assertions cover this.
- Button/ToolButton/SplitButton, GroupPanel and Tab exposed retired parallel
  structural Theme fields. Removed their schema entries. Tab style application
  and generated resolver seeds now respect ordinary authored visual family.
  Five-family runtime/code-generation checks were added.
- Regression fixtures assumed outdated sizing, icon emission and occupied-root
  behavior. They now explicitly author icons and test valid empty-root drops.
- Coverage ignored automatic Cardinal4 projection. It now verifies both the
  custom editor contract and retained choices/expansion metadata.
- Gallery position and scalar group-name assertions predated published layout
  and ChartRing changes. Updated to Choices column 2 and Data respectively.
- Windows MCP text mode emitted CR CR LF. Byte-preserving stdin/stdout fixes
  Content-Length framing; both framing modes pass their existing smoke.
- ChartRing modal used a light default background with dark-theme text.
  Painting the existing resolved panel theme fixes visible readability.
- Supervisor runner now explicitly waits for test exit codes and retains stdout
  and stderr, rather than relying on stale shell status from GUI executables.

## Automated results

All listed successful processes returned exit 0.

| Gate | Debug checks/failures | Consolidated Release checks/failures |
|---|---:|---:|
| ChartRing integration | 82/0 | 82/0 |
| Tests | 1292/0 | 1292/0 |
| RegressionTests | 79/0 | 79/0 |
| FoundationTests | 73/0 | 73/0 |
| ExportedThemeContractTest | 24/0 | 24/0 |
| CurrentUiIntegrationTest | 64/0 | 64/0 |
| ThemeStructureOwnershipTest | 89/0 | 89/0 |
| ThemeAdapterCoverageTest | 9177/0 | 9177/0 |
| ThemeBuilderContractTest | 57/0 | 57/0 |
| DesignerClosureCatalogTest | 37/0 | 37/0 |
| PropertyEditorTests | not rerun | 77/0 |
| PropertyEditorOverrideCommitTest | not rerun | 6/0 |
| UiThemeStructureContractTest | not rerun | 1092/0 |
| ThemeDarkIntegrationTest | not rerun | 18/0 |
| UiSplitterCatalogTest | not rerun | 24/0 |

PropertyEditorCoreProbe passed. CLI list/schema and both MCP framing smokes passed.
Foundation generated application compiled and stayed alive from a foreign CWD.
Preset export: creation=3 catalog=9 total=12 failed=0; all 12 executables built.
ChartRing complete app built in Debug and Release. Complete and ComponentOnly
runtime verifiers each reported CHARTRING_GENERATED_RUNTIME failed=0 in both
configurations, from foreign CWD with theme.json absent.

The complete supervisor script returned 0. Its application build preceded the
last modal paint edit; the canonical application was separately rebuilt afterward.
ChartRing Release in that run and the final Debug rerun include the modal edit.

## Visible desktop checks and remaining work

Observed: saved ChartRing fixture loads; real ring previews render; Inspector and
Data both open the shared segment editor. After the paint repair its dark body,
labels, values and controls are readable. Adding a fourth draft row leaves the
document at three until Apply. Apply updates Preview and the count to four.
With Preview focused, one Ctrl+Z restores three. Escape closes the editor.
The loaded multiline label remains represented in its list row; comprehensive
untouched-value preservation has automated coverage but was not manually completed.

Remaining observations: in the loaded Dark fixture, the hierarchy remains light
and the Preview host is white with low-contrast ring center text. These are
observations, not a completed root-cause diagnosis. Ctrl+Z while the segment
property button retained focus did not perform document Undo; after clicking
Preview it did. Do not describe global shortcut routing as fully accepted.

Not completed: catalog add, remove/reorder, typed/wheel values, custom colour
picker, Cancel/window-close after draft changes, no-op Apply history, empty/zero
manual cases, precision preservation, reset/redo/multi-selection, resize and full
save/load round-trip; ProgressRing manual inspection; full Light/Dark/preset/role
and override isolation; mixed ordinary/Tab/Accordion/ring fixture; visible
generated-vs-Preview comparison; manual ComponentOnly comparison and sentinel
re-export preservation. Automated runtime proof is not visual parity acceptance.

## Evidence

Paths below are relative to E:/apps/github/upp_uidesigner. Build evidence remains
local/ignored, not committed. Desktop screenshots are in this task's tool history.

- build/UID-LOCAL-CLOSURE-02-supervisor-complete.log (successful full runner)
- build/UID-LOCAL-CLOSURE-02-supervisor*.log (earlier failures preserved)
- build/supervisor-* (individual test stdout/stderr)
- build/UID-CHARTRING-01-Release-20260921-231551
- build/UID-CHARTRING-01-Debug-20260921-231657
- build/*-final-debug.*.log (focused reruns)
- build/UID-LOCAL-CLOSURE-02-app-final.log (canonical app rebuild)
- build/closure-visual-chartring/ChartRingFixture/project.uidesign.json

## Changed paths

- RunSupervisorValidation.ps1
- UiDesigner/Core/UiDesignerSerialization.cpp
- UiDesigner/Editors/UiDesignerChartRingEditor.cpp
- UiDesigner/MCP/main.cpp
- UiDesigner/Services/UiDesignerSession.cpp
- UiDesigner/Theme/UiDesignerButtonThemeAdapterV2.cpp
- UiDesigner/Theme/UiDesignerGroupPanelThemeAdapterV3.cpp
- UiDesigner/Theme/UiDesignerTabThemeRuntimeAdapter.cpp
- UiDesigner/UiDesigner/UiDesignerHierarchyView.cpp
- tests/Tests/{main.cpp,Tests.upp}
- tests/RegressionTests/{main.cpp,RegressionTests.upp}
- tests/ThemeStructureOwnershipTest/{main.cpp,ThemeStructureOwnershipTest.upp}
- tests/ThemeAdapterCoverageTest/main.cpp
- tests/ThemeBuilderContractTest/main.cpp
- tests/DesignerClosureCatalogTest/main.cpp
- docs/ACTIVE_WORK.md
- docs/UID_LOCAL_CLOSURE_02_REPORT.md

No reusable dependency edits, generated sources or logs are included.
