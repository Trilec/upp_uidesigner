# UiDesigner ThemeCore

Headless theme document, preview state, validation, serialization and undo/redo. It contains no
Ctrl or application-shell dependency.

`UiDesignerThemeSnapshot` owns the six-colour Light/Dark palettes, semantic role assignments,
global theme metrics and durable Theme Studio style recipes. Style recipes are keyed by
appearance, control/panel domain, catalog type and semantic role; their field ids are the real
Theme adapter fields rather than a separate Theme Studio schema.

Standalone Theme JSON uses schema 3. Schemas 1 and 2 remain loadable. Style recipes may contain
typed U++ Values such as `Color`, so serialization recursively converts those values to an
explicit JSON representation and restores them on load. Project save and Theme JSON export use
this same ThemeDocument state.


## Runtime/export ownership

The generated-application contract deliberately separates ThemeDocument fields:

- **Runtime appearance:** `preset`, `mode`, and durable `style_overrides`. Export compiles these into the generated component. Preset/mode is applied before controls build; style recipes inherit per appearance/domain/type/role.
- **Instance authority:** active control-node Theme overrides live in the Designer document and apply after the inherited ThemeDocument recipe. Disabled/saved local overrides inherit the ThemeDocument recipe again.
- **Theme Studio samples only:** `studio_preview` is never projected into Designer document controls or generated code.
- **Authoring metadata without a production-control bridge:** the six-colour palettes, role-slot assignments, legacy `accent`, and global spacing/radius/shadow metrics remain serialized Theme Studio metadata. They currently influence Designer/Studio presentation but are not synthesized into production control recipes.

The last category is deliberately explicit: generated applications reproduce the actual Designer control Preview rather than inventing a palette-to-style mapping that Preview itself does not use.
