# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`. Refresh the current remote HEAD before implementation or publication and preserve unrelated concurrent advances.

## CURRENT SUPERVISORY STATE — 2026-08-21

STATUS: **PROPERTYEDITOR / LABEL DEMO / DESIGNER / THEME STUDIO INTERACTION UNIFICATION SOURCE COMPLETE; CURRENT WINDOWS + VISUAL VALIDATION PENDING.**

Authoritative branch: `main`.

Current source/test checkpoint before this documentation update:

- `8ef7417b54b2309b89cb8d9d6d1cc6afa55994f2` — deterministic Theme Builder contract coverage for shared Designer metadata and separate preview-presentation state;
- `b1a061319f60e36f7bdeb1e911ff791dc8a2a450` — Theme Studio preview properties reuse normal Designer catalog metadata and Preview adapters;
- `9697d625f561dab3eb662f854fbdb36b526d0a0e` — Theme Studio preview-presentation persistence;
- `0aefb699cbd7038aafbb54fae12dd325f3fdeb18` — normal Designer and Theme override projections preserve rich PropertyEditor metadata.

Current `upp_Ui/main` dependency checkpoint:

- `00502ca0b779187314d72d51fc11fbb16f5ebad2` — Cardinal4 PropertyEditor matrices preserve canonical caller values such as `Left / Right / Top / Bottom`;
- parent `9d89914ef3068bd88b53697a3552051d174efe0d` — focused numeric PropertyEditor editing retains mouse-wheel numeric adjustment instead of scrolling the whole PropertyEditor.

Important retained Preview checkpoint:

- `0757a06f640f5353944874390fd8e157b39f0a10` — Grid preview reconstruction and nested transparent-layout selection fix.

Previous Windows validation was accepted for the older pair `upp_Ui bcd1278462bb...` / `upp_uidesigner b721c07e28...`, including ThemeBuilderContractTest, ThemeDocumentTest, PreviewLayoutRegressionTest and UiDesigner Debug/Release. That evidence does **not** validate the newer source above.

## SUPERVISORY OWNERSHIP

The supervisor owns architecture, diagnosis, substantive source changes, review and publication.

Gary is the Windows validator/helper: fetch the exact requested `main`, build/run focused gates, report exact diagnostics, and make only genuinely mechanical build corrections when appropriate. Substantive design/source failures return to the supervisor.

## UNIFIED PROPERTYEDITOR CONTRACT

The Label demo, normal Designer Inspector and Theme Studio use the same PropertyEditor interaction vocabulary rather than separate approximations.

Current contract:

- bounded `NumericInt` / `NumericDouble` values retain the compact numeric field and slider-toggle affordance;
- clicking the slider action changes editing mode without changing the property itself;
- a focused numeric editor owns mouse-wheel increments/decrements; the outer PropertyEditor scrolls only when a focused numeric editor is not handling the wheel;
- four-way `Left / Right / Top / Bottom` choices project through the shared Cardinal4 `UiMatrixSelector` editor while preserving the caller's canonical values;
- colour properties retain their normal colour editor/dropdown/alternate-editor behavior;
- icon properties retain the shared icon chooser;
- editor affordances and authored/inherited state are independent concepts.

For Theme Studio style rows:

- inherited/authored state remains the style-state layer;
- Reset returns an authored field to its inherited adapter value;
- slider/matrix/colour/icon editor actions remain available independently of that state;
- Theme Studio must never replace editor affordances merely to show authored/inherited state.

The normal Designer projection and Theme override projection both go through the same `UiDesignerPropertySpec` / `UiDesignerThemeOverrideSpec` metadata finish path, so kind, bounds, step, slider toggle, custom editor, editor variant and choices stay aligned.

## LABEL DEMO / ICON PRESENTATION

The Label demo uses the shared PropertyEditor APIs:

