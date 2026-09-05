# ACTIVE WORK
Remote main is authoritative. Fetch both repositories before work/publish; never force-push.

BASE:
- UiDesigner: e872e43c1704901fc3499dcaaef8213234d995b2
- upp_Ui: 408c82c418fb87fb56cce4cac6752dd69add9dcf

TASK: Current public Ui authoring coverage and release closure audit.
STATUS: PARTIAL — first coherent source tranche; RC remains blocked by coverage gaps.
PUBLISHED: This checkpoint; resolve its SHA with git log -1 -- docs/ACTIVE_WORK.md.
VALIDATION: Source/blob identity and static checks only. Windows/U++ validation pending.

TOUCHED:
- Catalog runtime contract; Services advanced catalog; Preview advanced adapters.
- CodeGen production eligibility and GroupPanel icon parity.
- Theme ProgressRing adapter and package/registry membership.
- CurrentUiIntegrationTest; DesignerClosureCatalogTest; FoundationTests generated fixture.
- RunSupervisorValidation.ps1; docs/BUILD.md; this recovery file.

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

REMAINING PUBLIC CONTROLS:
UiRangeSliderEdit, UiDateTime, UiColorMatrix, UiMatrixSelector, UiChartRing, UiGallery.
No catalog-only stubs have been added for these. Each requires its complete value/data,
configuration, real Preview, appearance, generation and test contract.

OTHER AUDIT FINDINGS TO CLOSE:
- List/Tree authored data reaches Preview but is absent from generated setup.
- Export writes theme.json but generated startup does not apply the ThemeDocument.
- Existing slider/range and splitter configuration coverage needs further review.
- Full 49-control coverage matrix and release harness audit are still in progress.

NEXT ACTION: Publish this tranche, verify remote commit/main containment, then implement
remaining controls in coherent groups. Do not wait for Windows to continue source work.

WINDOWS: E:\apps\github\upp_uidesigner and E:\apps\github\upp_Ui, main.
Run RunSupervisorValidation.ps1 with current github.var nests and CLANGx64.
Record both exact heads, individual build/test results and visible-desktop checks.
No Windows PASS is claimed here. No AI drawer/provider/AgentFlow runtime work before RC closure.
