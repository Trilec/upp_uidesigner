# Layout decisions

Start outside in: stable regions, expansion, then local flows.

| Situation | Native structure |
| --- | --- |
| Stable header/body/footer | One-column, three-row Grid; Fit/Expand/Fit children |
| Shared row/column alignment | Grid with explicit cells |
| Simple vertical stack | Box direction V |
| Toolbar/actions | Box direction H |
| Items continuing onto another row | H Box with wrap Flow |
| Repeated aligned wrapping slots | H Box with wrap Snap, after inspecting schema |
| Workbench sidebars/canvas | Outer header/body/footer Grid; nested three-column middle Grid |

Grid does not provide flow wrapping or automatic responsive breakpoints. Boxes
are preferable for independent sequences. Do not create a grid per label just to
position text. A persistent shell may justify Grid even with a single column.

Sizing is per axis. Expand consumes remaining space; Fit measures content; Fixed
is for a deliberate size requirement. Set the shell to Expand and usually the
main body to Expand, with heading and footer height Fit. A Fit outer container
cannot also be expected to fill the canvas. Respect minimum sizes: Box defaults
may impose a taller footer; set min_height deliberately when a compact row is wanted.

For right-aligned action buttons: horizontal Expand-width/Fit-height Box, first
a Spacer with h_sizing Fill, then Fit buttons. A Fill Spacer between two groups
separates left/right actions. Wrapping may move groups to another line; do not
promise a spacer prevents wrapping. Use consistent gap/inset, typically 8/16,
but adapt to the reference and density requested.

UiLabel is appropriate for a plain heading. UiTitleCard is a richer title, icon,
subtitle/supporting information and optional hosted content. It is not a generic
mandatory wrapper. Set its `title` for the heading; a Label inside its content
slot is not its title. Its slot takes one control: host one Box to contain several.
UiPanel supplies a surface and content region, not text. Use actual labels for text.

When transcribing an image/HTML: map major proportions to flexible layout intent,
not a page full of absolute x/y coordinates. Keep text editable. Infer hidden
structure conservatively; use supported colours/fonts/icons. State approximations.
Test at the reference size and a narrower/wider size when a preview is available.
