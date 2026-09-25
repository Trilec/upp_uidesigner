# Role default consistency repair — 2026-09-25

## Ownership

Standard, Subtle, Accent and Alert remain the existing four roles. A resolved
role provides a baseline style; an authored Theme recipe overrides its fields;
an active per-control override takes precedence over that recipe. Clearing an
override restores inheritance. Controls retain their complete custom-style APIs.

Theme Studio and future AI palette tools should produce ordinary, editable Theme
recipes through the existing document/history service. A minor AI refinement
should change only requested fields. Saving keeps authored choices; baseline
generation must not overwrite them implicitly.

## Repair

- List and Tree adapters now use the selected role for Inspector values, preview
  and generated C++. Previously those paths resolved Standard regardless of role.
- Toggle's adapter likewise honours the role instead of always resolving Accent.
- Tree shares collection colour defaults with List while retaining tree geometry.
- Accordion now has one reusable `UiTheme::ResolveAccordion(role)` baseline used
  by the control, Designer and export. Its header was previously fixed to Accent
  in duplicated implementations.
- Tab visual restoration preserves resolved colours. The Underline helper had
  replaced them with light/default values. Active Accent/Alert text and the
  separate active-frame colour now follow the role across visual families.
- Non-Minimal/Pill presets retain their metrics while receiving the selected
  nonstandard role colours for List, Tab, Toggle and TitleCard.
- Alert ProgressBar uses a red-family track and frame instead of a blue/slate
  track behind its red fill.

## Scope

No project schema, role count, control override surface, history ownership or
Apply workflow changes. Standard Tree selection, Standard Accordion header and
an explicitly Standard Designer Toggle now use neutral defaults; explicitly
authored styles remain authoritative. A standalone Toggle's no-argument theme
resolver keeps its existing Accent default.

This repair does not add global palette generation, editable chat swatches or a
new role-palette model. The master six-colour palettes are still stored in Theme
documents but are not yet inputs to the shared runtime theme resolver. Designing
that bridge is a separate step; it must compile ordinary editable style values
without replacing the existing control style model.

## Validation

Regression coverage exercises seven presets, both modes and all four roles:
preview/Inspector agreement, role-only export, explicit overrides, changing roles
with an override, and returning to inheritance after reset. Saved Theme recipe
precedence for List, Tree and Accordion is covered by ExportedThemeContractTest.

Validation evidence and publication references are recorded in ACTIVE_WORK.md.
