# UiDesigner Theme

Theme Studio is the visual authoring surface for the session-owned `UiDesignerThemeDocument`.
It is not a second Designer document and it does not maintain a parallel control-property
schema.

The toolbar exposes separate Control and Panel semantic roles plus compact six-colour Light
and Dark palettes. Clicking a palette swatch opens the production `UiColorPicker`; persistent
swatches can also be dragged onto PropertyEditor colour rows.

Representative controls backed by a real catalog Theme adapter are selectable. Selecting one
projects that control's registered `UiDesignerThemeOverrideSpec` fields into the Theme Studio
PropertyEditor. Inherited values are resolved through the real adapter, while previews,
commits, resets and undo/redo remain owned by `UiDesignerThemeDocument`.

Committed Theme Studio edits are stored as durable per-appearance/per-role/per-type style
recipes. The gallery applies those recipes back through the same adapters so edits are visible
on the selected representative control while authoring.

Controls without a stable Designer Theme adapter remain visual context only; do not invent a
Theme Studio-only property model for them. Add or extend the real adapter first if such a
control is to become editable here.
