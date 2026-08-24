# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

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