- bounded numeric rows are created through `AddNumericInt`, which enables the slider toggle;
- Icon Side is a Cardinal4 matrix;
- Icon Width / Icon Height / Content Gap are bounded numeric edits;
- icon rendering supports `Auto`, `PreserveColor` and `MonoTint`;
- explicit icon sizing is applied through `UiLabel::SetIconSize`, and `UiLabel` treats a positive explicit size as the rendered icon size when scale-to-content is off.

No separate Label-demo-only editor mechanism is introduced. The shared PropertyEditor changes are consumed by the demo and Designer.

## THEME STUDIO PRODUCT CONTRACT

Theme Studio is a theme-focused editing workspace, not a static gallery and not a second Designer document.

Top toolbar:

- Controls / Containers preview modes;
- independent Panel Role (`Surface`, `Subtle`, `Strong`);
- Control Role (`Standard`, `Subtle`, `Accent`, `Alert`);
- Light and Dark each retain six persistent palette swatches;
- clicking a swatch opens the temporary production `UiColorPicker`; accepting commits the six-colour palette atomically;
- persistent swatches remain colour drag sources for PropertyEditor colour rows;
- no permanently embedded full colour picker belongs in the Theme Studio preview.

Selectable samples use real catalog Theme adapters. The right PropertyEditor is rebuilt from each selected type's real `UiDesignerThemeOverrideSpec`; inherited values resolve through the adapter for the selected semantic role, and committed edits remain in `UiDesignerThemeDocument` history.

## THEME STUDIO PREVIEW-PRESENTATION LAYER

Theme Studio has a deliberately separate sample-presentation layer for properties that help judge a style but are not themselves runtime theme recipe fields.

`UiDesignerThemeSnapshot::studio_preview` is keyed independently of appearance/semantic role:

```text
control|UiLabel
control|UiButton
panel|UiPanel
```

This prevents staging choices from becoming duplicated Light/Dark or role-specific style data.

For any selected control whose real catalog exposes them, Theme Studio can project shared Designer metadata for:

- icon chooser;
- icon render mode;
- icon side;
- icon width / height or scalar icon size;
- scale icon to content;
- content gap;
- horizontal / vertical alignment.

These appear under `Preview / Content` and `Preview / Layout` and are added by copying the real `UiDesignerPropertySpec`, so slider toggles, Cardinal4 matrices, choices and custom editors are exactly the normal Designer metadata rather than Theme Studio clones.

A Theme Studio preview icon defaults to a visible Widgets icon when the control's authored default is empty/None, with at least a 24px preview dimension where a bounded icon dimension exists. `MonoTint` is the preview default so the real themed Icon Ink fields can be judged visually. Reset returns preview staging to these projection fallbacks.

Applying preview-presentation values uses `UiDesignerPreviewFactory::Apply` with the selected control's real catalog spec. Theme Studio therefore shares the normal Designer runtime property application path.

Preview-presentation state is serialized separately from `style_overrides`; it must never pollute generated/runtime theme recipes.

## DURABLE STYLE RECIPE CONTRACT

`UiDesignerThemeSnapshot::style_overrides` remains keyed by:

```text
<Light|Dark>|<control|panel>|<type>|<role>
```

Example:

```text
Light|control|UiButton|Accent
```

Each style target contains only authored adapter fields. Reset removes the authored field and restores the inherited adapter value; an empty target recipe is removed.

Standalone Theme JSON remains schema 3. Schemas 1 and 2 remain accepted for migration. Typed U++ Values such as `Color`, including nested values, are recursively encoded/decoded rather than passed raw to `AsJSON`.

Project Save embeds `theme.Get().ToValue()`. Theme JSON export and generated-package theme metadata use the same authoritative ThemeDocument serialization. Dedicated standalone Theme JSON import/reuse UI remains a later bounded feature.

## FOCUSED TESTS

### `Utilities/PropertyEditorV1RunTests`

Must pass in Debug and Release:

```text
PropertyEditorV1RunTests: Checks: <n> Fails: 0
```

This is the focused shared-PropertyEditor regression gate for the numeric, rich-editor and scrolling lifecycle used by the Label demo, normal Designer and Theme Studio.

### `tests/ThemeBuilderContractTest`

