# ACTIVE WORK

Remote GitHub `main` is authoritative. Refresh exact remote HEAD before work and before publication; never force-update `main`; preserve unrelated concurrent changes.

## CURRENT SUPERVISORY STATE — 2026-08-23

STATUS: **THEME STUDIO / DESIGNER / PROPERTYEDITOR API-PARITY SOURCE WORK COMPLETE; WINDOWS DEBUG/RELEASE + VISUAL ACCEPTANCE PENDING.**

Source checkpoint before this bookkeeping commit:

- `upp_uidesigner/main`: `91a699bdab89f86ed48e494ccfa4c6ce8859c9ce` — complete Theme Studio adapter coverage documented.
- `upp_Ui/main`: `527859915efefa2c0ab918a7f9f8bb6bf6e7a939` — PropertyEditor mouse-override regression now derives clicks from live PropertyEditor geometry.
- Important `upp_Ui` ancestor: `ceb70739a97f5de25f23505114be24ff8607c359` — Boolean PropertyEditor callbacks are snapshotted before preview dispatch so a preview-triggered PropertyEditor rebuild cannot clear the subsequent commit callback.

Do not claim Windows acceptance until Gary validates the exact then-current `main` line.

## OWNERSHIP

The supervisor owns architecture, diagnosis, substantive implementation, source review and publication. Gary performs Windows/U++ builds, tests and visual smoke, making only genuinely mechanical build/API corrections. Substantive failures return to the supervisor.

## THEME STUDIO CONTRACT

Theme Studio is a whole-application Theme Builder, not a second Designer document and not a static control gallery.

Toolbar contract:

- Controls / Containers preview modes only;
- Panel Role: Surface / Subtle / Strong;
- Control Role: Standard / Subtle / Accent / Alert;
- Light / Dark are independent;
- six Light + six Dark palette swatches remain visible;
- the production `UiColorPicker` opens transiently from a swatch and commits all six colours atomically.

Selected sample PropertyEditor now begins with read-only **Identity**:

- Control;
- Type;
- Appearance (Light/Dark);
- Role;
- Scope (Control-role or Panel-role recipe).

Durable theme fields are presented under **Appearance** using the runtime ownership vocabulary. Common StyledPalette/StyledMetrics areas are:

- General;
- Face;
- Frame;
- Text Ink;
- Icon Ink;
- Typography;
- Content Margin;
- Focus;
- Shadow;
- Highlight;
- Additional.

Composite controls retain genuine domains such as Indicator, Track, Thumb, Fill, Arrow, Popup, Header, Body, Rows, Badge, Drag and Tab rather than being flattened into generic Surface/Ink buckets.

`UiFill` surfaces use the shared FillRecipe editor (Solid / None / gradient). Resource-backed `StyledSkin` / image fields remain deliberately deferred until Theme adapters have the real Designer document-resource resolver; do not fake raw filesystem paths. The eventual user-facing skin label is `Skin (Nine Slice)`.

## PROPERTYEDITOR ALIGNMENT

Normal Designer and Theme Studio continue to project the same metadata into the production PropertyEditor.

Locked behavior:

- bounded NumericInt/NumericDouble keep the numeric field + slider-toggle affordance;
- a focused numeric editor owns mouse-wheel increments rather than scrolling the outer PropertyEditor;
- colour rows retain picker/dropdown, palette DnD and clipboard hex support;
- Left/Right/Top/Bottom choices retain the Cardinal4 matrix selector;
- icon fields retain the shared icon chooser;
- curve fields retain point-curve and Bézier editors;
- authored/inherited state is independent from the editor affordance;
- Reset returns to inherited; Undo restores authored state;
- Light/Dark and semantic roles remain isolated.

The previous broad `PropertyEditorTests` 70/2 result was a stale test-coordinate problem, not a contract change: the mouse override tests used an absolute Y that no longer pointed at the property row after filter/group geometry evolved. `527859915...` computes the live row/override locations from PropertyEditor style geometry. The test still requires both override-circle and inherited-row-body activation semantics.

## COMPLETED API/CATALOG/ADAPTER COVERAGE

### Button family

`UiButton`, `UiSplitButton` and `UiToolButton` use the fuller common Style vocabulary, real FillRecipe faces, typography, content margin, focus, shadow/highlight and their control-specific fields while preserving established serialized IDs.

### Check / Radio / Toggle

- CheckBox: body + Indicator palettes/metrics, typography, alignment, indicator geometry, mark thickness and marker render mode.
- RadioButton: body + Indicator palettes/metrics, typography and indicator geometry.
- Toggle: Track / Thumb style domains and control-specific geometry/appearance.

Resource-backed checked/tri-state/glyph images remain deferred to the real resource resolver.

### Progress / Slider / ScrollBar

- ProgressBar: separate Track and Fill palette/metrics including the previously missing Track and Fill radii, plus content/focus/shadow/highlight/typography where owned.
- Slider: separate Track and Thumb palette/metrics, ticks, geometry, thumb ring/additional fields.
- ScrollBar: Track, Thumb and Arrow palettes/metrics, arrows, thumb length, expansion/fade, grip and insets.

### Containers

