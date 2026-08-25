# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

## THEME ACCEPTANCE REPAIR R3 — 2026-08-25

BASE:

- `upp_uidesigner/main`: `c0e0a16d8c70d349ac8ebe9ee4dc3ea73750158f` at supervisor refresh.
- `upp_Ui/main`: `78c8bfb55f1b351ed7d63c66f467eaf308dd7b44` at supervisor refresh.

TASK: `THEME-ACCEPT-R3` — close the retained adapter-suite catalog failure caused by duplicate normal/Theme `UiTab` property ids without renaming the older normal Designer schema or weakening catalog validation.

TOUCHED:

- `UiDesigner/Theme/UiDesignerTabThemeRuntimeAdapter.cpp`
- `tests/ThemeAdapterCoverageTest/main.cpp`
- `docs/ACTIVE_WORK.md`

STATUS: **IMPLEMENTATION COMPLETE — WINDOWS REVALIDATION PENDING.**

SOURCE CHECKPOINT: `ff492cadc3f8141e2b3a56f92abdae50bb874335` on `work/theme-tab-schema-r3`; this bookkeeping commit is a descendant of the reviewed source/test repair. It is not acceptance evidence until Windows validation passes.

REPORTED R2 WINDOWS VALIDATION:

- Exact tested heads: `upp_uidesigner c0e0a16d8c70d349ac8ebe9ee4dc3ea73750158f`; `upp_Ui 78c8bfb55f1b351ed7d63c66f467eaf308dd7b44`.
- `git diff --check`: PASS; both worktrees clean.
- Focused `PreviewLayoutRegressionTest`: `checks=23 failed=0`; both structural Grid rebuilds preserve 1x3 and runtime Grid reports three cells.
- Principal Debug: ThemeAdapterCoverage `3721/0`, ThemeBuilderContract `42/0`, ThemeDocument `31/0`, PreviewLayout `23/0`.
- Principal Release: same four summaries PASS.
- UiDesigner Debug and Release builds: PASS with no compile/link errors.
- Retained Debug adapter suites each failed only because `catalog.Validate()` reported `UiTab.tab_font_face duplicates a normal property id`: Label `242/1`, List/Edit `114/1`, Dropdown/Accordion `103/1`.
- Manual smoke and branch cleanup were correctly skipped after the deterministic retained-suite stop condition.

R3 DIAGNOSIS / REPAIR:

- `NormalizeTab()` already exposed normal Designer ids such as `tab_font_face`, `tab_padding_*`, `strip_inset_*`, `affordance_gap`, `min_tab_main` and `visual` before the complete Tab Theme adapter was added. Preserve that older normal Designer document contract.
- The later Tab Theme adapter already used distinct `style_*` public ids for overlapping fields including `style_tab_extent`, `style_item_spacing`, `style_body_gap`, `style_content_gap`, `style_expand_tabs` and `style_active_tab_uses_body_face`, but fourteen overlaps were left unprefixed.
- R3 completes that existing Theme-side convention for the remaining overlaps: tab font face/bold/italic, tab padding four sides, strip inset four sides, affordance gap, minimum tab length and visual.
- Only the Theme override `id` is prefixed. `adapter_field_id` remains the canonical runtime field (`tab_font_face`, `visual`, etc.), so base adapter apply/resolve/codegen and the runtime `SetVisual()` bridge keep their existing semantics.
- `ThemeAdapterCoverageTest` now validates the whole catalog before adapter assertions and uses `style_tab_font_face` / `style_visual` as the public Tab Theme ids. This makes the principal coverage gate catch future normal/Theme namespace collisions directly.
- No normal Designer property id, UiTab runtime API, base Tab theme field implementation, Theme document structure, or catalog validation rule was weakened.

NEXT ACTION:

1. Fetch current `upp_uidesigner/main`; confirm R3 source checkpoint `ff492cadc3f8141e2b3a56f92abdae50bb874335` is an ancestor of tested HEAD.
2. First run `ThemeAdapterCoverageTest` CLANGx64 Debug. Expected summary after the added catalog assertion is `checks=3722 failed=0`.
3. Then run the retained Debug Label, List/Edit and Dropdown/Accordion adapter suites; all must return `failed=0` and no catalog error.
4. If green, rerun the four principal Debug + Release acceptance suites, UiDesigner Debug + Release builds, then perform the existing manual Theme Studio/PropertyEditor smoke.
5. Stop on any substantive failure. Do not relax catalog uniqueness, rename normal Tab properties, or delete cleanup branches until full Theme acceptance passes.

