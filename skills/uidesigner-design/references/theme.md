# Roles and theme scope

Standard: ordinary controls. Subtle: secondary emphasis. Accent: primary emphasis.
Alert: warning/destructive emphasis. Alert is not the default role for every Cancel
button; cancellation is often ordinary. Use the exact role choices in each schema.
Panel and control roles can be previewed independently in Theme Studio.

Theme defaults and Theme recipes are inherited; active per-node overrides win.
Changing a document's control role does not author a whole system theme. Preserve
existing recipes and overrides unless asked to change them. Avoid per-control
colour duplication for a system-wide theme request.

For a precise local colour use a registered theme-field ID and the native encoding:
`{"$type":"Color","r":240,"g":190,"b":40}` with integer channels 0..255.
Do not put CSS strings in Color fields. Font, fill, frame and interaction-state
fields vary by control; consult its theme_fields. Preserve hover, selected,
disabled and focus behaviour. Never claim measured contrast without calculating it.

The six palette slots seed the Theme Studio authoring generator: background,
surface, border, foreground, Accent, Alert, separately for Light and Dark. Existing
themes containing only palette metadata do not automatically restyle every native
control. Full styling is represented by explicit supported recipe values.
The embedded assistant's prepare_theme_design tool generates those values and
previews a candidate for human Keep. The optional generated ownership map lets
later palette changes preserve manual refinements; it does not replace styles.
This portable design skill's primary output is an editable design document; do
not invent a new theme token format or treat tool arguments as a saved theme file.

A visual style also needs typography hierarchy, frame/rule weights and shadow treatment. Match those from the reference rather than changing only an accent colour. Use installed fonts when known; state substitutions. Hard offset shadows and square frames can express neubrutalism, while textures and decorative outlined lettering may require real assets. Do not promise those effects from palette values alone.

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

Designer Inspector preview/cancel, commit/reset, document JSON and Theme Studio recipes
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
