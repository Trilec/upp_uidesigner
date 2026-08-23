# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before acting, never force-update `main`, and preserve unrelated concurrent changes.

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