---

## THEME ACCEPTANCE REPAIR R2 — 2026-08-25

BASE:

- `upp_uidesigner/main`: `b18e63ce65ef8bb562991fcf02a84e8d12bc7742` at supervisor refresh.
- `upp_Ui/main`: `cccda9916e32fd9bd8d3a0d041cf10e3848546f7` at supervisor refresh. Comparison from `1284a7d72c8e1c83c6fb4f1c4f8ea56c194f67eb` to this HEAD is two commits ahead with no changed files; the repository tree is unchanged across that range.

TASK: `THEME-ACCEPT-R2` — repair the deterministic `PreviewLayoutRegressionTest` Grid reconstruction failure without weakening the regression test or duplicating Grid structural state.

TOUCHED:

- `UiDesigner/Preview/UiDesignerPreview.h`
- `UiDesigner/Preview/UiDesignerGridPreviewAdapter.cpp`
- `docs/ACTIVE_WORK.md`

STATUS: **IMPLEMENTATION COMPLETE — WINDOWS REVALIDATION PENDING.**

SOURCE CHECKPOINT: `9962c1cdb683bc5712758d539fb17b0d301f8c54` on `work/theme-preview-grid-rebuild-r1`; this bookkeeping commit is a descendant of that source repair. Neither commit is acceptance evidence until Windows validation passes.

REPORTED WINDOWS DEBUG VALIDATION:

- `ThemeAdapterCoverageTest`: `checks=3721 failed=0`.
- `ThemeBuilderContractTest`: `checks=42 failed=0`.
- `ThemeDocumentTest`: `checks=31 failed=0`.
- `PreviewLayoutRegressionTest`: `checks=23 failed=3`.
- The exact tested `upp_uidesigner` HEAD was not restated in the result report; do not infer it. The prior mandatory adapter gate nevertheless passed, including the three R1 regressions.
- Release tests, UiDesigner Debug/Release builds and manual smoke were correctly not run after the deterministic PreviewLayout stop condition.

R2 DIAGNOSIS / REPAIR:

- The document remains authoritative and preserves Grid `rows=1`, `columns=3` after child insertion. The failure occurs only after structural preview reconstruction: the fresh runtime Grid exposes four cells, i.e. its 2x2 catalog/runtime default.
- Live `rows` / `columns` changes already use the document pair together through `SetGridSize(cols, rows)`, so the defect is not document mutation or `UiGridLayout` pair semantics.
- `UiDesignerGridPreviewAdapter.cpp` already owns the correct reconstruction model: `UiDesignerGridPreviewCtrl` stores rows/columns and minimum-cell dimensions as coupled preview state and replays each field without losing its paired value.
- That translation unit previously contained only a file-static adapter-registration object. Preview packages are linked through package archives; a registration-only object has no referenced external symbol and may be discarded by the linker. In that case `UiDesignerPreviewFactory::Adapter()` lazily installs the generic runtime adapter instead.
- The generic `ApplyRuntime()` intentionally returns `RequiresSubtreeRebuild` for `rows` / `columns`. During `BuildNode()`, `ApplyAllProperties()` replays properties but does not consume those return values, so a fresh generic Grid remains at the 2x2 default. This exactly matches the observed four-cell failure.
- R2 removes the fragile file-static registration. `UiDesignerPreviewAdapterRegistry` now has an explicit constructor defined in `UiDesignerGridPreviewAdapter.cpp`; construction registers the Grid adapter. `UiDesignerPreview.cpp` therefore has a concrete constructor-symbol dependency on the Grid adapter translation unit, forcing that object into the link and making registration deterministic.
- No Grid geometry algorithm, document property, catalog default, runtime `UiGridLayout` API, Theme code or regression expectation was changed.
- Reviewed R2 source diff contains exactly two source/header paths: one constructor declaration and the registration/linkage correction.

NEXT ACTION:

