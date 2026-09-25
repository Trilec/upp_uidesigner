# Assistant design guidance and reference inputs

The compiled `UiDesigner/Assistant/Skills.h` guidance is supplied on demand by
`retrieve_skill`. Catalogue schemas remain the authority for supported fields.
The system prompt directs layout requests to this guidance before composition.

## Outside-in design

Identify stable shell regions first, choose the expanding focus, then work inward.
Grid suits explicit aligned rows/columns and persistent header/body/footer or
sidebar/canvas/inspector regions. Box suits one-dimensional groups and nested
sequences; horizontal Flow wrapping suits responsive toolbars/card collections.
Snap is ordered wrapping aligned to repeated slots. Vertical Box does not wrap.
Fit header/actions and Expand body is the usual dialog arrangement; make a
different choice when the request requires it. Ask only when expansion is ambiguous.

TitleCard is useful for title/icon/subtitle/copy and attached controls, not every
heading. A Label is sufficient for simple headings. Its one direct content child
can be a Box/Grid containing several controls. A horizontal Fill Spacer separates
left/right groups or pushes Fit OK/Cancel buttons to the right in an expanding row.
Wrapping groups need separate consideration: a spacer does not prevent wrapping.

The skill now includes a validated Label/Grid/body/actions example alongside the
rich TitleCard example and minimal Box example. No preset lookup is required.

## Preset coverage audit

The catalogue already covers Holy Grail, Magazine, SPA, Card Grid, Split Screen,
F Pattern, Header with Actions and Designer Workbench. These lean toward full
application/content shells. A separate new-document Dialog starter exists, but
was not an insertable catalogue preset. `DialogTemplate` now fills that gap:
three-row Grid, Fit Label, Expand Panel, Fit horizontal actions with Fill Spacer
and OK/Cancel. It is an ordinary editable composition and does not wire behavior.
Potential future patterns include labelled forms, master/detail and empty states;
they are not added merely to increase the preset count.

## What reference input supports now

- Open an existing Designer project/design through the normal Load flow, then
  ask the assistant to inspect/refine it. No automatic loading tool is exposed.
- Paste a short description or HTML source as text. The assistant can interpret
  textual layout intent; it cannot claim to have rendered or seen that HTML.
- Icon choices come from each control's schema. The full icon library and remote
  asset downloading are not exposed by the current assistant tools.
- Images are not attached to provider messages today. Selecting a vision-capable
  model alone does not add attachment support.

## Palette and style direction

The theme skill now describes semantic palette planning, Light/Dark choices,
Standard/Subtle/Accent/Alert emphasis, independent panel roles, local override
precedence and preserving interaction states. HTML/CSS text can supply colors,
variables and style intent without rendering. Brutalist direction is translated
into deliberate surfaces, strong borders, square corners and typography, rather
than pretending there is a universal style switch.

The current assistant can prepare individual registered Theme recipes. It cannot
commit a global palette-token change or an atomic multi-recipe style pack. The
skill explicitly states this boundary: palette discussion is broader than the
currently executable edits. Applying a recipe can stale another pending proposal;
subsequent recipes must be prepared against current state. A future full-system
palette action should reuse Theme transactions, with validated multi-target scope,
preview and one Theme Undo, rather than loop over independent local overrides.

## Catalogue sidebar repair

Scrolled catalogue rows previously painted into the fixed filter/heading lane:
painting and wheel calculations used a 40px boundary while row placement and
hit-testing used 72px. All catalogue categories shared this defect. Rows now
share one viewport boundary, are clipped below the filter, and clamp scrolling
after resize. Pixel and input regressions cover overlap and filter-lane activation.
Small Accent section titles now belong to the common side-column component above
page content, giving both left and right sections the same heading spacing.

## Proposed reusable reference-input feature (not implemented)

Add an Attach reference action beside the composer, with a visible removable
thumbnail/file chip and text explaining that Send shares it with the provider.
Keep typed text/image reference parts in AppChat and their presentation in
AppChatUi. The provider adapter needs verified image-message support and explicit
model capability metadata; unsupported profiles must explain the limitation.
Bound image dimensions, encoded bytes and total request size separately from text.

For HTML, initially accept source as inert text plus an optional user-supplied
screenshot. Rendering arbitrary HTML adds a browser/resource-loading boundary
and should be a separate deliberate feature, not hidden inside file attachment.
For a native starting template, retain the normal project Load/catalogue preset
path so the assistant receives an inspectable hierarchy rather than guessing it.

References describe appearance and intent; they do not grant tools or execution
authority. Convert them to native layout proposals using current schemas and the
same human Apply/Undo contract. Keep attachments/transcripts out of generated
application output. This feature needs its own tests for provider payloads,
unsupported models, cancellation, clearing, size limits and unchanged pre-Apply
design state before being advertised as available.

## Validation for this change

Debug Embedded: `build/Assistant-Debug-20260925-153341` PASS. Release Embedded
with native launch also passed, including AssistantDesigner 80/0, Regression 88/0,
AppChat 23/0, TitleGrid 37/0, ThemeDocument 31/0, ThemeStudioRole 380/0 and
ExportedThemeContract 27/0. The final Release run includes a subsequent filter
top-edge alignment adjustment; its directory/executable identity is recorded below.

`build/LayoutSkills-preset-exports.log`: all 13 generated creation/catalogue
packages built, including DialogTemplate. New deterministic tests cover the Grid
Label example, TitleCard with a wrapping Box of two buttons, rejection of a second
direct content child, preset insertion/validation/generation/Undo, clipped scrolling,
filter hit-testing and scroll clamping after resize.

`build/LayoutSkills-final-live.log`: OpenRouter `deepseek/deepseek-v4-flash`,
26/0 on final assistant instructions. Original exact dialog prompt: six calls/five
rounds, including rejected UiLabel.font_height and a corrected proposal. TitleCard
and explicit Label variants: five calls/four rounds each. Existing heading edit:
two calls/three rounds. No pre-Apply mutation, canonical Apply and Undo verified by
the live fixture. A native model-driven palette restyle was not tested or claimed.

Native checks verified both section headings, preset scrolling with the drawer
open and the Inspector title switch. Human Apply/Undo was not repeated in the
native drawer this turn; the existing live fixture exercised those host paths.
No image attachment, HTML rendering or global palette mutation was added.

Final Release: `build/Assistant-Release-20260925-153653` PASS.
Canonical application: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`;
SHA256 `77F5FD0D989A02CBE46CD286082EB0F2351C0C19C2D2C28B079E7D7E8A97D5E6`,
launched PID `401252`. Source base `ec6a9bd215d17ed1ca36aa3adcc1112b2a70b996`;
shared Ui unchanged at `ed635bc1dcbbd0d23524d1184843995c239c0963`.
