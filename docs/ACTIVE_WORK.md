# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

Historical Theme acceptance and earlier Designer recovery checkpoints remain in Git history. This file is the current recovery checkpoint, not an accumulating transcript.

## DESIGNER CLOSURE R7 — 2026-08-28

BASE:

- Designer closure continuation base: `cc5405da21c21215e46a78f2245700fc6793c8ed`.
- Current published `upp_uidesigner/main` source head before this bookkeeping commit: `689fe825ead08da4d9bb66c598f65921e23032ea`.
- Current `upp_Ui/main`: `d51d46a61ed9ce8081b03babf3192c79efffe35f`.
- Required reusable PropertyEditor working-range checkpoint: `44a7b1af0d26730a1f1744b80d4bea319d561bd5`; compare to current `upp_Ui/main` confirms it remains the merge-base/ancestor. Intervening Ui commits touch only UiGraph/UiNodeGraph route-edit work and do not touch PropertyEditor, Theme, GroupPanel, UiDoc, RangeSlider, or Designer dependencies.

TASK:

Finish the remaining Designer correctness/Theme/preset closure and leave only Windows/U++ platform validation.

STATUS: **IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING.**

PUBLISHED SOURCE CHECKPOINTS:

- `f680416b99bee979af1e363f888f42340f7f9fe2` — `Complete Designer closure contracts`.
- `d9c94b2cf37a4b5fe71492cf0be93d431858f123` — `Default ToolButton to icon-only text`.
- `689fe825ead08da4d9bb66c598f65921e23032ea` — `Expand preset generated-package coverage`.

TOUCHED / ACTIVE CLOSURE PATHS:

- `UiDesigner/Preview/UiDesignerAdvancedPreview.cpp`
- `UiDesigner/Services/UiDesignerAdvancedCatalog.h`
- `UiDesigner/Services/UiDesignerAdvancedCatalog.cpp`
- `UiDesigner/Services/UiDesignerPresetPlacement.cpp`
- `UiDesigner/Services/UiDesignerPresets.cpp`
- `UiDesigner/Services/UiDesignerSession.h`
- `UiDesigner/Services/Services.upp`
- `UiDesigner/Theme/Theme.upp`
- `UiDesigner/Theme/UiDesignerGroupPanelThemeAdapterV3.cpp`
- `UiDesigner/Theme/UiDesignerThemeAdapter.h`
- `UiDesigner/Theme/UiDesignerThemeAdapterRegistry.cpp`
- `UiDesigner/Theme/UiDesignerThemeBuilderV2.h`
- `UiDesigner/Theme/UiDesignerThemeBuilderV2.cpp`
- `UiDesigner/Theme/UiDesignerThemeBuilderV2Contract.cpp`
- `UiDesigner/Theme/UiDesignerThemeGallery.h`
- `UiDesigner/UiDesigner/UiDesignerCatalogList.cpp`
- `UiDesigner/UiDesigner/UiDesignerHierarchyRename.cpp`
- `UiDesigner/UiDesigner/UiDesignerInteractionOverlayV2.h`
- `UiDesigner/UiDesigner/UiDesignerPresetDropV2.cpp`
- `UiDesigner/UiDesigner/UiDesignerVersion.h`
- `UiDesigner/UiDesigner/UiDesignerWindow.h`
- `UiDesigner/UiDesigner/UiDesignerWindowClosure.h`
- `UiDesigner/UiDesigner/UiDesignerWindowClosure.cpp`
- `UiDesigner/UiDesigner/UiDesigner.upp`
- `tests/DesignerClosureCatalogTest/main.cpp`
- `tests/PreviewLayoutRegressionTest/main.cpp`
- `tests/ThemeBuilderContractTest/main.cpp`
- `tests/PresetExportTests/main.cpp`
- `tests/PresetExportTests/BuildPresetFixtures.ps1`
- `tests/PresetExportTests/PresetExportTests.upp`
- `docs/ACTIVE_WORK.md`

IMPLEMENTATION:

- Common fixed/min/max sizing keeps its legal authored range through 10,000 while the PropertyEditor slider uses a practical 0–500 working range. Maximum width/height zero remains the canonical `No limit` sentinel; direct entry above 500 remains legal.
- `UiRangeSlider` is exposed as the real reusable Ui control with range editor metadata, genuine Preview runtime and normal generated `ValueArray`/`SetData` contract.
- `UiNodeGraph` is exposed as the real reusable control with a genuine three-node Preview model; Designer does not invent a second graph data model.
- `UiToolButton` exposes normal Text metadata and now defaults to an empty string, so newly dropped ToolButtons are icon-only while text remains editable and clearable.
- `UiGroupPanel` exposes subtitle and canonical icon-picker metadata. Preview uses real `SetSubTitle`, `SetIcon` and `ClearIcon`; generated code preserves subtitle through the existing generic codegen and icon through the delegating GroupPanel Theme adapter V3.
- `UiDoc` uses one canonical authored `value` property for Scalar Data. The Data pane projection, Preview `SetData`, undoable Session property commits and generated `.SetData(...)` all share that value; no duplicate `node.data` state is introduced.
- Hierarchy double-click tears down the manual-drag/capture gesture before delegating to `UiTree::LeftDouble`, restoring rename-on-double-click.
- Preset activation and drag identity are separated correctly. Canvas preset drops use location-aware `InsertPresetAt`; existing direct/hierarchy insertion remains intact.
- Demo preset is registered and builds a 2x2 Grid containing four distinct GroupPanels demonstrating RangeSlider, UiDoc, UiNodeGraph and ToolButton.
- Preview regression coverage verifies four distinct non-overlapping GroupPanel grid cells, nested selection-stack order, Button -> GroupPanel reparenting, GroupPanel -> free Grid-cell reparenting, and positioned preset insertion.
- Catalog painting resolves paper/alternate/accent/ink/disabled/divider colours from the active Ui theme instead of fixed light-only system colours; the scope label no longer retains a constructor-time light style copy.
- Theme Studio V2 exposes independent universal `Standard / Subtle / Accent / Alert` Panel Role and Control Role axes. Runtime container adapters already resolve universal `UiRole`, so Accent and Alert remain real roles.
- Theme Studio preserves independent `panel|...` and `control|...` style-target namespaces.
- Theme Studio swaps DATA and CHOICES placement and constrains the Save split-button inside the Buttons group.
- Panel Role / Control Role labels, tooltips and status messages explicitly state that the two role axes are independent.
- Designer branding uses `ICON_BRAND_NEWLOGO_V5_48()` for the native window, header media and version badge with compact inset sizing. Visible version is `v1.0.1-RC2`.
- `PresetExportTests` now exports all three creation presets plus every preset in the live application catalog. The PowerShell harness discovers every generated package directory and compiles each package, so future registered presets automatically enter the generated-package acceptance path.