1. Fetch current `upp_uidesigner/main`; confirm the published R2 source checkpoint `9962c1cdb683bc5712758d539fb17b0d301f8c54` is an ancestor of the tested HEAD before validation.
2. Run `tests/PreviewLayoutRegressionTest` under CLANGx64 Debug first. Require `PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=23 failed=0`. Specifically confirm both structural rebuilds preserve Grid 1x3 state and the runtime Grid reports three cells.
3. If the focused gate passes, rerun the four Debug acceptance suites as one set, then proceed to Release versions, UiDesigner Debug/Release builds and the existing manual smoke.
4. Stop on any substantive failure. Do not weaken `PreviewLayoutRegressionTest`, reintroduce duplicate Grid state, or delete cleanup branches until full Theme acceptance passes.

---

## THEME ACCEPTANCE REPAIR — 2026-08-24

BASE:

- `upp_uidesigner/main`: `dfde894feedf791e74a2753fb89334ad7f6de5d2`.
- `upp_Ui/main`: `e1af79a92ed98186b6a16f43504566c492188a97` at supervisor refresh; its advance beyond `7309358f51af357d5676d376658996c3c13c5486` contains test/example image assets only.

TASK: `THEME-ACCEPT-R1` — repair the three `ThemeAdapterCoverageTest` failures reported by Windows Debug validation without redesigning Theme Studio or changing established serialized schemas.

TOUCHED:

- `UiDesigner/Theme/UiDesignerCoreControlThemeAdapters.cpp`
- `tests/ThemeAdapterCoverageTest/main.cpp`
- `docs/ACTIVE_WORK.md`

STATUS: **IMPLEMENTATION COMPLETE — PLATFORM VALIDATION PENDING.**

PUBLISHED: `6fdd752b5514ee7c1fd41a26e0a216809a7bbf83` — reviewed source/test repair checkpoint; this bookkeeping commit records the recovery state on top of it.

VALIDATION / DIAGNOSIS:

- Windows `upp_Ui` Debug + Release on `7309358f51af357d5676d376658996c3c13c5486` passed `PropertyEditorV1RunTests` (`156 Fails: 0`), `PropertyEditorSemanticRunTests` (`checks=44 failed=0`) and `PropertyEditorTests` (`72 Fails: 0`).
- Initial `upp_uidesigner` Debug validation on `b471a24531b3fd4ecd55e98341af5aca4edd3047` exposed stale `UiProgressBar::Style` references to removed `min_thickness` / `show_text` members. Current remote `dfde894...` already contains the narrow correction removing those invalid projections while preserving Track/Fill palette, metrics, radius and typography coverage.
- After that mechanical Progress correction, `ThemeAdapterCoverageTest` reported `checks=3719 failed=3`: ScrollBar `grip_color` ownership, Label `face_normal`, and Dropdown fake-skin detection.
- ScrollBar was the production defect: `grip_color` is a real authored field and is implemented by Add/Apply/Resolve/Emit, but its legal `Null` default made value-based `HasField()` incorrectly report no ownership. The adapter now explicitly owns `grip_color` without changing its runtime default.
- Label was a coverage-test schema mismatch: the established Label adapter and dedicated regression suite use the stable serialized id `face.normal`. The broad coverage test now uses that canonical id rather than introducing a rename to `face_normal`.
- Dropdown was a coverage-test classification error: `popup_use_main_skin` is a real boolean style policy, not a resource-backed skin editor. The resource guard now rejects unresolved `skin_image` fields instead of rejecting every legitimate property whose name merely contains `skin`.
- Source/test diff was reviewed from `dfde894...` through `6fdd752...`; it contains only the one ScrollBar ownership correction and the two intended coverage-contract corrections.
- Windows Debug/Release revalidation of `6fdd752...` or a descendant remains mandatory. No visual acceptance or branch deletion is authorized yet.

NEXT ACTION:

1. Fetch current `upp_uidesigner/main` and confirm `6fdd752b5514ee7c1fd41a26e0a216809a7bbf83` is an ancestor of the tested HEAD.
2. Run `tests/ThemeAdapterCoverageTest` under CLANGx64 Debug first; require `failed=0` before resuming the remainder of the Theme Studio matrix.
3. If green, run the remaining Debug/Release Theme suites and UiDesigner builds, then perform the existing visual smoke including Button rebuild safety, Tab visual families, PropertyEditor affordances and Light/Dark/role isolation.
4. Stop on any substantive failure and return it to the supervisor. Do not delete cleanup branches until full Theme acceptance passes.

---

## CURRENT STATE — 2026-08-23

