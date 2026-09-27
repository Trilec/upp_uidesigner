# v2.0.4 control drawing repair

- Fixed the shared cached antialiased circle painter: U++ Painter::Ellipse takes a centre and radii, not a rectangle origin and dimensions. Range-slider endpoint markers and adjustable-bound handles now draw complete circles. Added raster coverage regression checks.
- Slider and range-slider track endpoints reserve half the themed thumb extent, including large custom thumbs, in both orientations.
- DateTime uses the same dropdown arrow artwork and default size as UiDropdown, preserving its aspect ratio. SplitButton's default arrow size is reduced to the same baseline; explicit overrides remain available.
- Sidebar action pairs are mirrored, with Collapse on the outer edge. Collapsed rails include padding and sufficient width for both buttons.
- Designer hides interaction decorations while an embedded scrollbar owns mouse capture, then refreshes geometry and restores them on release. The capture watcher is cancelled on destruction.

Validation: AssistantDesignerTests includes caret, circle raster and slider endpoint checks; native Designer inspection covers the collapsed rails and gallery controls. Latest executable: `bin/UiDesigner.exe`.