- Panel: full palette/metrics, transparent mode and stable existing frame recipe IDs.
- ScrollPanel: full panel surface style.
- GroupPanel: common panel style plus Header ink/typography/layout/insets/spacing, separator/header-band settings and Body inset.

### Tab

Tab now has complete Theme coverage for:

- Body palette/metrics;
- Tab-item palette/metrics;
- tab typography;
- tab extent, spacing and body/content gaps;
- tab padding and strip inset;
- icon size/side and affordance gap;
- minimum tab length;
- indicator colour/thickness/span;
- active frame width;
- open-corner radius;
- active-tab-uses-body-face;
- Classic / Underline / Segmented / Rail / Document visual families.

`UiTab` stores the visual family as runtime instance state as well as Style metadata. `UiDesignerTabThemeRuntimeAdapter` therefore applies `UiTab::SetVisual(...)` during preview and emits it during code generation; generated custom-style setup resolves from the same authored visual family rather than silently seeding from Classic.

### Existing rich adapters retained

Label, edit family, Dropdown, List, Tree and Accordion keep their richer existing adapters. Theme Studio selectable samples use the same catalog adapters as normal Designer; unsupported/passive references are not made falsely selectable.

## THEME STUDIO PREVIEW PRESENTATION

Sample-only content/layout remains separate from durable theme recipes in `UiDesignerThemeSnapshot::studio_preview`.

Where the normal Designer catalog provides them, Theme Studio reuses the exact `UiDesignerPropertySpec` + `UiDesignerPreviewFactory` path for icon chooser/render mode, icon side, icon size/dimensions, scale-to-content, content gap and alignment.

Style-owned insets are represented by the real theme `Content Margin` / control-specific inset fields rather than creating a second competing Theme Studio value. Preview staging remains separate from `style_overrides` and is not emitted as runtime theme code.

## DETERMINISTIC GATES

### upp_Ui

- `Utilities/PropertyEditorV1RunTests` — `Fails: 0`.
- `Utilities/PropertyEditorSemanticRunTests` — `failed=0`.
- `Utilities/PropertyEditorTests` — `Fails: 0`; includes keyboard/mouse inherited-override behavior and the geometry-stable mouse regression.

### upp_uidesigner

- `tests/ThemeAdapterCoverageTest` — `THEME_ADAPTER_COVERAGE_SUMMARY ... failed=0`.
- `tests/ThemeBuilderContractTest` — `THEME_BUILDER_CONTRACT_SUMMARY ... failed=0`.
- `tests/ThemeDocumentTest` — `THEME_DOCUMENT_SUMMARY ... failed=0`.
- `tests/PreviewLayoutRegressionTest` — `PREVIEW_LAYOUT_REGRESSION_SUMMARY ... failed=0`.
- Existing focused adapter suites remain complementary: Label, List/Edit, Dropdown/Accordion.

`ThemeAdapterCoverageTest` verifies every Theme Studio selectable type resolves a supporting adapter, every exposed adapter field is owned, FillRecipe survives projection, Progress/Slider/ScrollBar radii exist with numeric slider affordances, container and Tab coverage is present, Tab Cardinal4/codegen semantics hold, and unresolved fake Skin fields are absent.

## VISUAL ACCEPTANCE FOCUS

On Debug UiDesigner:

1. Reproduce the old Button crash path: select Button, open Layout, toggle Scale icon to content repeatedly. It must not crash and the change must commit normally.
2. Theme Studio Identity must show Control / Type / Appearance / Role / Scope.
3. Select Label, Button, CheckBox, RadioButton, Toggle, ProgressBar, Slider, ScrollBar, Panel, GroupPanel, ScrollPanel and Tab; their Appearance groups must match their real API domains and visibly update the sample.
4. ProgressBar Track/Fill radius, Slider Track/Thumb radius and ScrollBar Track/Thumb/Arrow radius must be independently editable.
5. Tab Classic / Underline / Segmented / Rail / Document must visibly switch the runtime sample; Body, Tab and Indicator settings must update independently.
6. Numeric slider toggle + focused wheel, colour picker, Cardinal4, icon chooser, Reset and Undo must behave like the normal Designer PropertyEditor.
7. Verify Light/Dark and roles remain independent.

## BRANCH HYGIENE

Do not create another work branch for this tranche.

At the latest branch audit the following remote branches were fully behind `main` (`ahead_by=0`) and are cleanup candidates after successful validation:

`upp_uidesigner`:
- `work/label-fillrecipe-parity`
- `work/label-reference-audit-closure`
- `work/label-registry-switch`
- `work/model-api-preview-migration`

`upp_Ui`:
- `agent/uibutton-interaction-hardening`
- `temp-demo-button-do-not-use`

**Do not delete `upp_Ui/agent/uigraph-interaction-reconcile`.** At the latest audit it is diverged from `main` and contains one unique UiNodeGraph interaction commit. It must be reconciled separately before branch removal.

Gary should re-verify containment immediately before deleting any cleanup candidate.

## BOUNDARIES AFTER ACCEPTANCE

Still not complete and must not be claimed complete:

- standalone Theme JSON import/reuse UI;
- a universal runtime bridge that makes arbitrary Theme Studio recipes replace every global `upp_Ui` resolver in all generated applications;
- document-resource-backed Theme skin/glyph editing.

Those are separate bounded follow-ups after this API-parity tranche is accepted.