STATUS: **THEME STUDIO / DESIGNER / PROPERTYEDITOR API-PARITY SOURCE WORK COMPLETE; WINDOWS DEBUG/RELEASE + VISUAL ACCEPTANCE PENDING.**

Source checkpoints immediately before this bookkeeping update:

- `upp_uidesigner/main`: `5ac1a4168dc59e83c7badafbdb2e72343b59571c`.
- `upp_Ui/main`: `7309358f51af357d5676d376658996c3c13c5486`.
- PropertyEditor Boolean rebuild-safety ancestor: `ceb70739a97f5de25f23505114be24ff8607c359`.
- PropertyEditor mouse-override test repair: `527859915efefa2c0ab918a7f9f8bb6bf6e7a939`.
- PropertyEditor inherited/reset presentation source: `be3f2eb9a1180041675964749471aa369315dda4`.

Do not claim Windows acceptance until the exact then-current heads are validated.

## OWNERSHIP

The supervisor owns architecture, diagnosis, substantive implementation, review and publication. Gary performs Windows/U++ builds, focused tests and visual smoke, making only mechanical build/API corrections. Substantive failures return to the supervisor.

## LOCKED THEME STUDIO CONTRACT

Theme Studio is a whole-application Theme Builder, not a second Designer document.

- Controls / Containers modes only.
- Panel Role: Surface / Subtle / Strong.
- Control Role: Standard / Subtle / Accent / Alert.
- Light and Dark remain independent.
- Six Light + six Dark swatches remain visible; the production `UiColorPicker` opens transiently and commits the six-colour palette atomically.
- Selecting a sample begins the PropertyEditor with read-only Identity: Control, Type, Appearance, Role, Scope.
- Durable theme fields sit beneath Appearance and use real runtime ownership rather than generic Surface/Ink aliases.

Common StyledPalette/StyledMetrics vocabulary:

`General -> Face -> Frame -> Text Ink -> Icon Ink -> Typography -> Content Margin -> Focus -> Shadow -> Highlight -> Additional`

Composite controls retain genuine domains such as Indicator, Track, Thumb, Fill, Arrow, Popup, Header, Body, Rows, Badge, Drag and Tab.

`UiFill` fields use the real FillRecipe editor (Solid / None / gradient). Resource-backed `StyledSkin` / image fields remain deferred until Theme adapters receive the real Designer document-resource resolver. Do not expose fake filesystem paths. The eventual user-facing label is `Skin (Nine Slice)`.

## PROPERTYEDITOR ALIGNMENT

Normal Designer and Theme Studio reuse the same production PropertyEditor metadata and editors.

- bounded NumericInt/NumericDouble keep the numeric field and slider-toggle;
- focused numeric wheel edits the numeric value, not the outer PropertyEditor scroll;
- colours keep picker/dropdown, DnD and clipboard hex;
- Left/Right/Top/Bottom keep Cardinal4;
- icons keep the shared chooser;
- point and Bézier curves remain first-class;
- authored/inherited state never replaces the editor's own affordance;
- resettable Theme Studio/preview rows show a passive inherited marker while inherited and expose Reset once authored;
- Reset returns to inherited; Undo restores authored state;
- Light/Dark and role targets remain isolated.

The old broad `PropertyEditorTests` 70/2 result came from stale absolute mouse coordinates after filter/group geometry evolved. `527859915...` derives the override/body clicks from live PropertyEditor geometry while retaining both interaction assertions.

## COMPLETED ADAPTER COVERAGE

- **Button / SplitButton / ToolButton:** real FillRecipe faces, Frame, Text/Icon Ink, Typography, Content Margin, Focus, Shadow, Highlight and control-specific fields while preserving established serialized IDs.
- **CheckBox:** body + Indicator palettes/metrics, typography, alignment, indicator geometry, mark thickness and marker render mode.
- **RadioButton:** body + Indicator palettes/metrics, typography and indicator geometry.
- **Toggle:** Track / Thumb style domains and control-specific geometry/appearance.
- **ProgressBar:** separate Track and Fill palette/metrics, including the previously missing Track and Fill radii.
- **Slider:** separate Track and Thumb palette/metrics, radii, ticks, geometry and thumb-ring fields.
- **ScrollBar:** Track, Thumb and Arrow palette/metrics/radii, arrow layout, thumb length, expansion/fade, grip and insets.
- **Panel:** full palette/metrics, transparent mode and stable existing frame recipe IDs.
- **ScrollPanel:** full panel surface style.
- **GroupPanel:** panel style plus Header ink/typography/layout/insets/spacing, separator/header-band and Body inset.
- **Tab:** Body + Tab-item palettes/metrics, typography, spacing, padding/strip inset, icon size/side, indicator, active frame/open-corner, expansion and Classic/Underline/Segmented/Rail/Document visual families.
- Existing rich Label, Edit, Dropdown, List, Tree and Accordion adapters remain authoritative.

