# ACTIVE WORK

Remote main is authoritative. Sequential coding on main; no feature branch.
Fetch before work and immediately before publishing. Keep published and locally
reported validation separate. The graph project's own gates are not Designer gates.

## Edit bounds and button gallery refinement — 2026-09-26

- BASE: Designer `99c6b6d18bbcda30ef289130a0d010a8c4881953`; shared Ui
  `8c2a77bbcc412d42c881a7579d0d5a5289d1f851`. Clean main branches and no remote advancement.
- CAUSE: UiBaseEdit::LayoutSides used raw frame deflation, unlike the ordinary
  text path. Numeric fields retain side items even with spin buttons hidden, so
  their text background could cover the lower frame when a shadow was enabled.
- FIX: side layout now uses the canonical styled face (shadow, resolved frame,
  skin inset), applying text margins once afterwards. Shared fix applies to all
  editors with side controls, without changing Theme recipes.
- PREVIEW: measured button row heights/widths, wider first button, smaller share
  for the split button, and a 12-pixel logical gap above breadcrumbs. Dropdown
  group heading shortened to Defaults.
- REGRESSION: 36 numeric checks cover integer/decimal, spin visible/hidden,
  no/positive/negative shadow offsets, skin insets, side bounds and lower-frame
  pixels. Debug Embedded PASS (`build/Assistant-Debug-20260926-115302`), including
  AssistantDesigner 159/0. Release Embedded and launch PASS
  (`build/Assistant-Release-20260926-115459`): AppChat 23, AssistantDesigner 159,
  TitleGrid 37, Regression 88, ThemeDocument 35, ThemeStudioRole 1854,
  ExportedThemeContract 36; zero failures. Native saved Brutalist Light/Dark
  inspection confirms intact numeric borders, readable button icons and breadcrumb gap.
- SHARED DEPENDENCY: `122cca0beab1e765847e4f48e47b6696a341f6ef`.
  Canonical `build/UiDesigner.exe` SHA256
  `3C5AD6EC9EF1B9827F876D67EA9FE335E988F8BB73892B6CD65C3C4A44D76CAD`,
  launched PID `387616`. No live provider call needed for these rendering changes.
- DESIGN ONLY: proposed Themes tree with project-owned drafts, duplicate/rename/
  delete, and explicit Add to My Themes for reusable snapshots. Project Save
  would preserve project themes; Theme Save/Publish and application export remain
  separate. No multi-theme ownership or deletion behavior implemented in this fix.

## Theme rendering refinement — 2026-09-25

- BASE: Designer `399c8dacf019a09ccdce29b2688d8f381c77c2ad`; shared Ui
  `d5dc1beb3d36559caca0eb4d15db202b4162b879`. Both clean main branches;
  fetched before work with no remote advancement.
- CAUSES: ProgressBar measured its fill/text against the shadow-bearing outer
  track. Tree omitted surface/font recipes; Table/Breadcrumbs bypassed recipes.
  Tab text/icons, Accordion header icons and Dropdown indicator icons were missing from their schemas;
  Slider's active track retained a preset ink behind the legacy thumb alias.
  Breadcrumbs replaced explicit custom colours in SetCustomStyle. Generated
  chart series deliberately retained preset blue. Gallery rows used fixed heights
  and child bounds did not account for the group's styled body. Table painted its
  viewport across the outer frame; its viewport and scrollbars now use the styled face.
- FIXES: inner-track progress geometry and clipped percentage painting; explicit
  data-control recipes with export parity; missing semantic inks exposed; authored
  Breadcrumb colours preserved; palette-derived chart series; enabled borders,
  border-seed-anchored role outlines, one outer elevation, measured editor/progress
  rows and panel-body containment. Manual overrides still win. Table/Breadcrumbs
  are selectable in the Studio Inspector. Role APIs remain independent of the
  Designer's authoring policy.
- LIMIT: the existing Slider dashed setting describes its frame; a dedicated
  dashed active-track style is not implemented. Assistant guidance says so.
- VALIDATION: Debug Embedded PASS (`build/Assistant-Debug-20260925-221857`).
  Release Embedded and canonical launch PASS (`build/Assistant-Release-20260925-222006`).
  AppChat 23, AssistantDesigner 123, TitleGrid 37, Regression 88, ThemeDocument 35,
  ThemeStudioRole 1854, ExportedThemeContract 36; zero failures. Standalone
  ProgressBar 35/0 (`build/ThemeRefine-Progress.log`). Portable schema comparison:
  all 68 controls match the CLI; updated `build/uidesigner-design-skill.zip`.
- LIVE: native OpenRouter `deepseek/deepseek-v4-flash` generated two valid review
  proposals, each using 5 calls in 4 rounds with no errors (limit 16 unchanged).
  Latest: round 1 inspect_theme + retrieve_skill; round 2 list_fonts +
  inspect_theme_control; round 3 prepare_theme_design; round 4 final response.
  Native Keep succeeded; Undo/Redo restored the previous/candidate appearance.
  Preview left the blank design unchanged. Saved through Save Theme As to
  `build/Brutalist-refined.theme.json`; original `Brutalist-review.theme.json`
  remains untouched. Reload verified theme persistence. Light and Dark reviewed.
