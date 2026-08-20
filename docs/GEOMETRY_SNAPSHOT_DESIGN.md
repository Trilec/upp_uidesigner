# UiDesigner Geometry Snapshot

`UiDesignerPreviewCanvas` owns the geometry snapshot in the Preview package. It is
session state only: it is never serialized and never passed to code generation.

After parent-first runtime layout completes, Preview builds a temporary snapshot and
publishes it as one complete replacement. Consumers receive only a const snapshot.
Relayout, document rebuild, and relevant property changes replace it; stale records are
therefore removed atomically.

All stored rectangles are in preview-canvas/document coordinates. Node `rect`, `body`,
`item_rects`, `gap_rects`, and `inset_rects` use that same space. The interaction overlay
performs one conversion by adding the preview canvas origin in workspace coordinates.

Records contain node and parent IDs, depth, paint order, selectable/drop-target flags,
outer and body rectangles, inset regions, and runtime item/cell and gap regions. Box and
Grid item rectangles are read from the runtime layout after it has assigned them; the
overlay does not divide Grid space into guessed equal cells.

Normal selection resolves the deepest selectable record in paint order, except that
transparent layout-owned interaction space is reserved first. Explicit Box/Grid inset or
gap regions select their owning layout, and a narrow perimeter rail keeps a zero-inset,
zero-gap layout directly selectable even when a stretched child covers its complete body.
When nested layout rails coincide, the deepest layout wins. Away from those layout-owned
regions, the normal deepest child remains the selection target.

Drop planning uses the same geometric snapshot, then climbs the model parents while the
drop service checks semantic compatibility. Thus an occupied Panel wins at its centre,
while exposed Box inset or gap space resolves to the Box.

The current scope is Window, Box Layout, Grid Layout, and Panel, with additional named
regions for several composite containers. Debug guides are Designer-only outlines and
region hints; they do not alter runtime painting or geometry.
