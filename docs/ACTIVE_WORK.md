# ACTIVE WORK

Remote main is authoritative. Sequential coding on main; no feature branch.
Fetch before work and immediately before publishing. Keep published and locally
reported validation separate. The graph project's own gates are not Designer gates.

## UID-ASSISTANT-FIRSTUSE-02 and TitleCard/Grid repair

Follow-up: reported horizontal Box collapse reproduced with Box-first Grid child
order and repaired by synchronizing surviving managed-item indices. Assistant
now has a validated TitleCard/body/bottom-actions example, canonical explicit
Grid placement, and local human text Apply. See `DIALOG_LAYOUT_FOLLOWUP.md` for
diagnosis and final validation/publication evidence. Earlier acceptance below
covered the simpler prompt and did not establish this reported variant.

- BASE: Designer `bb50fb84fd295844a29246651ea3b5d9b708069f`; shared Ui
  `9f1cf55481db2b5039a9fe3bd66daee91acce726`. Newer main work preserved.
- TASK: repair TitleCard role/line lengths and Grid Fit measurement; reproduce
  the exact first-use dialog request and repair bounded assistant discovery.
- TOUCHED: shared Ui Grid/TitleCard, Designer sizing/catalog/preview/export/theme,
  AppChat, assistant guidance/host/drawer, tests and documentation.
- STATUS: implementation and required offline gates complete; live evidence and
  publication details in `ASSISTANT_FIRSTUSE_02_REPORT.md`.
- PUBLISHED: shared Ui `ed635bc1dcbbd0d23524d1184843995c239c0963`; tested Designer
  implementation `4623ad0e59a95fa8c29a1937390c1b2307520fd9`. Both pushed,
  fetched and confirmed contained in origin/main. This follow-up is documentation only.
- VALIDATION: required Debug/Release Embedded runs, deterministic boundary and
  layout regressions, separate exact-prompt live provider and visible Apply/Undo.
- NEXT: Curt's visual judgement in the canonical application. Templates are visual;
  OK/Cancel behavior wiring remains an explicit separate design action.

## UID-ASSISTANT-01 — embedded assistant checkpoint A

- BASE: Designer `ec6c541d2040cbd1b390339aec1e661844f89980`; reusable Ui
  `297beabdea87e3cc2c32282968e262ef68392abb`.
- TASK: native AppChat drawer, configurable DeepSeek/OpenRouter transport,
  bounded schema tools and reviewed proposals through existing history ownership.
- TOUCHED: AppChat, Assistant/AssistantUi, shared Commands/Services/ThemeCore,
  native window, assistant tests, validation runner and documentation.
- STATUS: PASS for embedded functional checkpoint A; final visual judgement is Curt's.
- PUBLISHED: `c5e09de9c43d34349e874a8cabfc17c6b373036d`; fetched and confirmed
  on origin/main after push. The following documentation-only commit records publication.
- VALIDATION: see `ASSISTANT_IMPLEMENTATION_REPORT.md` for final evidence.
- NEXT ACTION: Curt's final visual judgement; optional live MCP attachment B remains pending.
  AgentFlow integration is not part of this task.

## Current Theme Studio repair — UID-STUDIO-LIVE-01

### Local preset and theme-file workflow follow-up

Base: `013fcb8268f979228a6c8c58e1b9930bbdd53e01`.
Preset selection now uses WhenSelectData and silently synchronizes from the
ThemeDocument after load/undo. Presets preserve authored recipes; inherited
properties use the newly selected preset. Save Project retains design and theme;
Save Theme As and Load Theme exchange standalone theme JSON. Import and explicit
confirmed reset use Theme history, with Ctrl+Z/Y routed to Theme in Theme Studio.
Close also checks unsaved Theme changes. The code view shares export generation.
The gallery has four table rows and selectable ProgressRing/ChartRing samples.

Release validation: ThemeStudioRoleTest 380/0, ExportedThemeContractTest 27/0,
ThemeDocumentTest 31/0, UiTabThemePaintTest 21/0; all exit 0. Canonical Release
Designer build passes. Evidence: `build/StudioPreset-20260923-163421`.
Curt's interactive visual acceptance is pending. Palette-slot metadata retains
its existing meaning; this follow-up does not introduce a new global palette map.

BASE: `2ecd474b64d972a48f074feee0ebeee1d6133088` / main.
DEPENDENCY: upp_Ui `8114269abd91cc33569f68117bef4fd4d537897a` (native Tab paint repair).
TASK: live independent Panel/Control Role preview, populated Table, dark active Tab.
TOUCHED: `UiDesigner/Theme/UiDesignerThemeBuilderV2.{h,cpp}`;
`tests/ThemeStudioRoleTest/{main.cpp,ThemeStudioRoleTest.upp}`;
`docs/THEME_STUDIO_LIVE_ROLES.md`; this file.
STATUS: WINDOWS FOCUSED VALIDATION COMPLETE — CURT VISUAL ACCEPTANCE PENDING.
PUBLISHED: commit containing this entry; recover with `git log -1 -- tests/ThemeStudioRoleTest/main.cpp`.
VALIDATION: complete pinned originals reconstructed with matching Git blob hashes;
source/API/package/full-diff review and local git diff --check PASS. No Windows
compile or executable/visual result is claimed for this new checkpoint.
WINDOWS FOLLOW-UP: the first Release run passed UiTabThemePaintTest 21/0 but
exposed 30 deterministic ThemeStudioRoleTest failures. The bounded repair makes
UiPanel Standard resolve the universal role instead of the legacy Surface
fallback, makes the Slider adapter role-aware, and rebuilds Breadcrumbs role ink
when the live Control Role changes. Final Release validation passes
UiTabThemePaintTest 21/0 and ThemeStudioRoleTest 340/0, both exit 0; canonical
Release UiDesigner.exe also compiles and links successfully. The repair is
published in the commit containing this entry; Curt performs all visual
acceptance from the launched canonical executable. No full regression matrix here.