- REQUIRED SHARED UI: `8c2a77bbcc412d42c881a7579d0d5a5289d1f851`.
- EXECUTABLE: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`,
  SHA256 `F086B89DD74A7F99164792D676B76402E739287CEDB95B444A4C233BFE078A4D`,
  review PID `305880`. Built from this change with the shared dependency above.

## AI Theme proposals and portable JSON design skill — 2026-09-25

- BASE: Designer `6bd8c8479893526be4d664e7f919bafacb5ae53e`; shared Ui
  `68ca57683423e60823e37471865a2ce9be991c7f`. Main preserved; remote refresh
  showed no advancement before publication.
- IMPLEMENTED: Light/Dark six-seed generation into explicit recipes for four
  roles; optional installed body/heading fonts, size/bold hierarchy, hard outer
  shadows and line widths. Manual overrides remain authoritative. Proposal preview,
  Compare, targeted refinement, human Keep, Theme Undo, named Save/Load and a
  grouped dropdown distinguish temporary proposals from saved themes.
- DIAGNOSED: Slider/ScrollBar FillRecipe aliases collided with generic ink;
  ProgressBar ink alias targeted track instead of fill. Apply/inspect/export now
  agree. Native preview exposed transparent source-over failing to cut out hard
  shadow interiors; shared Ui now applies the existing cutout mask explicitly.
  Gallery children respect measured group headers when fonts become larger.
- LIVE: OpenRouter `deepseek/deepseek-v4-flash` generated typography/shadows and
  a palette in 5 calls/3 rounds, then refined only the requested Accordion title
  in 4 calls/4 rounds. `ThemeLive4.stdout.log`: 10 checks, 0 failures, including
  no durable mutation, trusted Keep and one-step Undo. Earlier failures are retained:
  refinement discovery, gallery alias exception, and rejecting integral JSON
  numbers represented as floating point; each drove a concrete repair.
- SKILL: `skills/uidesigner-design` uses the existing schema-4 document JSON;
  three complete examples, 68 exported control schemas, layout/role guidance and
  portable validation. All examples pass portable + canonical validation;
  simple dialog generates and compiles as a standalone native package.
  `build/uidesigner-design-skill.zip` is the portable distribution.
- LIMITS: no embedded reference-image attachment, generated textures or decorative
  outline typography; no persistent Use-for-Designer preference, crash-recovery
  drafts or dedicated role-swatch matrix. Solid fill refinements are supported;
  other structured fill modes are not yet exposed to assistant edits. Designer
  chrome still follows its preset; recipes affect samples and document preview.
- FINAL OFFLINE: Debug `Assistant-Debug-20260925-210740` and Release/Launch
  `Assistant-Release-20260925-210817` PASS. AssistantDesigner 107/0;
  ThemeDocument 35/0; ThemeStudioRole 1854/0; ExportedThemeContract 36/0;
  AppChat 23/0; TitleGrid 37/0; Regression 88/0. Shared drawing tests:
  3 suites/0 failures, surface-cache 16/0 including shadow pixels.
- DEPENDENCY: shared Ui tested/published `d5dc1beb3d36559caca0eb4d15db202b4162b879`;
  fetched and confirmed equal to origin/main.
- EXECUTABLE: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`, PID `353648`,
  SHA256 `69E4E6CF8976E89E408F7D8F3862660BC65482141ED79DEC98AED032812BACC2`.
- NATIVE: live proposal rendered; Compare restored the original, Keep committed,
  one Undo restored the original, Redo restored the proposal, Save Theme As created
  `build/Brutalist-review.theme.json`. Restart and My themes reload passed on the
  final binary. Light/Dark reviewed; saved Light theme left open.
- VISUAL LIMITATIONS: Table composition and Tree background/frame still include
  inherited preset styling; categorical ring colours stay separate. Dark progress
  ink on bright fill needs contrast-policy refinement. This is working authoring
  infrastructure and an editable starting point, not final visual parity with the
  reference art or complete palette coverage. The gallery still uses fixed sample
  control heights; extreme body fonts can clip even though headers are measured.

## Theme Studio Inspector hang and legacy audit — 2026-09-25

- BASE: Designer `74a8c15f108ca51f3daa400c99a045c3b6261be3`; shared Ui
  `68ca57683423e60823e37471865a2ce9be991c7f` (unchanged).
- REPRODUCED: blank Theme Studio hangs after mixed sample/group selection.
  Matching-symbol stack confirms recursive legacy/V2 target synchronization
  through the global shell-theme preview callback.
- REPAIR: separate Inspector target events from rendered-theme changes; one
  shared binding lifecycle with concrete target/style/model policies. Preserve
  genuine transient-preview cancellation and normal Theme history.
- CLEANUP: remove obsolete gallery target/style implementations, duplicate V2
  binders and two unreferenced DesignOverlay prototype headers. Retain live
  shared sample/layout/toolbar code and production interaction overlays.
- GALLERY: removed reference panel/button; left column is Buttons/Data/Rings.
  Assets now expands to Red/Green/Blue with a narrow On/Off column.
