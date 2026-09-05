# ACTIVE WORK
Remote main is authoritative. Fetch both repositories before work/publish; never force-push.

BASE:
- UiDesigner: a6376549870e750e9f384fa137dcbb017deb67e0
- upp_Ui: 408c82c418fb87fb56cce4cac6752dd69add9dcf

TASK: Current public Ui authoring coverage and release closure audit.
STATUS: PARTIAL — RangeSliderEdit and existing List/Tree generation closed in source; theme export remains a blocker.
PUBLISHED: This checkpoint; resolve its SHA with git log -1 -- docs/ACTIVE_WORK.md.
VALIDATION: Source/blob identity and static checks only. Windows/U++ validation pending.

TOUCHED:
- Catalog theme-schema decorator; Services advanced catalog/value projection; Preview adapters.
- CodeGen List/Tree model emission, nested typed containers and RangeSliderEdit setup.
- Theme registry supplies schema decoration without new Services-to-Theme symbols.
- CurrentUiIntegrationTest and shared ModelDataFixture.h; FoundationTests generated fixture.
- CodeGen and test package membership; docs/BUILD.md; this recovery file.

COMPLETED SOURCE:
- UiProgressRing is an actual Ctrl with scalar value, separate total (0 indeterminate),
  automatic/custom center text and document-owned animation configuration.
- ProgressRing appearance overrides use the current cap_roundness API and resolve
  the reusable control's actual theme. No Designer-local ring renderer.
- GroupPanel generates SetIcon or ClearIcon consistently with Preview.
- codegen=false and unresolved production contracts reject C++ export.
- Explicit adapter_backed_runtime preserves RangeSlider/NodeGraph adapter support;
  Placeholder by itself is not evidence of a missing runtime.
- Supervisor gate now runs the existing closure/catalog regression and new integration test.
- Generated Foundation fixture exercises ProgressRing and GroupPanel set/clear APIs.

- UiRangeSliderEdit uses one two-element value, separate domain/step/direction and
  field width/gap/inset/precision. Inspector and Data share the configured editor.
- RangeSlider/NodeGraph regression now resolves and instantiates real registry adapters.
- List/Tree authored items and nested children now generate reusable model setup.
- Nested ValueArray/ValueMap generation preserves Color values without JSON erasure.
- Foundation rectangle expectations match the current 80x25 button sizing profile.

REMAINING PUBLIC CONTROLS:
UiDateTime, UiColorMatrix, UiMatrixSelector, UiChartRing, UiGallery.
No catalog-only stubs have been added for these. Each requires its complete value/data,
configuration, real Preview, appearance, generation and test contract.

OTHER AUDIT FINDINGS TO CLOSE:
- Export writes theme.json but generated startup does not apply the ThemeDocument.
  The global Designer bridge currently applies only preset/mode; style recipes are
  applied to Theme Studio samples separately. Trace/fix the complete authored path.
- Time needs explicit canonical codec round-trip tests before UiDateTime is added.
- Older Catalog/CodeGen/Session Theme-symbol seams remain; no .upp cycle added.
- Existing slider/range and splitter configuration coverage needs further review.
- Full 49-control coverage matrix and release harness audit are still in progress.

NEXT ACTION: Close exported-theme/application fidelity first, then typed-value controls
and dedicated structured model controls. Do not begin AI UI before RC closure.

WINDOWS: E:\apps\github\upp_uidesigner and E:\apps\github\upp_Ui, main.
Run RunSupervisorValidation.ps1 with current github.var nests and CLANGx64.
Record both exact heads, individual build/test results and visible-desktop checks.
No Windows PASS is claimed here. No AI drawer/provider/AgentFlow runtime work before RC closure.
