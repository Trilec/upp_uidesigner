# ACTIVE WORK

Remote GitHub is authoritative. Never force-update `main`.

BASE: `155e51eb696537f8ac6a8a3af1629d2278513f66` — Label FillRecipe reference checkpoint before the final model-access migration.

TASK: `UIDESIGNER-LABEL-REFERENCE-ACCEPTANCE`.

TOUCHED:

- `UiDesigner/Theme/UiDesignerLabelThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerThemeAdapter.cpp`
- `UiDesigner/Theme/UiDesignerThemeAdapter.h`
- `UiDesigner/Theme/Theme.upp`
- `tests/LabelThemeAdapterTest/LabelThemeAdapterTest.upp`
- `tests/LabelThemeAdapterTest/main.cpp`
- `UiDesigner/Preview/UiDesignerPreview.cpp`
- `tests/Tests/main.cpp`
- `docs/THEME_OVERRIDE_COVERAGE.md`
- `docs/ACTIVE_WORK.md`
- cross-repository reference: `Trilec/upp_Ui` `8a016656651fb929c08ac1c2f801a6b8c2f2ab77`.

STATUS: **LABEL REFERENCE ACCEPTED — INTERACTIVE QUADGRADIENT RESELECTION SMOKE DEFERRED.**

PUBLISHED:

- `155e51eb696537f8ac6a8a3af1629d2278513f66` — dedicated Label adapter, FillRecipe parity, focused test and coverage note.
- `59cb685219dc94bcad8d8cba82d1acc7528c6836` — migrated remaining Designer preview/test callers from retired `GetInternalModel()` to canonical `Model()`.

SOURCE REVIEW:

- `UiLabel` routes to the dedicated `label` theme adapter rather than the generic basic adapter.
- Label groups match the shared reference convention for all non-resource fields: General, Face, Frame, Ink, Icon, Typography, Content Margin, Focus, Shadow, Highlight.
- Face Normal/Hot/Pressed/Disabled use `PropertyEditorKind::FillRecipe`.
- Solid and QuadGradient recipes share the production FillRecipe schema; preview rendering converts the recipe to `UiFill`, while field resolution and generated C++ retain the authored recipe so gradient colours, tile size and blur are not lost.
- Standard role with no authored overrides clears custom style and remains attached to theme resolution.
- Designer Skin remains intentionally deferred until the theme-adapter preview contract can resolve `UiDesignerDocument::resources`; do not add raw-path image editing.
- The `59cb685...` diff is mechanical only: four production preview accesses and the corresponding Tree/List test assertions use `Model()`; no ownership or test expectations changed.
- Gary's repository grep after that migration found no remaining `GetInternalModel()` calls.

VALIDATION:

- `tests/LabelThemeAdapterTest` Debug: `checks=242 failed=0`.
- `tests/LabelThemeAdapterTest` Release: `checks=242 failed=0`.
- UiDesigner Debug build: PASS, zero warnings/errors.
- UiDesigner Release build: PASS, zero warnings/errors.
- UiDesigner Release launch: PASS; remained responsive.
- `git diff --check`: PASS.
- No `Face/Skin` group in Designer: confirmed.
- Full add/select UiLabel and QuadGradient refresh/reselection interaction remains manually unverified because the custom U++ controls expose no Windows UI Automation descendants. This is a deferred interactive smoke, not a source/build blocker.

NEXT:

1. Continue the shared override-layout rollout with UiList and UiBaseEdit.
2. Then normalize the composite UiDropdown and UiAccordion domains without flattening Popup/Header/Body semantics.
3. When a convenient interactive Windows session is available, perform the deferred UiLabel QuadGradient reselection smoke; do not block the next implementation slice on UI Automation limitations.
4. Keep `docs/ACTIVE_WORK.md` current with each coherent published checkpoint.