- DESIGN: `THEME_WORKFLOW_AND_CLEANUP.md` records audit evidence and a proposed
  integrated theme library/draft/proposal/save/export workflow. That future UI
  is not implemented by this repair.
- VALIDATION: Debug Embedded PASS (`Assistant-Debug-20260925-192339`);
  Release Embedded/Launch PASS (`Assistant-Release-20260925-192424`).
  ThemeStudioRole 1854/0; ThemeDocument 33/0; ExportedThemeContract 36/0;
  AssistantDesigner 80/0; AppChat 23/0; TitleGrid 37/0; Regression 88/0.
  Additional ThemeBuilderContract 57/0 and ThemeDarkIntegration 21/0 passed.
- NATIVE: repeated the formerly hanging Button/Accordion/List/ProgressBar/
  group/Line Edit selection sequence on the final Release: Inspector updates
  promptly and the process remains responsive. No live provider test was needed
  or run for this local lifecycle repair. Canonical application left open.
- EXECUTABLE: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`, PID `232812`,
  SHA256 `A24E1FA728439B33A160C1E45337164B45418E8B39C5EC9CE1324C0DCB32E931`.

## Role default consistency repair — 2026-09-25

- BASE: Designer `94b1f643cc3c7dabbb59cc68fbc79440791ea66e`; shared Ui
  `ed635bc1dcbbd0d23524d1184843995c239c0963`. Remote advancement inspected;
  both were current and clean before implementation.
- REPAIR: List/Tree/Toggle role propagation, shared Accordion defaults, Tab
  role ink/active-frame preservation, Alert ProgressBar track, role colours
  across presets. Inspector, preview and export use the same resolved defaults.
- OWNERSHIP: role baseline, then saved Theme recipe, then local override.
  Existing custom-style APIs, history and save/export ownership are preserved.
- DETAILS: `ROLE_DEFAULT_REPAIR.md` documents causes, intentional default
  changes and the separate future global-palette bridge. No AI palette tool
  or new palette model was introduced in this repair.
- DEPENDENCY: tested/published shared Ui
  `68ca57683423e60823e37471865a2ce9be991c7f`; fetched and contained in origin/main.
- VALIDATION: Debug Embedded plus Broader PASS (`Assistant-Debug-20260925-173142`);
  Release Embedded plus Launch PASS (`Assistant-Release-20260925-173418`).
  ThemeStudioRole 1836/0; ExportedThemeContract 36/0; adapter coverage 9177/0.
  Shared structure 1260/0, Tab paint 21/0, surface 13/0; logs under
  `build/RoleRepair-*`. Existing document/layout/assistant gates also passed.
- NATIVE: final canonical gallery checked in Alert, light and dark. No live
  provider request was made for this control/theme repair. Existing dark button
  and table contrast merits separate palette tuning; no blanket contrast claim.
- EXECUTABLE: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`, PID `24548`,
  SHA256 `4260D46C3D064F89C34B339CB6C47A1650D3883A93B0A69C34C4B477984E4325`.

## Assistant conversation UI and reuse reference — 2026-09-25

Follow-up: outside-in design guidance now covers Grid versus wrapping Box,
expansion, TitleCard's single content slot, spacers and icon/reference limits.
Added an insertable DialogTemplate and executable Grid/Label example. See
`ASSISTANT_DESIGN_GUIDANCE.md` for the preset audit and proposed reference inputs.
The same follow-up repairs scrolled catalogue rows painting over filters and
places consistent Accent section headings beneath both sidebar icon strips.
Theme guidance now covers palette/style intent from HTML/CSS and truthfully
limits executable changes to supported recipes, not global style packs.
Follow-up validation: Debug/Release passed; AssistantDesigner 80/0, Regression
88/0, live creation/refinement 26/0; all 13 preset exports compiled. Native
sidebar headings/scrolling checked; canonical Release left open. Evidence and
remaining palette/reference limitations are in `ASSISTANT_DESIGN_GUIDANCE.md`.

- BASE: Designer `d95d50462872fa10aa2ae70bdb410ac9fdd60fd1`; shared Ui
  `ed635bc1dcbbd0d23524d1184843995c239c0963` (unchanged).
- TASK: refine the lower assistant drawer from Curt's mockup and establish a
  reusable native conversation component for other applications.
- IMPLEMENTED: independent `AppChatUi` package, standalone consumer, chronological
  folded cards, explicit History, Clear all, read-only Show code, per-proposal
  actions and Refine, bottom profile settings, one Assistant/Collapse toggle.
- AUTHORITY: AppChatUi owns presentation only. Host owns proposals/validation;
  document/Theme services retain Apply and Undo. No TitleCard extension needed.
- GUIDANCE: explicit Label heading takes precedence over TitleCard examples;
  applied refinements target existing node IDs instead of inserting duplicates.
- REUSE: `REUSABLE_ASSISTANT_UI.md`; evidence in `ASSISTANT_CONVERSATION_REPORT.md`.
- VALIDATION: final Debug/Release Embedded PASS; AssistantDesigner 73/0; separate
  OpenRouter live 26/0; native Apply/refine/Undo checked. Canonical app left open.

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
