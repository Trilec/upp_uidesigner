# AI Theme proposals and named files

The application service now prepares a Light/Dark role baseline from six explicit
colour seeds per appearance: background, surface, border, foreground, Accent,
Alert. The authoring policy generates supported adapter colours and corner/frame
metrics for Standard, Subtle, Accent and Alert. It produces ordinary editable
Theme recipes; no reusable control API or runtime role resolver was changed.

Generated recipes include tinted surfaces, interaction states, frames, text,
icons, tracks and supported nested surfaces. Categorical chart-series colours
retain their existing semantics. This is a baseline, not a claim of perfect
contrast or complete coverage of every custom painting path.

Theme JSON remains `upp-ui-theme-designer`, schema 3. Its optional `generated`
map records the last generated values, while `styles` remains the effective
explicit recipe source consumed by preview/export. Manual field edits relinquish
generation ownership for those fields. Later palette changes update owned fields
and preserve explicit refinements. Older readers can still display the explicit
styles; they do not understand generation ownership. Local document overrides
continue to win. Existing themes without `generated` retain their behavior.

## Review lifecycle

- `prepare_theme_design` is a bounded native assistant operation. It accepts two
  six-colour arrays, radius, frame width and an explicit replace-authored flag.
  Optional style fields add installed body/heading fonts, independent size/bold,
  hard outer shadows and rule widths. Palette changes retain these style choices.
  It stages a temporary candidate and opens Theme Studio. It cannot Keep or Save.
- The dropdown shows **Proposal — unsaved**. The proposal card has **Keep theme**,
  **Compare**, **Refine**, and **Dismiss**. Compare/dropdown Current switches the
  effective preview without changing durable state. Inspector edits affect the
  visible candidate. Selection does not clear it.
- `inspect_theme_control` resolves catalogue type/display names and returns at
  most 32 matching style fields plus exact targets. `inspect_theme` reads palette
  metadata or one recipe. `prepare_theme` refines only requested fields in the
  visible candidate. A later read-only failure does not discard it.
- Human Keep rechecks captured document/Theme revisions and commits one Theme
  history entry. Duplicate Apply returns its receipt. Discard/clear removes the
  candidate. Durable Undo/Redo requires keeping or discarding a pending candidate.
  Model conversations and candidates are not serialized into project output.
- Save Theme saves the kept working copy to its known file; first save uses Save
  As. Successful files enter **My themes**. The dropdown shows the current name,
  base preset and `*` for edits since the standalone-file checkpoint. Project
  Save and Theme Save have independent checkpoints. Switching a modified theme
  offers Save/Discard/Cancel. Built-in starting points create an unnamed working
  copy and do not overwrite the previous named file.

## Validation and boundaries

Deterministic tests cover no durable mutation during preview, bounded discovery,
selection stability, Compare, targeted refinement, generated-field ownership,
later read failure, Keep, exact one-step Undo/Redo and standalone save/reload.
Live acceptance uses the existing AssistantLiveTest executable with argument
`theme`, using the configured provider and normal allowlisted tools.

The first live refinement failed through unnecessary discovery and bad targeting;
the compact theme lookup and concrete target guidance repaired that sequence.
The bounded turn limits remain unchanged. Native UI verification and final build
identities are recorded in ACTIVE_WORK.md.

The broader workflow proposal remains in THEME_WORKFLOW_AND_CLEANUP.md. Automatic
crash-recovery drafts, a persistent **Use for Designer** appearance preference and
a dedicated matrix of editable role swatches are not implemented here. Theme
Studio currently previews recipes on its samples and document preview; the
application's custom-painted chrome still follows its existing preset styling.

Native gallery testing also exposed a legacy slider/scrollbar alias collision: `ink_normal` is a thumb FillRecipe, not track ink. Apply, resolve and export now handle the alias before generic palette fields. Proposal staging rolls back if rendering fails. Live JSON numeric values are validated by integral value/range, not their internal C++ storage type. Solid FillRecipe refinements are supported; other structured fill modes remain unsupported by assistant editing.

Generated themes preview their background seed behind the gallery. Gallery sample placement uses measured group headers so larger theme fonts do not overlap controls. The hard-shadow repair is in shared Ui commit `d5dc1beb3d36559caca0eb4d15db202b4162b879`; its transparent-interior/visible-offset pixel regressions pass.

Visual acceptance found remaining coverage boundaries: the composite table and Tree background/frame retain some preset styling, chart series retain their own palette, and light ink on bright progress fill in Dark needs further contrast-policy work. Sample body-control heights can clip extreme fonts. Do not describe this baseline as complete visual parity or automatic contrast compliance.