Must build/link the complete Theme module and pass in Debug and Release:

```text
THEME_BUILDER_CONTRACT_SUMMARY checks=<n> failed=0
```

Current assertions include:

- atomic six-colour palette history and Light/Dark independence;
- durable selected-control style edits and transient style preview;
- valid schema-3 typed-value JSON round-trip;
- reset/undo and empty style-target cleanup;
- separate `studio_preview` commit, reset and JSON round-trip;
- preview-presentation state does not enter `style_overrides`;
- Label icon metadata is present in the real Designer catalog;
- bounded Designer numeric projection retains `show_slider_toggle`;
- `Icon Side` projects to the Cardinal4 custom editor while preserving `Left / Right / Top / Bottom` values;
- icon projection retains the shared icon chooser;
- Button Radius Theme override projection retains the same bounded numeric slider-toggle metadata;
- shared colour parsing and Theme Gallery / toolbar link visibility.

### `tests/ThemeDocumentTest`

Must pass in Debug and Release:

```text
THEME_DOCUMENT_SUMMARY checks=<n> failed=0
```

### `tests/PreviewLayoutRegressionTest`

Must pass in Debug and Release:

```text
PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=<n> failed=0
```

## MODEL / PROPERTYEDITOR CONTRACT

Retained architecture:

- `UiDesignerDocument` is durable authored UI state;
- `UiDesignerThemeDocument` is durable authored theme state;
- PropertyEditor models are projections, never parallel application state;
- transient previews remain transient;
- durable edits go through their owning document/command service;
- catalog Theme adapters remain authority for control-specific theme fields;
- Preview adapters remain authority for normal control presentation properties;
- Window remains presentation/wiring, not another state owner;
- Designer PropertyEditors retain the explicit 38% label/value divider and theme-aware styling.

## CURRENT WINDOWS VALIDATION GATE

Validate current final `main`, not an earlier ancestor.

1. Fetch exact `upp_Ui/main` and `upp_uidesigner/main`; report both SHAs.
2. `PropertyEditorV1RunTests` Debug + Release — `Fails: 0`.
3. `ThemeBuilderContractTest` Debug + Release — `failed=0`.
4. `ThemeDocumentTest` Debug + Release — `failed=0`.
5. `PreviewLayoutRegressionTest` Debug + Release — `failed=0`.
6. Build `UiLabelDemo` Debug.
7. Build `UiDesigner` Debug + Release.
8. Launch Debug UiDesigner, perform the smoke below, and leave it running for Curt.

Visual/interaction smoke:

- Label demo: bounded numeric values show the value-to-slider action; Icon Side opens the Cardinal4 selector; icon size/gap and Auto/PreserveColor/MonoTint visibly work; a focused numeric value changes with the mouse wheel without scrolling the full PropertyEditor.
- normal Designer: select a `UiLabel`; Icon Side uses Cardinal4, bounded icon dimensions have the same slider-toggle behavior, icon chooser/render mode remain available, and focused numeric mouse-wheel editing behaves the same way.
- Theme Studio: select `UiLabel` and `UiButton`; Theme rows such as Radius/Focus Alpha retain their numeric slider toggle; colour rows retain the colour editor; preview-only icon/content/layout rows appear when supported and use the same shared editors; a MonoTint icon visibly reflects the themed Icon Ink; Reset/inherited state remains independent of editor mode.
- Panel Role / Control Role and Light / Dark style targets remain independent.

If a substantive failure occurs, stop and report exact SHA/configuration/diagnostic/reproduction. Do not redesign the architecture during validation.

## KNOWN BOUNDARY / NEXT AFTER ACCEPTANCE

Theme Studio authors and exports durable per-type/per-role style recipes, but arbitrary Theme Studio recipes are not yet a replacement for every global `upp_Ui` resolver in normal generated/runtime applications. Do not claim that broader runtime semantic-palette bridge is complete.

After this slice is Windows/visual accepted, the next bounded work is dedicated Theme JSON import/reuse and then, if required, the shared `upp_Ui` runtime theme-consumption bridge.
