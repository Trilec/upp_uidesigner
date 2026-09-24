# TitleCard lines and mixed Grid sizing

TitleCard's Role now remains effective without requiring a local Theme override,
both in Preview and generated code. Explicit local overrides still take priority.

Both line-length menus expose None, Small, Medium and Large. Small is the existing
short 40-DPI-unit accent. Medium measures the complete title string using its
current font; use it for a title-width underline. Large uses the available heading
space. With a hosted content-cell control, horizontal lines stop before its lane,
leaving the configured Content-cell gap. Card-line alignment follows text alignment.
Vertical Medium card dividers use the text-block height.

The Title line sits below the heading; the Card line decorates the selected card
edge. Enable the corresponding Show line setting and choose a non-None length.

For a one-column, three-row form:

1. Set the Grid width and height modes to Expand.
2. Set the TitleCard width to Expand and height to Fit (or a deliberate Fixed height).
3. Set the middle Panel width and height to Expand.
4. Set the bottom BoxLayout height to Fit, direction H, and its two buttons to Fit.

The heading and button row retain their natural measured heights; the Panel row
receives the remaining height. If the TitleCard is also Expand vertically, its row
shares that extra height. Fit on the Grid itself measures descendants instead of
compressing them against a zero-height constraint. Stored Fixed dimensions are
inactive while that axis is Fit or Expand. These rules also apply after export.

Select a covered Grid in the hierarchy. Repeated ordinary clicks at the same canvas
point cycle from the frontmost child through its containing layouts; controls do
not need to be moved out of the Grid to select it.

Regression evidence is in `TitleGridRegressionTest`: real GUI lifecycle, rendered
line pixels, live Preview roles/sizing, covered-parent selection and emitted code.

Follow-up: changing a nested Box direction rebuilds that control. Removing its
managed item shifts sibling indices, so preview now updates the surviving
indices before reattaching the rebuilt control. The regression inserts the Box
before the Panel and TitleCard (the reported hierarchy order), then switches
V/H repeatedly and checks that all three row rectangles remain disjoint.
