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


## Designer Preview and generated applications

Theme Studio style recipes now cross the real application boundary. The Designer canvas inherits the recipe selected by appearance, control/panel domain, catalog type and semantic role, then layers active per-control overrides on top.

Complete-package and component export use the same recipe resolver. Export flattens inherited recipes into generated C++ style setup while preserving the original Designer document in `design.json`. `studio_preview` remains sample-only.

Generated C++ applies the compiled `UiTheme` preset/mode before any control style is resolved. `theme.json` is an optional source/authoring artifact; the executable does not load it and therefore has no process-working-directory dependency.

## Frame Accent

Frame Accent adds a coloured edge inside the existing surface frame. It follows
its current rounded corners and does not alter content margins or control size.
It is independent of frame enabled/width, highlights and shadows. Selecting no
edges (the default), setting thickness to zero, or alpha to zero hides it.

Panel, GroupPanel and ScrollPanel expose **Frame Accent** in their Theme fields:
`frame_accent_top`, `frame_accent_bottom`, `frame_accent_left` and
`frame_accent_right` are independent booleans. `frame_accent_thickness` is an
integer from 0 to 60, `frame_accent_alpha` is opacity from 0 to 255, and
`frame_accent_color` is a native Color. A null colour follows the current
interaction state's normal frame colour. Omitting an override preserves theme
inheritance; an explicit false can switch off one inherited edge.

For example, these are node `theme_overrides`, not ordinary `properties`:

```json
{
  "frame_accent_top": true,
  "frame_accent_bottom": false,
  "frame_accent_thickness": 3,
  "frame_accent_color": {"$type":"Color","r":65,"g":116,"b":156},
  "frame_accent_alpha": 220
}
```

Other controls reuse the same fields for rectangular styled surfaces. Composite
controls retain their established surface prefixes, such as
`thumb_frame_accent_top` for a slider thumb or `tab_frame_accent_top` for a tab.
Always read that control's current `theme_fields` schema rather than assuming a
prefix or adding an unsupported field to a specialised ring surface.

Inspector preview/cancel, commit/reset, document JSON and Theme Studio recipes
all use the same registered fields. Generated C++ writes only authored changes
into `StyledMetrics::frame_accent`; it does not introduce a drawing override or
an extra child control. Plain C++ callers can use, for example:

```cpp
panel.SetFrameAccent(StyledFrameAccent::Top | StyledFrameAccent::Right,
                     DPI(3), Color(65, 116, 156), 220);
// Return the per-control accent settings to their defaults:
panel.ClearFrameAccent();
```

Designer currently exposes Classic CheckBox and RadioButton visuals, so only
`indicator_frame_accent_*` is offered for those controls; their unpainted outer
accent is omitted. Tab-cap `tab_frame_accent_*` fields appear in the canvas
Inspector only when the authored `visual` is `Segmented`. Switching visuals hides
these fields without deleting their saved values. Theme Studio's fixed Classic
tab sample hides them as well. The schema advertises this visibility condition
and its explanation to document-building tools. `UiRangeSlider` and
`UiRangeSliderEdit` currently have no Designer Theme adapter, and RangeSegments
has no Designer entry; none advertises unsupported thumb accents.
