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