SOURCE REVIEW / PUBLICATION:

- Complete closure diff and package membership reviewed before each fast-forward publish.
- `upp_uidesigner/main` was refreshed immediately before each source publication and the exact remote SHA was re-fetched afterwards.
- Current Ui dependency advance was compared against `44a7b1af...`; only unrelated UiGraph files changed.
- No `upp_Ui` application workaround was added; the reusable working-range behavior remains in PropertyEditor.
- Supervisor environment cannot execute Windows/U++ binaries or local `git diff --check`; these remain explicit validator gates rather than being reported as PASS.

WINDOWS / U++ VALIDATION GATES:

1. Confirm `689fe825ead08da4d9bb66c598f65921e23032ea` is an ancestor of the exact tested `upp_uidesigner/main` HEAD and `44a7b1af0d26730a1f1744b80d4bea319d561bd5` is an ancestor of the exact tested `upp_Ui/main` HEAD. If either main has advanced, inspect intervening changes before testing.
2. Run `git diff --check` in both repositories.
3. `upp_Ui/Utilities/PropertyEditorWorkingRangeTest`, Debug and Release: require `PROPERTY_EDITOR_WORKING_RANGE_SUMMARY checks=10 failed=0`.
4. `upp_uidesigner/tests/DesignerClosureCatalogTest`, Debug and Release: require `DESIGNER_CLOSURE_CATALOG_SUMMARY checks=35 failed=0`.
5. `upp_uidesigner/tests/ThemeBuilderContractTest`, Debug and Release: require `THEME_BUILDER_CONTRACT_SUMMARY checks=57 failed=0`.
6. `upp_uidesigner/tests/PreviewLayoutRegressionTest`, Debug and Release: require `PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=39 failed=0`.
7. Run `tests/PresetExportTests/BuildPresetFixtures.ps1` with the normal `github` assembly / `CLANGx64` toolchain. Require the export summary to report `failed=0` and require every generated creation/catalog preset package discovered by the script to compile and produce its executable. Generated fixture directories must be removed on success and retained on failure.
8. Run `ThemeAdapterCoverageTest` Debug and Release; require `failed=0` and report the observed check count rather than enforcing a stale historical count.
9. Re-run the retained Theme suites: ThemeDocument (`31/0` if unchanged), Label adapter (`242/0`), List/Edit adapter (`114/0`), Dropdown/Accordion adapter (`103/0`), plus the existing principal Theme/Preview suites; stop on the first substantive failure.
10. Build `UiDesigner` Debug and Release with no compiler or linker errors.
11. Manual Debug smoke:
    - activate and drag Holy Grail, Magazine and Demo presets; verify no `Unknown preset` and verify canvas placement follows the hovered destination;
    - verify nested Preview selection can reach GroupPanel/layout ancestors and existing nodes can be dragged Button -> GroupPanel and GroupPanel -> a free Grid cell;
    - verify hierarchy double-click starts name editing;
    - verify four expanded GroupPanels in a 2x2 Grid remain distinct/selectable without overlap or last-cell clobbering;
    - verify ToolButton starts icon-only, Text is editable, and clearing Text returns to icon-only;
    - verify GroupPanel icon chooser, set and clear behavior;
    - verify fixed/min/max sliders are usable over 0–500, typed values above 500 remain valid, and max zero means No limit;
    - verify UiDoc Data pane exposes/commits its scalar value and Preview updates;
    - verify RangeSlider and NodeGraph appear as genuine Designer controls;
    - switch Dark Theme and verify the left catalog/filter/scope surfaces respect it;
    - in Theme Studio verify Panel Role and Control Role each offer Standard/Subtle/Accent/Alert and changing one does not change the other;
    - verify DATA/CHOICES placement, Save button containment and panel/control recipe isolation;
    - verify the app/header/version badge use the main brand icon and the visible version is `v1.0.1-RC2`.
12. No substantive validator edits. Minor mechanical build/test corrections only; any architectural/runtime failure returns to the supervisor.

NEXT ACTION:

Run the Windows/U++ validation matrix above. If every gate passes, publish a small `ACTIVE_WORK.md` acceptance update changing status to PASS with exact tested heads/results, then perform branch cleanup only for refs proven fully contained in `main`.

HYGIENE NOTE:

A remote branch inventory on 2026-08-28 found `main` plus legacy work branches and several temporary/accidental refs. No branch has been deleted without containment proof. The current connector does not expose branch-ref deletion. In particular `__ignore` and `__ignore2` were accidentally created during this supervisor session at ancestor `cc5405da...` and contain no intended unique work; they are safe cleanup candidates once branch deletion is performed with an appropriate Git client/API. Classify the remaining legacy work branches against current `main` before deletion.