`UiTab` visual family is runtime instance state as well as Style metadata. `UiDesignerTabThemeRuntimeAdapter` therefore applies `UiTab::SetVisual(...)` during preview and emits it during generated setup; custom-style setup is seeded from the same authored visual family.

## PREVIEW-PRESENTATION LAYER

Sample-only presentation remains in `UiDesignerThemeSnapshot::studio_preview`, separate from durable `style_overrides`.

Where the normal Designer catalog exposes them, Theme Studio copies the exact `UiDesignerPropertySpec` and uses `UiDesignerPreviewFactory` for icon chooser/render mode, icon side, icon dimensions/size, scale-to-content, content gap and alignment. Style-owned insets use the actual Content Margin/control-specific theme fields rather than a competing preview value.

## TEST GATES

`upp_Ui`, Debug + Release:

- `Utilities/PropertyEditorV1RunTests` -> `Fails: 0`
- `Utilities/PropertyEditorSemanticRunTests` -> `failed=0`
- `Utilities/PropertyEditorTests` -> `Fails: 0`

`upp_uidesigner`, Debug + Release:

- `tests/ThemeAdapterCoverageTest` -> `THEME_ADAPTER_COVERAGE_SUMMARY ... failed=0`
- `tests/ThemeBuilderContractTest` -> `THEME_BUILDER_CONTRACT_SUMMARY ... failed=0`
- `tests/ThemeDocumentTest` -> `THEME_DOCUMENT_SUMMARY ... failed=0`
- `tests/PreviewLayoutRegressionTest` -> `PREVIEW_LAYOUT_REGRESSION_SUMMARY ... failed=0`
- retain Label, List/Edit and Dropdown/Accordion focused adapter suites as complementary regression gates.

Build `UiDesigner` Debug + Release.

Manual Debug smoke:

1. Button -> Layout -> toggle Scale icon to content repeatedly: no crash; preview + commit remain valid.
2. Theme Studio Identity shows Control / Type / Appearance / Role / Scope.
3. Select Label, Button, CheckBox, RadioButton, Toggle, ProgressBar, Slider, ScrollBar, Panel, GroupPanel, ScrollPanel and Tab; Appearance domains match the real API and edits visibly affect the sample.
4. Exercise Progress Track/Fill radius, Slider Track/Thumb radius, ScrollBar Track/Thumb/Arrow radius.
5. Tab Classic / Underline / Segmented / Rail / Document visibly changes the sample; Body, Tab and Indicator styling remain independent.
6. Exercise numeric slider-toggle + focused wheel, colour picker, Cardinal4, icon chooser, inherited marker/Reset and Undo.
7. Check Light/Dark and semantic-role isolation.

## BRANCH HYGIENE

Do not create another branch for this tranche.

Latest verified cleanup candidates (`ahead_by=0` versus `main` when audited):

`upp_uidesigner`:
- `work/label-fillrecipe-parity`
- `work/label-reference-audit-closure`
- `work/label-registry-switch`
- `work/model-api-preview-migration`

`upp_Ui`:
- `agent/uibutton-interaction-hardening`
- `temp-demo-button-do-not-use`

Gary must re-check containment against current `main` immediately before deleting any candidate.

**Do not delete either UiGraph reconciliation branch during this validation:**

- `agent/uigraph-interaction-reconcile`
- `agent/uigraph-interaction-reconcile-r1`

Concurrent UiGraph work has been advancing `main`, and these branches still compare as diverged/unique by commit identity. Reconcile that work separately before deleting those refs.

## OUT OF SCOPE / NOT YET COMPLETE

Do not claim these are complete:

- standalone Theme JSON import/reuse UI;
- universal generated/runtime consumption of arbitrary Theme Studio recipes as a replacement for every global `upp_Ui` resolver;
- document-resource-backed Theme skin/glyph editing.
