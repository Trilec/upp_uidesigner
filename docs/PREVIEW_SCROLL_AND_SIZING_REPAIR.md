# Preview scrolling and sizing repair — v2.0.3

- Shared collection theme defaults now use a strong blue selection with white
  text (red for Alert), including Minimal light and dark. Authored overrides
  retain precedence.
- UiList Fit measurement uses the current row count instead of a hard-coded
  four-row sample. Empty lists retain one usable row.
- Inspector previews of managed Box/Grid sizing use the pending value rather
  than only the saved document value. Fixed mode remains an exact allocation;
  Fit uses content measurement with the configured minimum/maximum.
- Preview layout preserves logical positioning (`SizePos`) when a host owns its
  child's bounds. Previously a geometry walk converted this into a fixed rect,
  causing scroll content to stop resizing and descendants to outgrow their cells.
- Editing forwards wheel input and scrollbar clicks to the real UiScrollPanel
  and UiScrollBar. Native capture handles thumb dragging. Scroll notifications
  refresh preview geometry after control mutations finish; destruction cancels
  queued work. Off-screen geometry is excluded from selection and drop targets.
- Shared UiScrollPanel also handles horizontal-only and Shift-wheel scrolling.

Validation: AssistantDesignerTests, 297 checks passing; includes the original
179-item assistant gallery, content bounds, transient minimum changes on lists,
trees and buttons, clipping, wheel routing and light/dark selection colours.
Native verification also exercises scrolling and selection in the Designer.

The initial incremental test build crashed in UiColorPicker's internal
ScrollPanel construction. A full rebuild removed stale objects after the shared
control's binary layout changed. The clean test build also passed under CDB.

Executable: `bin/UiDesigner.exe` (v2.0.3). Existing saved user designs are not
rewritten by these repairs.
