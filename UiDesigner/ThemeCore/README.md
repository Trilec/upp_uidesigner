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
