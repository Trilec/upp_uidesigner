# ChartRing Designer integration — UID-CHARTRING-01

## Source contract

UiChartRing is a proportional multi-value chart, separate from the already
implemented UiProgressRing. The reusable Ui implementation is unchanged.

The Designer catalog exposes:
- `segments`: one ordered ValueArray of `{value, label, color}` records;
- `explicit_total`: 0 selects automatic sum; a larger total leaves a remainder;
- `center_text`: authored text, empty clears it;
- ordinary role, visibility, enabled and layout configuration.

A segment value must be finite and non-negative. Zero-valued segments and empty
collections are valid and preserve identity/order. `color` is a typed Color or
null; null deliberately preserves automatic Theme series colour rather than
saving the current resolved colour. Unknown/malformed records and overflowing
sums are rejected by the shared parser. Catalog validation blocks invalid
ChartRing documents from successful code generation/export. This is not a new
general validation framework for every Session custom-property write.

## Editor and authority

Inspector and Data share `designer.chart-ring.segments`. It opens a resizable
modal segment editor using existing Ui controls and the PropertyEditor colour
picker. Add, remove, move, label/value and automatic/custom colour changes affect
only a disposable draft until Apply. Apply emits one normal command-backed
collection edit; Cancel/escape/window close does not mutate the document.
Unchanged numeric values retain their original precision when other row fields
are edited. The property editor detects disposal or an externally refreshed
value while its modal dialog is open before applying a result.

`UiDesignerControlSpec::data_property_id` defaults to `value`, preserving existing
single-property Data bindings. ChartRing selects `segments`. There is no second
chart payload in node.data and no second persistent command history. The Data
property group is labelled Data for these projections.

The UI-specific editor lives in UiDesigner/Editors. Services remains headless;
only GUI/test link roots opt into Editors and register it explicitly.

## Preview, Theme and generated application

The Preview registry constructs the actual UiChartRing and uses SetSegments,
SetTotal and SetCenterText. Applying malformed Preview data is transactional.

The `chart_ring` Theme adapter exposes supported track/text/series colours and
ring/font metrics. Data, total, labels and centre text never become Theme fields.
The normal ThemeDocument recipe/local override precedence is reused. The curated
Theme Studio sample layout is unchanged; this tranche adds the adapter and
Inspector override surface, not a new permanent gallery tile.

CodeGen emits ClearSegments/AddSegment and the actual total/text APIs. It does
not call inherited Ctrl::SetData for a collection. Automatic colours emit Null,
empty labels emit a real empty string, and numbers use 17-digit formatting.
Complete and ComponentOnly exports reuse the existing compiled-Theme path.
No generated runtime dependency on UiDesigner, this editor, or theme.json is added.

## Source inventory

Production:
- Core/UiDesignerChartRingData.h and Core.upp
- Catalog/UiDesignerCatalog.h and UiDesignerCatalog.cpp
- Services/UiDesignerAdvancedCatalog.h and UiDesignerAdvancedCatalog.cpp
- Editors/Editors.upp, UiDesignerChartRingEditor.h, UiDesignerChartRingEditor.cpp
- Preview/UiDesignerChartRingPreview.cpp and Preview.upp
- Theme/UiDesignerChartRingThemeAdapter.cpp, UiDesignerThemeAdapterRegistry.cpp, Theme.upp
- CodeGen/UiDesignerModelDataCodeGen.cpp
- UiDesigner/UiDesigner.upp and UiDesignerWindowClosure.cpp

Paths above are relative to UiDesigner/. Tests live in tests/ChartRingIntegrationTest.
RunSupervisorValidation.ps1 includes the generated-runtime gate.

## Windows acceptance

From E:\apps\github\upp_uidesigner, current main, U++ E:\upp-18468, github/CLANGx64:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/ChartRingIntegrationTest/BuildGeneratedFixture.ps1 -DebugBuild
powershell -NoProfile -ExecutionPolicy Bypass -File tests/ChartRingIntegrationTest/BuildGeneratedFixture.ps1
```

Require build success, suite failed=0, and complete/component runtime verifiers
printing CHARTRING_GENERATED_RUNTIME failed=0 with exit 0. Both verifiers execute
from a foreign CWD with theme.json removed; the original complete app also builds.

Then build the Designer GUI and manually exercise the segment editor from both
Inspector and Data. Check Apply/Cancel, empty chart, zero row, reorder, mixed
selection, undo/redo/reset, save/load, styling and resize. Compare the generated
application with Preview. Source review is not a substitute for this desktop gate.

After focused acceptance, the local senior continues the existing Designer-only
RC/Theme/export closure. Do not reintroduce the separate UiGraph performance or
authoring matrix. Remaining new Designer controls are DateTime, ColorMatrix,
MatrixSelector and Gallery; agent runtime/UI implementation remains a later task.
