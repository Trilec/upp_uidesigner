# Header and palette interaction repair

The header must be measured after its final brand and dropdown recipes are
applied. Relaying out only the header's children retained the old outer height
and offset when changing presets, most visibly with Pill. The final chrome
callback now lays out the owning window, without preset-specific offsets.

Catalog and Inspector filters have four pixels of top clearance. The shared
PropertyEditor filter lane grows to preserve its input height. Catalog has a
square, theme-coloured boundary consistent with PropertyEditor.

Palette swatches capture the pointer until drag initiation, then release capture
before entering native drag/drop. This supports fast drags out of small swatches.
The shared PropertyEditor uses PasteClip's accepted state during drag-over:
AcceptText's return value indicates an actual paste, not format acceptance.

Drop a palette colour onto an Inspector colour or fill property to author it.
Inherited overrides activate through the existing override callback. A fill drop
sets a solid fill; it replaces the fill mode rather than editing one gradient
corner. Read-only and disabled properties remain protected.

Drop onto a Theme Studio sample to choose a supported property and state from
its Inspector schema. Group panels delegate drops over their children to those
samples. The popup opens after the native drag session ends. The selected role
and appearance remain the target, edits use ThemeDocument::Commit, and Theme
Undo applies. This changes a role recipe, not just the sample instance.

Validation (2026-09-27):

- Release application build passed.
- PropertyEditorTests: 133 checks, zero failures.
- AssistantDesignerTests: 200 checks, zero failures.
- Native visual checks: Minimal, Pill Light and Pill Dark header centring;
  filter clearance and catalog frame.
- Native interactions: palette to button/frame and button/solid background;
  palette to Inspector colour and solid-fill fields; Undo restores the previous
  background. These used a separate disposable build, not the user's project.

The wider preset contrast/appearance audit remains separate from this repair.