The role dropdowns now consume WhenSelectData, not the non-selection WhenAction.
Programmatic synchronization is silent and uses item data. Existing sample adapter
updates preserve independent role axes; omitted compound slider/editor and
breadcrumb samples now update too. Table has three named columns and six rows,
with mode/Control Role presentation independent of its GroupPanel's Panel Role.
Native UiTab no longer invents an OS light face for a transparent active cap.
Read THEME_STUDIO_LIVE_ROLES.md for scope, regression markers and visual checks.

The next section retains the separate DateTime validation boundary; it is not a
requirement to rerun that whole task before Curt can inspect these visual repairs.

## Retained DateTime checkpoint — UID-DATETIME-01

BASE: `a602b446810a57a25fbb2945ae0be2822f031bf7` / main.
Initial inspection base: `7a01c6aa2320a8d48902db028a98254710111ac0`.
DEPENDENCY INSPECTED: upp_Ui `57e8d38167cde7cee2bc62b0093979af86ca91ca`.
TASK: UiDateTime Designer catalog, scalar editor, Preview, Theme and generated runtime.
TOUCHED: Catalog nullable projection/registration and validation; new Core DateTime
value contract; Services registration/configuration; Editors registration/value editor;
new Preview/Theme/CodeGen adapters and their .upp membership; focused tests/fixture;
RunSupervisorValidation.ps1; this document and DATETIME_INTEGRATION.md.
STATUS: IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING.
PUBLISHED: `2ecd474b64d972a48f074feee0ebeee1d6133088`.
VALIDATION: complete touched-source reconstruction against fetched blob hashes;
source/API/package/dependency/diff review and local git diff --check. No Windows
compilation, executable run, or visual acceptance performed for this new checkpoint.
NEXT ACTION: run DateTimeIntegrationTest generated-fixture script in Debug, then
Release; rebuild canonical Designer and manually inspect Date/Time/DateTime through
Inspector and Data. Then run the combined Designer supervisor gate.

UiDateTime uses one local ISO datetime_value or null. Mode, seconds, bounds, null
policy and presentation are document configuration. Untouched hidden components
survive mode changes. Native Time values are emitted into generated C++; there is
no generated dependency on Designer, locale parsing or the wall clock. Null must not
be replaced by a property default. The typed-value editor supplies explicit Apply
as well as native picker commits; UI code stays in Editors, not headless Services.
See DATETIME_INTEGRATION.md and tests/DateTimeIntegrationTest/README.md.

The nine dark-theme/title files were not edited in this checkpoint. Main advanced
to a602b446 while this work was prepared; its changes were confined to those nine
files, and the new checkpoint is based on that tip. They are preserved, not replaced.

## Accepted/reported preceding work

UID-LOCAL-CLOSURE-02 at 7a01c6aa (with the dependency above) passed automated gates:
ChartRing Debug/Release 82/0 and complete/component runtime verifiers failed=0;
Tests 1292/0; Regression 79/0; Foundation 73/0; ExportedTheme 24/0;
CurrentUiIntegration 64/0; Theme ownership 89/0; adapter coverage 9177/0;
Theme builder 57/0; closure catalog 37/0; PropertyEditor 77/0;
UiThemeStructure 1092/0. All 12 generated presets built and the full Release
supervisor runner exited 0. Evidence locations are retained in
UID_LOCAL_CLOSURE_02_REPORT.md, not replaced by the new source review.

The senior subsequently reported ThemeDarkIntegration Debug/Release 21/0,
Regression Debug/Release 85/0, canonical Release application build PASS and
whitespace checks PASS for the dark surfaces/heading correction. The nine files
are now published at a602b446. This remote session did not execute those tests.
Curt reports the ProgressRing manual interaction and visual inspection passed.

Remaining visual acceptance is NOT silently marked complete:
- full manual Theme-role isolation;
- side-by-side Designer Preview versus exported application comparison;
- new DateTime picker/editor and mode/range/null/presentation interaction.

## Contracts and next scope

- Canonical Document and separate Theme; Commands own durable edits and undo.
- Tab/Page/content and Accordion/Section/content ownership stays unchanged.
- Theme preset/mode and explicit recipes compile into runtime output; active local
  overrides win, disabled/reset local overrides inherit, studio_preview is sample-only.
- Global palette/role-slot/metric metadata is not a new production style mapping.
- Complete/component generated applications do not search CWD for theme.json.
- Preserve user-owned code on re-export.
- Remaining new control integrations: UiColorMatrix, UiMatrixSelector, UiGallery.
- The native assistant direction is defined by AI_ASSISTANT_ARCHITECTURE.md;
  UID-ASSISTANT-01 supersedes the earlier embedded AgentFlow proposal.
