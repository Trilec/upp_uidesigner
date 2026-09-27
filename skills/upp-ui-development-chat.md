## File: SKILL.md

---
name: upp-ui-development
description: Build, integrate or repair native U++ C++ applications and reusable Ui controls, including ownership, layouts, themes, models, PropertyEditor and UMK packages. Use for native implementation; not for Designer JSON authoring or browser-only mockups.
---

# U++ and Ui development

Use the target checkout as API authority. Bundled references are portable snapshots,
not a substitute for the headers of the version being compiled. Do not assume the
machine paths or API summaries from an old session still apply.

## Start with the relevant contracts

- Read [coding and ownership](references/upstream/docs/00_UPP_CODING_GUIDE.md)
  and [build discovery](references/build.md) before implementation.
- For controls, use the [catalogue](references/upstream/docs/01_UI_CONTROLS_GUIDE.md)
  to locate the actual public header and maintained example. Verify setter names,
  return types, model binding and child-host APIs before writing calls.
- Read [layout and interaction](references/composition.md) for application shells,
  scrolling, focus and control integration.
- For appearance, read [themes](references/upstream/docs/02_UI_THEME_GUIDE.md);
  for a custom renderer, read [drawing](references/upstream/docs/07_UI_DRAWING_GUIDE.md).
- For data-backed views, read [models](references/upstream/docs/03_UI_MODEL_GUIDE.md).
  For inspectors, also read [PropertyEditor](references/upstream/docs/05_UI_PROPERTY_EDITOR_GUIDE.md).
- For control demos, read the [demo contract](references/upstream/docs/04_UI_DEMO_GUIDE.md).
  For graphs, start with [graph usage](references/upstream/docs/08_UIGRAPH_GUIDE.md);
  read [graph internals](references/upstream/docs/09_UIGRAPH_DEVELOPMENT.md) only when changing that engine.

## Preserve U++ semantics

Parenting is not deletion ownership. Prefer member controls, One or owning Array;
borrowed models outlive views. Ptr is an observer, not shared ownership. Moveable
is a relocation promise, not a generic optimization. Treat pick/clone deliberately.
Keep callbacks, timers, GUI capture and transient editor lifetimes explicit.
Callbacks can synchronously rebuild or destroy the originating control.

Use one authoritative semantic model and the current notification/request APIs.
Validate imported values before replacing live state. Null, inherited, mixed,
explicit None and an incomplete edit are distinct states. Keep preview, commit,
cancel and undo responsibilities separate.

Compose with native controls and Ui layouts; fix reusable defects at their owning
layer. Do not conceal a bad measurement, focus path, clipped paint or missing
notification with application-specific padding or repaint overrides. An actual
application spacing preference belongs in its layout.

Follow existing naming, header guards, package boundaries and direct dependencies.
Do not introduce an ops-table architecture merely because an old prompt recommends
it. Ordinary virtual interfaces or value models are appropriate when they match
the current code. Do not import old Chameleon recipes into Ui's role resolver.

## Verify what will ship

Build the runnable caller with the actual assembly and method. For shared changes,
run focused regression tests and repository-required checks, including relevant
Debug/Release and header/BLITZ coverage. Inspect changed controls in native UI:
small bounds, resize, keyboard/focus, Light/Dark, disabled and selected states.
Compile generated C++ when its contract changes. Successful compilation is not
proof of visual correctness or a working interaction.

Report exact artifact paths, checks performed and remaining limits. A historical
guide's publishing section does not authorize committing, pushing or releasing.


## File: references/build.md

# Build discovery and integration

Find the application's .upp, the installed U++ source/toolchain, assembly .var,
build method, Ui checkout and external nests. Use existing build scripts first.
The assembly's source nests resolve packages; its output folder does not.
Do not install or upgrade a compiler to bypass an unexplained local failure.

At the review date this workstation uses E:/upp-18468/umk.exe, CLANGx64 and
E:/apps/github/upp_Ui. These are examples to verify, never universal paths.
The Ui repository's GitHubOut.var records Ui, examples, Animation, statemachine
and uppsrc nests. Designer has its own assembly/package; it is not a Ui dependency.

UMK shape: `umk <comma-separated-nests> <main-package> <method> <flags> <output>`.
Use the installed tool's help: this workstation's UMK does not resolve a `.var`
file passed as the assembly argument. Translate its UPP nests to a comma-separated
argument and use `--out-dir` for the artifact cache.
For example, from the Ui checkout:

```powershell
& 'E:/upp-18468/umk.exe' 'E:/apps/github/upp_Ui,E:/apps/github/upp_statemachine,E:/apps/github/upp_animation,E:/upp-18468/uppsrc' 'examples/UiLabelDemo' 'CLANGx64' --out-dir './build/cache' -br +GUI './build/UiLabelDemo.exe'
if ($LASTEXITCODE) { throw 'Build failed' }
```

Debug is the default, -r selects Release, -b enables BLITZ and -a rebuilds all.
Check the installed tool's help before relying on other switches. Preserve both
ordinary and BLITZ compilation: missing direct includes can be masked by BLITZ.
If a shared header changes object layout and an incremental executable crashes,
rebuild affected dependencies cleanly before attributing it to application logic.

A library has no application entrypoint. A runnable GUI package supplies
GUI_APP_MAIN and GUI mainconfig. Declare direct dependencies in uses and keep
file membership complete. Image decoders need their plugin packages. Diagnose
missing packages via nests and missing WinMain via the selected main package.

For .lay resources, verify LAYOUTFILE against the actual include search roots;
do not impose an absolute path or the old guide's contradictory path rules.
Use the real project's resource macros and .iml pipeline for icons.

Generated native apps need their .upp, sources and assets together. Put behavior
outside overwritten generated regions. Wire OK/Cancel actions, not just labels;
set startup theme/mode, size and resize policy explicitly. Keep executables in
the project's bin/output convention, scratch evidence under build, and preserve
unsaved work before replacing a running application.

The full getting-started snapshot is available at
[upstream/GETTING_STARTED.md](upstream/GETTING_STARTED.md). Its release runner
has a clean-checkout workflow; do not discard local work to satisfy that gate.


## File: references/composition.md

# Native composition and control repair

Choose structure from sizing behavior. UiGridLayout places stable rows/columns;
UiBoxLayout handles ordered rows/columns, spacers and supported wrapping.
SetGridSize takes columns then rows; placement APIs take row then column—check
the overload. Fit measures preferred content subject to constraints, Fixed asks
for an extent, and Expand shares allocated space. Minimums do not create space
in an undersized parent. Avoid nested boxes with no meaningful sizing purpose.

Build a shell with header, expanding body and footer; put sidebars and workspace
inside the body. Use a wrapping Box where flow is intended, rather than assuming
CSS grid/flex rules are identical. Keep footer and assistant areas in the layout,
not painted over the workspace. A plain label is often sufficient for a title.

UiPanel is a styled surface, not automatic flow. Single-root hosts need one
layout root for multiple children. Read each host's attachment API: UiScrollPanel
exposes Content(), and its scrolling children belong there, not beside it.
UiGroupPanel has separate header/body slots. UiTab, UiStack and UiAccordion own
page/section visibility; don't let a layout accidentally show inactive pages.
Preserve logical SizePos/HSizePos/VSizePos when code measures and repositions
children; assigning a raw rectangle can erase the anchoring contract.

Frames, shadows, focus rings, skin insets and content margins participate in
measurement as documented. Apply DPI once. Use the shared geometry for Layout,
Paint, hit tests and min/preferred sizes. Don't fix a clipped icon by changing
its drawing rectangle while leaving hit and sizing rectangles unchanged.
Painter::Ellipse uses centre/radii; Draw::DrawEllipse takes a rectangle.
Keep aspect ratio for pictographic carets; explicit rectangular icon sizes may
intentionally stretch in controls that support that contract.

Prefer UiList/Tree/Table/Gallery models for data, not a Ctrl per record. A sparse
active cell editor is different from thousands of permanent editors. Keep stable
keys where needed and emit the narrow model notification after committed changes.
No speculative preallocation based on total logical item count during Paint.

Keyboard navigation follows the visible tree projection across expanded siblings
and groups and stops at ends. Handled keys must not leak into unrelated selectors.
Scroll interaction must route to the actual viewport/bar; overlays consume the
same clipped scrolled geometry. Capture loss cancels/completes per the control's
contract exactly once. Preserve page, selection and scroll through theme changes.

Ui themes distinguish preset, Light/Dark/System mode and semantic role. Keep
authoring/review mode separate from an exported app's startup mode. A custom
style is an owned snapshot; ClearCustomStyle resumes inheritance. Transparent
None is not a request for an OS-colored fallback. Use theme-aware icon/ink/focus
colors, and account for shadows in layout instead of painting beyond allocation.

For legacy CtrlLib controls, inspect the actual typed Style and ChPaint/ChMargins
contracts. SetStyle may borrow a style whose lifetime must cover use. Ui's
SetCustomStyle owns a snapshot. There is no universal interchangeable
Chameleon_Style matrix for both systems.


## File: references/upstream/GETTING_STARTED.md

# Getting Started

The shortest path is to build UiLabelDemo, then read its small application shell
and generated control code. Reading Graph workspace architecture is not a prerequisite
for displaying a label or button.

## 1. Use a declared U++ environment

The maintainer setup uses Windows, U++18468 and CLANGx64. Keep your established
compiler/framework version while validating a checkpoint; do not upgrade silently.
Ui depends on Core, Draw, Painter, CtrlCore, CtrlLib and the external Animation
package. The checked-in assembly also includes the external upp_statemachine nest.
Demos using PropertyEditor find its packages under this repository's Utilities path.

GitHubOut.var contains the maintainer's actual nest and output configuration:

```text
UPP = "E:/apps/github/upp_Ui/examples;E:/apps/github/upp_Ui;E:/apps/github/upp_statemachine;E:/apps/github/upp_animation;E:/upp-18468/uppsrc";
OUTPUT = "E:/apps/github/upp_Ui/build";
```

At those paths use that file unchanged. On another machine create a local assembly
.var with equivalent existing nests and your actual output folder; do not change
source paths or depend on somebody else's E: drive. Keep build output outside source
or in a git-ignored build directory. TheIDE and UMK must resolve the same packages.

## 2. Build a runnable demo, not the library

From the maintainer checkout in PowerShell:

```powershell
Set-Location E:\apps\github\upp_Ui
& 'E:\upp-18468\umk.exe' 'E:/apps/github/upp_Ui,E:/apps/github/upp_statemachine,E:/apps/github/upp_animation,E:/upp-18468/uppsrc' 'examples/UiLabelDemo' 'CLANGx64' --out-dir './build/cache' -b +GUI './build/UiLabelDemo.exe'
if ($LASTEXITCODE) { throw 'UiLabelDemo build failed' }
& '.\build\UiLabelDemo.exe'
```

Use the comma-separated nest list with the installed UMK. Its command-line help
does not promise `.var` path loading; passing `GitHubOut.var` directly to this
build reports a missing package. TheIDE can use the `.var` assembly; give UMK
the equivalent nests and an explicit artifact directory. Debug is the default; `-r`
selects Release and `-b` BLITZ. `-a` is rebuild-all, not a library-link workaround.
TheIDE users open an assembly with the same nests, select UiLabelDemo and run it.

Ui/Ui.upp and the PropertyEditor packages are libraries with no main entry point.
A missing main when asking for Ui.exe is a wrong build target, not a missing library
feature. Real test/demo packages supply GUI_APP_MAIN or CONSOLE_APP_MAIN.

## 3. Explore one control

Use Inspector for normal public behavior, Theme Overrides for explicitly authored
style, and Code for the corresponding C++. UiLabelDemo is the shell reference;
UiButtonDemo is the next action/state example. UiEditDemo covers the text-edit
family; UiIntFloatDemo covers numeric input. See the Controls Guide for other types.

Generated examples deliberately omit the demo shell. Copy them into an ordinary
U++ application with the stated headers/resources and lifetimes. Where the generator
requires host resource/provider/callback code, supply it explicitly. Compile the
actual output unchanged before treating the example as accepted.

## 4. Run the surgical implementation gate

Update clean main first; do not discard local work:

```powershell
git status --short
git pull --ff-only
if ($LASTEXITCODE) { throw 'Update failed; preserve local work and inspect' }
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\ValidateUiRelease.ps1
if ($LASTEXITCODE) { throw 'Read the reported validation evidence directory' }
```

The runner checks a clean main checkout and the freshly fetched origin/main,
reads the supplied assembly, derives UMK from its uppsrc nest unless -Umk is given,
and records tested HEAD, version, toolchain, logs and summaries. It never adds a
library main, commits code, deletes user files or kills an unrelated demo instance.
-AssemblyFile and -Method select an already installed equivalent environment.
-RequiredAncestor verifies a published checkpoint without requiring exact HEAD equality.

Profiles: Surgical (default), Headers (isolated public headers), Demos (build each
retained example), Full (retained test/demo builds and test execution). Select
-Configuration Debug/Release/Both and -Blitz explicitly; Full defaults to Both.
Broader profiles may reveal unfinished release work: failure is not permission to
weaken tests or remove a target. -SelfTest exercises the evidence parser only.

A surgical PASS is not full visual, generated-code, all-controls or cross-platform
acceptance. The release inventory and ACTIVE_WORK keep those boundaries explicit.

## Where to go next

Read the [Controls Guide](docs/01_UI_CONTROLS_GUIDE.md), then the guide for the
subsystem you are changing. Read [Coding](docs/00_UPP_CODING_GUIDE.md) before edits.
Graph users start with its usage guide; graph authors/developers use the separate
development guide and the existing workspace runner. Do not resurrect DesignMatrix
or historical checkpoint tasks as the current graph workflow.


## File: references/upstream/docs/00_UPP_CODING_GUIDE.md

# 00 — U++ Coding Guide

Reusable engineering rules for the Ui library family. Read this before changing a
control, demo or package. The Controls Guide describes the public surface; the
Theme, Models, Demo, PropertyEditor and Drawing guides own their respective
contracts. Graph usage and Graph development are separate guides.

## Packages and dependencies

A `.upp` file defines a package. A library has no application entry point and no
`mainconfig`. A runnable test/demo supplies `GUI_APP_MAIN` or `CONSOLE_APP_MAIN`
and an appropriate configuration. Do not add `main`/`WinMain` to a library merely
to make an executable build command link. Compile libraries through a real caller.

Declare direct package dependencies in `uses` and direct header dependencies in
source. Keep `.upp` file membership complete, including internal `.inc` parts
that TheIDE should expose. Production never depends on demos/tests. Headless
packages may use Core/Draw where required but never acquire CtrlCore/CtrlLib/Ui
merely to reuse a GUI helper. Image import/export callers declare the relevant
`plugin/png`, `plugin/jpg` or `plugin/bmp` package.

Use the actual assembly `.var` and installed build method. The repository's
`GitHubOut.var` is the maintainer's Windows configuration, not a portable path
promise. Record the U++ build and external dependency revisions when validating.
Build output belongs in the assembly output directory or a temporary evidence
directory, never among source files. See [Getting Started](../GETTING_STARTED.md).

## Source organization and comments

Headers have an include guard, necessary includes, `namespace Upp`, public API,
then private state. Use `_Package_Header_h_` guards. A Ctrl type using THISBACK
has `typedef T CLASSNAME;`. Keep substantial Paint/Layout/event implementations
in `.cpp`; tiny getters can remain inline. `.inc` is appropriate for deliberate
single-translation-unit implementation parts, not a second production copy.
Do not mechanically convert `.inc` to `.h` or fragment an already small control.

Each public control header explains author/license, purpose, intended usage,
GUI-thread context, non-obvious ownership and a short example. Document range,
null, normalization and callback semantics beside the relevant public methods.
Implementation comments explain invariants and reasons, not obvious assignments.
Remove stale alternative implementations and misleading comments, not useful
rationale. Header comments refer to the library release identity in `UiVersion.h`;
individual controls do not maintain independent release counters.

Public API follows U++ naming: SetX/GetX, WhenX events, SetData/GetData for binding.
Keep family vocabulary stable: SetText for primary text, SetTitle/SetSubTitle for
container identity, UiDirection/UiAlign for direction/alignment. Convenience
fluent APIs are allowed where the family already uses them. Do not add aliases
or meaningless getters simply to force spelling symmetry. Changing a public
contract requires its callers, tests, generated C++ and documentation to change
in the same coherent slice; discuss disruptive changes before publishing them.

New demo/application names should expose responsibility: `tc_header`,
`box_header_actions`, `pnl_preview`, `stk_pages`, `btn_copy`, `lbl_caption`,
`pe_inspector`, `pe_model_inspector`, `edit_code`, `str_code`, `img_preview`,
`val_icon`, `ctx_theme`. Do not churn established member names for cosmetics.

## Parenting is not C++ ownership

Adding a Ctrl to a parent establishes its GUI relationship; it does **not**
transfer responsibility for deleting the C++ object. Removing it detaches it.
Prefer member controls, `One<T>` or an explicitly owning `Array<T>`. Never infer
heap ownership merely from Add/SetContent/SetModel. A borrowed child/model must
have an explicit lifetime contract; use `Ptr<Ctrl>` for potentially destroyed
controls and confirm the current parent before resizing/detaching borrowed
content. Reject self/ancestor parenting before changing existing content.

Do not attach one control to two parents or introduce parent cycles. Composite
construction/destruction, child removal/reparenting and model replacement are
regression-test cases. External models must outlive their active binding unless
the API explicitly supplies a lifetime-safe detach mechanism.

### Core ownership and relocation details

`Ptr<T>` is a non-owning, destruction-aware observer for a compatible `Pte<T>`
object; it is not shared ownership and does not prolong the pointee's lifetime.
`One<T>` owns one object; `Array<T>` owns its elements and is useful for controls
and other types that cannot be relocated. Declare borrowed models before the
views that use them when member destruction order must keep the model alive.

`Moveable<T>` declares that the type is safely relocatable; it is not a switch
to make an arbitrary class faster. Never add it to a Ctrl, mutex, self-referencing
object or address-sensitive type just to put that type in a Vector. Use an owning
indirect container instead. Vector growth can invalidate element references;
Array pointees have different stability guarantees, but erasing one still ends
its lifetime. Do not hold container references across callbacks that mutate it.

`pick` transfers resources; use the source afterwards only as permitted by its
picked-state contract. `clone` requests an explicit deep copy for a supporting
type. Neither operation makes external pointers or borrowed model lifetimes safe.
Prefer the project's U++ containers and string/value APIs; adapt deliberately at
external-library boundaries rather than mixing ownership conventions implicitly.

Keep application data separate from modeless dialog lifetime. A local dialog is
safe during a blocking Run(); returning after Open() requires an owner that stays
alive. A copied lambda capturing `this` still holds a raw pointer. Cancel owned
timers and detach observers before destroying the state they access. Post GUI work
through the established dispatch path; do not block the GUI waiting on a worker
that itself needs the GUI lock.

## Callbacks, capture and timers

A committed notification observes the new public state. A request event reports
intent before an application-owned model mutation. These are different contracts;
see [Models](03_UI_MODEL_GUIDE.md). State whether programmatic setters emit user
events. Keep preview, commit and cancellation distinct; do not manufacture a
second commit on capture loss. Document whether a cancelled live edit rolls back
or retains its last live value.

A user callback may synchronously rebuild or destroy the originating control.
Copy an in-flight callback before dispatch where reconfiguration can clear it;
guard subsequent member access with a lifetime-aware pointer. Capture by value
where appropriate and never leave an unguarded delayed raw `this`. Rebinding
must ignore inactive/old model notifications, including address reuse.

Use owned TimeCallback/UiFrameTicker or the established Animation facility.
Cancel work on hide/remove/destroy where it no longer has a purpose. Ctrl timer
IDs are internal byte-offset identifiers, not arbitrary application handles;
do not invent large integer IDs. A replaced one-shot uses KillSet, not an
accumulating queue. Idle controls need no repeating clock; caret blinking and
intentional animation have explicit ownership and stop conditions.

## Values, validation and semantic authority

Use typed values where fixed; U++ Value/ValueArray/ValueMap are appropriate for
binding, property models and durable payloads. Validate types, enums, ranges,
finite numbers and Null sentinels before mutation. Validate imports into a
candidate before replacing live state. ASSERT protects programmer invariants;
it is not Release input validation. Preserve incomplete numeric text locally
until a complete value can be committed.

Prefer Vector, Array, VectorMap, Index, One and pick/clone with clear ownership.
One concern has one semantic authority. View projections and raster/geometry
caches are disposable derivatives, not parallel editable models. Do not restore
retired RefreshFromModel-style synchronization when model notifications suffice.
Persistence schema versions are independent from the library release version;
never reset or increment a schema for a cosmetic release-number change.

## Geometry, themes and rendering

Apply DPI exactly once. Fit/Fixed/Expand are sizing modes; alignment positions
within an allocation. GetMinSize/GetContentSize/Layout, hit testing and generated
code must agree on frames, skin insets, content margins, gaps and item spacing.
Geometry-affecting setters invalidate layout and paint; visual-only setters
invalidate paint. An unchanged setter should avoid needless work.

Every themeable visible control has meaningful Minimal Standard/Subtle/Accent/
Alert behavior in Light and Dark. Family typography roles and actual content
colors remain distinct from semantic emphasis. Pure layout/value helpers do not
need invented colored faces. Follow [Theme](02_UI_THEME_GUIDE.md).

Paint must not mutate models, emit semantic callbacks, open resources or start
clocks. Keep clipping balanced and handle empty/tiny rectangles. Direct Draw is
appropriate for simple straight geometry/text; native Painter supplies smooth
curves. Reuse bounded exact rasters when beneficial. Do not impose full-control
buffers on every widget. The shared final-device-pixel geometry budget is 0.35 px
within its documented numeric envelope, not a guarantee of identical backend
stroke/AA pixels. See [Drawing](07_UI_DRAWING_GUIDE.md).

## Review and release acceptance

Audit every concrete public control, not only controls with convenient demos.
Check API/ownership, four-role behavior, input/callback/cancellation, rendering
cost, source docs, canonical demo, generated C++ and relevant tests. Inheritance
can share implementation review but does not waive specialized behavior checks.
The machine-readable [release inventory](../tests/ui_release_inventory.json)
records coverage and unresolved work; an untested row is not a PASS.

Debug and Release must compile. A non-BLITZ/header-isolation build catches hidden
include dependencies; also preserve the supported BLITZ path. Keep diagnostics
and tests deterministic and fail on nonzero exit, missing summary or zero executed
checks. Do not delete a legitimate regression merely to reduce target count.
A replacement demo/test must retain the old useful coverage before removal.

Version changes come from `Ui/UiVersion.h`. The release candidate is not a claim
that all platform/visual gates passed. Public 1.0 compatibility starts only after
the declared release surface and known limitations are accepted. Independently
versioned sibling packages retain their own version histories.

## Publishing and recovery

Refresh remote main, inspect complete touched source/callers/tests, implement a
coherent slice, review the full diff, run git diff --check, publish and verify the
remote commit/diff/ancestry. Rebase only the intended changes onto a newer main;
never overwrite concurrent work or force-push it backwards.

Keep ACTIVE_WORK.md at no more than 100 lines: BASE / TASK / TOUCHED / STATUS /
PUBLISHED / VALIDATION / NEXT ACTION for current work only. Store contracts in
these guides, history in Git and validation logs outside source. PUBLISHED may
identify the containing commit by a stable git-log path to avoid a self-referential
SHA. Preserve concurrent workstreams and their outstanding validation boundaries.

Give the validator one copy-paste task: latest branch, required ancestor, exact
commands, expected summaries, focused manual checks, stop conditions, evidence
and allowed minor fix policy. Source-reviewed, compiled, runtime-tested and
visually accepted are separate evidence. Report only the states actually proved.


## File: references/upstream/docs/01_UI_CONTROLS_GUIDE.md

# 01 — Controls Guide

Public control catalogue for Ui. Each concrete type has an entry below; shared
base/model/drawing helpers are listed separately. The [release inventory](../tests/ui_release_inventory.json)
is the audit/validation register. A listed demo is a starting point for exploration,
not a claim that its entire API or generated code has already passed release acceptance.

## Common contracts

Include `<Ui/Ui.h>` for the full library or the narrower public header. Read the
[Coding Guide](00_UPP_CODING_GUIDE.md) for ownership and naming. Controls normally
run on the GUI thread. Adding a child establishes parenting, not C++ deletion
ownership; keep borrowed children/models alive or detach them explicitly.

Fit/Fixed/Expand select size; alignment selects position in an allocation. Apply
DPI once. GetMinSize/GetContentSize/Layout and hit geometry must use the same
frame, skin inset, content margin, gap and item-spacing vocabulary. Single-root
hosts need a layout inside their slot for several controls; do not overlap multiple
roots accidentally. UiGroupPanel has distinct header and body slots.

Themeable controls use Minimal Standard/Subtle/Accent/Alert in Light/Dark with
family-appropriate semantics. Typography roles are independent. Palette states
are Normal/Hot/Pressed/Disabled; selection, focus and read-only are separate.
StyleDefault is immutable; SetCustomStyle owns a snapshot; ClearCustomStyle
restores the current theme. Convenience setters can create a complete custom
snapshot: consult the header rather than assuming partial live inheritance.
Explicit None means intentionally absent, not inherited. See [Theme](02_UI_THEME_GUIDE.md).

Callbacks must document programmatic versus user behavior and preview/commit/
cancellation. A committed notification sees new state. Model-backed mutations
can be request-first; see [Models](03_UI_MODEL_GUIDE.md). SetData/GetData is binding,
not permission to coerce invalid types or use Null sentinels as real values.

Native Draw is suitable for simple straight geometry/text; Painter supplies
antialiased curves. Do not duplicate shape or raster systems per control. The
[Drawing Guide](07_UI_DRAWING_GUIDE.md) owns geometry/cache/performance rules.
The retired UiComposite property-row family must not return: compose Ui controls
and the production PropertyEditor instead.

## Catalogue

No dedicated demo is implied where the Example cell says "family coverage to accept".
A shell using a control does not substitute for its behavioral/property coverage.

| Control | Purpose | Reference example |
| --- | --- | --- |
| [UiLabel](../Ui/UiLabel.h) | Styled text, selection, wrapping, icons and media. | [UiLabelDemo](../examples/UiLabelDemo) |
| [UiButton](../Ui/UiButton.h) | Primary stateful action. | [UiButtonDemo](../examples/UiButtonDemo) |
| [UiToolButton](../Ui/UiToolButton.h) | Compact toolbar action; used in the reference shell. | [UiLabelDemo](../examples/UiLabelDemo) |
| [UiSplitButton](../Ui/UiSplitButton.h) | Primary action plus a separate dropdown action. | [UiSplitButtonDemo](../examples/UiSplitButtonDemo) |
| [UiCheckBox](../Ui/UiCheckBox.h) | Independent checked state and supported visual variants. | [UiCheckBoxDemo](../examples/UiCheckBoxDemo) |
| [UiRadioButton](../Ui/UiRadioButton.h) | Exclusive-choice presentation and grouping behavior. | [UiRadioButtonDemo](../examples/UiRadioButtonDemo) |
| [UiToggle](../Ui/UiToggle.h) | Boolean switch with track/thumb styling. | [UiToggleDemo](../examples/UiToggleDemo) |
| [UiBreadcrumbs](../Ui/UiBreadcrumbs.h) | Path navigation with optional icons and separators. | [UiBreadcrumbsDemo](../examples/UiBreadcrumbsDemo) |
| [UiLineEdit](../Ui/UiLineEdit.h) | Single-line text; shared UiBaseEdit behavior. | [UiEditDemo](../examples/UiEditDemo) |
| [UiIntEdit](../Ui/UiIntEdit.h) | Integer entry with numeric bounds and step behavior. | [UiIntFloatDemo](../examples/UiIntFloatDemo) |
| [UiFloatEdit](../Ui/UiFloatEdit.h) | Floating-point/scientific input with incomplete-input handling. | [UiIntFloatDemo](../examples/UiIntFloatDemo) |
| [UiPasswordEdit](../Ui/UiPasswordEdit.h) | Password masking and visibility controls. | [UiEditDemo](../examples/UiEditDemo) |
| [UiMultiEdit](../Ui/UiMultiEdit.h) | Multi-line editing and whitespace/tab policy. | [UiEditDemo](../examples/UiEditDemo) |
| [UiMaskEdit](../Ui/UiMaskEdit.h) | Mask-driven entry, formatting and validation. | [UiEditDemo](../examples/UiEditDemo) |
| [UiSlider](../Ui/UiSlider.h) | One scalar value within a domain. | [UiSliderDemo](../examples/UiSliderDemo) |
| [UiRangeSlider](../Ui/UiRangeSlider.h) | Ordered interval; optional adjustable inner bounds. | [UiSliderDemo](../examples/UiSliderDemo) |
| [UiSliderEdit](../Ui/UiSliderEdit.h) | Slider with a direct numeric editor. | family coverage to accept |
| [UiRangeSliderEdit](../Ui/UiRangeSliderEdit.h) | Interval slider with lower/upper numeric editors. | family coverage to accept |
| [UiRangeSegments](../Ui/UiRangeSegments.h) | Contiguous labeled segments over one fixed scalar domain. | [UiRangeSegmentsDemo](../examples/UiRangeSegmentsDemo) |
| [UiScrollBar](../Ui/UiScrollBar.h) | Scroll position/extent and themed arrows/thumb. | [UiScrollBarDemo](../examples/UiScrollBarDemo) |
| [UiProgressBar](../Ui/UiProgressBar.h) | Linear determinate/indeterminate progress. | [UiProgressBarDemo](../examples/UiProgressBarDemo) |
| [UiProgressRing](../Ui/UiProgressRing.h) | One amount against a total, circular presentation. | [UiProgressRingDemo](../examples/UiProgressRingDemo) |
| [UiChartRing](../Ui/UiChartRing.h) | Several proportional values composing one ring. | [UiChartRingDemo](../examples/UiChartRingDemo) |
| [UiMatrixSelector](../Ui/UiMatrixSelector.h) | Spatial cell/ordered-pair choice with shared glyphs. | [UiMatrixSelectorDemo](../examples/UiMatrixSelectorDemo) |
| [UiColorMatrix](../Ui/UiColorMatrix.h) | One to eight ordered color values with one shared picker. | family coverage to accept |
| [UiDateTime](../Ui/UiDateTime.h) | Local date/time/date-time input and picker. | [UiDateTimeDemo](../examples/UiDateTimeDemo) |
| [UiColorPicker](../Ui/UiColorPicker/UiColorPicker.h) | Multi-slot color editing, palettes and image/screen picking. | [UiColorPickerDemo](../examples/UiColorPickerDemo) |
| [UiDropdown](../Ui/UiDropdown.h) | Collapsed choice and model-backed popup. | [UiDropdownDemo](../examples/UiDropdownDemo) |
| [UiMenu](../Ui/UiMenu.h) | Command/check/radio/submenu model presentation. | [UiMenuDemo](../examples/UiMenuDemo) |
| [UiPanel](../Ui/UiPanel.h) | Styled surface with ordinary child parenting; use a layout child to arrange content. | [UiPanelDemo](../examples/UiPanelDemo) |
| [UiDirectContentHost](../Ui/UiDirectContentHost.h) | Borrowed single child with independent Fit/Fixed/Expand axes. | family coverage to accept |
| [UiGroupPanel](../Ui/UiGroupPanel.h) | Titled frame with separate header and body root slots. | [UiPanelDemo](../examples/UiPanelDemo) |
| [UiTitleCard](../Ui/UiTitleCard.h) | Title/subtitle/media with an adjacent content cell. | [UiTitleCardDemo](../examples/UiTitleCardDemo) |
| [UiStack](../Ui/UiStack.h) | Exclusive page hosting and measurement. | family coverage to accept |
| [UiAccordion](../Ui/UiAccordion.h) | Collapsible real-child sections with optional reorder. | [UiAccordionDemo](../examples/UiAccordionDemo) |
| [UiScrollPanel](../Ui/UiScrollPanel.h) | Bounded viewport around one content root. | [UiScrollPanelDemo](../examples/UiScrollPanelDemo) |
| [UiTab](../Ui/UiTab.h) | Tabbed page host with role-owned cap and strip fills. | [UiTabDemo](../examples/UiTabDemo) |
| [UiSplitter](../Ui/UiSplitter.h) | Pane sizing with styled split handles. | [UiSplitterDemo](../examples/UiSplitterDemo) |
| [UiQuadSplitter](../Ui/UiQuadSplitter.h) | Four-pane composition over ordinary splitters. | [UiSplitterDemo](../examples/UiSplitterDemo) |
| [UiAbsoluteLayout](../Ui/UiAbsoluteLayout.h) | Exact local child rectangles without automatic reflow. | family coverage to accept |
| [UiGridLayout](../Ui/UiGridLayout.h) | Logical rows and columns with stable placement. | family coverage to accept |
| [UiBoxLayout](../Ui/UiBoxLayout.h) | Ordered row/column flow with Fit/Fixed/Expand. | family coverage to accept |
| [UiList](../Ui/UiList.h) | Sequential model view and visible renderer pooling. | [UiListDemo](../examples/UiListDemo) |
| [UiTree](../Ui/UiTree.h) | Stable hierarchical model identity and visible projection. | [UiTreeDemo](../examples/UiTreeDemo) |
| [UiTable](../Ui/UiTable.h) | Coordinate/range model view with editing and headers. | [UiTableDemo](../examples/UiTableDemo) |
| [UiGallery](../Ui/UiGallery.h) | Tile/image presentation of a list model. | [UiGalleryDemo](../examples/UiGalleryDemo) |
| [UiDoc](../Ui/UiDoc/UiDoc.h) | Document view/editor over the authoritative UiDocCore. | [UiDocDemo](../examples/UiDocDemo) |
| [UiBezierCurveEditor](../Ui/UiBezierCurveEditor.h) | Editable cubic curve with selection and data binding. | family coverage to accept |
| [UiBezierCurveField](../Ui/UiBezierCurveField.h) | Curve editor with optional formula and copy composition. | family coverage to accept |
| [UiNodeGraph](../Ui/UiGraph/UiNodeGraph.h) | Retained graph topology, routing, hierarchy and presentation. | [UiGraphDemo](../examples/UiGraphDemo) |

## Edit family

UiBaseEdit provides shared editing, selection, placeholder, caret, side/spin and
style behavior. Concrete Line/Password/Multi/Mask/Int/Float types retain their own
input policy; sharing a base is not proof that specialized validation is identical.
UiEditDemo is the text-family direction; UiIntFloatDemo is its numeric companion.
Older individual demos remain until capability/export coverage is reconciled.

UiFloatEdit supports decimal/scientific notation, signed exponents and temporarily
incomplete text. Min/Max/MinMax constrain committed values; Step drives numeric
editing; Precision formats a complete value; NotNull controls emptiness.
TryGetValue and IsInputComplete distinguish a valid finite value from incomplete
input without destructive rewriting while typing. SetData accepts numeric data/
text and GetData returns the typed value or permitted Null.

```cpp
UiFloatEdit amount;
amount.MinMax(-1000, 1000).Step(0.25).Precision(3).ShowSpin(true);
amount.SetValue(12.5);
```

Mask validators/formatters, password visibility, multiline whitespace and numeric
spin behavior need separate family cases. Clipboard, Unicode, focus loss, Escape,
readonly/disabled transitions and partial exponents are acceptance inputs, not
just a successful constructor.

## Date/time and color fields

UiDateTime provides local date, time and combined date/time modes with locale or
ISO formatting, optional seconds and 12/24-hour display. It does not perform
time-zone conversion. Editable and presentation-only modes have separate frame
and clipboard policies; read the public header for null and range constraints.
Its picker uses the dropdown arrow artwork with preserved aspect ratio.

UiColorMatrix holds one to eight related colors and opens one UiColorPicker for
the complete set. Theme roles style the surrounding surface; they do not recolor
the authored swatches. Use the picker directly when the application needs its
larger editing surface rather than a compact multi-color field.

## Scalar sliders, intervals and segmented ranges

UiSlider owns one scalar value. UiRangeSlider reuses its style for an ordered
lower/upper interval inside a hard range. SetRange sets the domain, SetStep sets
snap units, SetValues sets the interval, GetLowerValue/GetUpperValue read it.
Active handles are explicit keyboard/wheel targets. SetStart/End/StartEnd are
established animation-friendly aliases over the same state, not another model.

EnableAdjustableBounds adds lower-bound/lower-selection/upper-selection/upper-bound
ordering inside the hard range. SetBounds edits those inner bounds. Bound handles
remain distinct targets; ShowEndpointMarkers controls the hard-domain markers.
UiRangeSliderEdit composes the authoritative slider with two UiFloatEdit fields;
SetFieldWidth/SetGap/SetInset affect composition, not semantic value. Slider() exposes
the actual range slider. Its binding remains the documented two-element ValueArray.

UiRangeSegments partitions one fixed domain into N contiguous labeled spans with
N-1 boundaries. It is neither a chart nor a continuous color-gradient editor.

```cpp
UiRangeSegments ranges;
ranges.SetRange(0, 100).SetStep(1).SetSegmentCount(4);
Vector<double> thresholds;
thresholds << 18 << 48 << 76;
ranges.SetBoundaryValues(thresholds);
Vector<double> current = ranges.GetBoundaryValues();
```

UiRangeSegment stores span, label, optional color (Null means automatic), and Value
data. SetSegments normalizes proportional weights; invalid/negative weights count
as zero and all-zero weights become equal. The domain endpoints must be finite
and have a finite difference. Invalid scalar setters leave state unchanged.
MinimumSegmentSpan enforces feasible minima; boundary i changes only spans i/i+1.
SetBoundaryValues validates the whole vector before changing structure. Split inserts
a boundary; Remove merges into a neighbor. Endpoints never move during a drag.

Direction/reverse/resize changes only projection, never scalar ordering or payloads.
Percent versus Domain labels are presentation. ShowLabels, ShowBoundaryValues,
ShowEndpointValues, ShowValuesOnInteraction and ShowDividers control presentation.
Selection of a segment is independent from the active boundary. Labels are omitted
when they cannot fit; a narrow vertical track is not a promise of readable long text.

WhenChanging reports live user edits and WhenAction committed edits; programmatic
setters are silent. WhenSegmentSelect/WhenBoundarySelect report user selection.
Capture loss/disable/Escape ends a live drag without another commit and retains its
last live value. Callbacks see committed state and may rebuild/destroy the control.
GetData returns ValueArray of maps (span/label/optional color/data); SetData accepts
that representation. There is no second mutable model.

Up to eight palette anchors form deterministic Series or sampled Gradient colors.
Extra authored Series entries cycle with predictable light/dark variants. Inherited
semantic ramps span the actual segment count; explicit segment colors win. Whole
style snapshots stay explicit through a theme change. The actual content strip and
thumbs use bounded AA/shared exact rasters; text stays Draw and no timer is owned.
UiReleaseSmoke protects numeric, lifetime, role and real-pixel/cache cases alongside
UiRangeSegmentsRunTests. In Graph, thresholds must not resize the preview camera.

## Matrix and color choices

UiMatrixSelector is one small bounded Ctrl for Position9, Compass8, Region5,
QuadPair or Cardinal4 spatial choices. Cardinal4 has no invalid center/diagonals.
SetPreset configures the grid; cells carry label/value/icon/glyph/visibility/enabled.
SetCell/Label/Value/Icon/Glyph and EnableCell/ShowCell expose content. Cell, selected
and readout roles are distinct. Grid dimensions/count and settled cell/readout
rectangles are queryable; sizing derives from actual geometry.

Single selection uses SelectIndex/GetSelectedIndex/GetSelectedLabel and SetData/
GetData. Pair selection uses SetPair, pair indices/completeness/orientation/direction
and readout. SetDefault/ClearDefault/ShowDefault supplies a dashed default marker
separate from committed selection. Arrow navigation skips ineligible cells;
WhenChanging is preview and WhenAction committed activation. Programmatic fire_action
arguments are explicit. Cell/readout gaps, radius, face/frame, fonts and insets are
style, not specialized domain relationships.

UiColorMatrix edits one through eight ordered colors. SetColors/SetColorCount/
SetColor/GetColors and per-slot labels describe the same set. Activating a swatch
opens one picker with the complete set, not a separate picker/model per color.
Cancel restores all opening colors; WhenChanging previews, WhenAction commits and
WhenSelect identifies the active slot. EnablePicker(false) makes selection-only.

Adaptive grid layout fits useful square swatches within actual capacity. Slot
size bounds/gap/frame/radius/shadow and containing-surface style use shared Ui
vocabulary. Actual swatch faces are the values, not role-colored decorations.
One color binds as Color; multiple colors as ValueArray. Arrows follow the settled
grid and Enter/Space opens the picker. Capacity matches the current picker contract.

UiColorPicker's authoritative slots are SetSlotCount, SetActiveSlot, SetSlot and
GetSlots (one through eight). It supports alpha/color models, palettes, image
analysis, stash/session and screen picking. Preview/commit/dialog acceptance/cancel
are distinct. Ordered multi-selection/drag transfer must preserve palette order.
UiColorPickerPaletteLab is reusable conversion/palette/analysis support, not another
control required per swatch.

## Local date and time

UiDateTime holds one local/naive U++ Time. DateMode/TimeMode/DateTimeMode, Locale/Iso
format, locale/12/24 clock, ShowSeconds, language and first-day settings affect
presentation. Time-zone/instant conversion belongs outside this control.

SetEditable and SetPresentation distinguish editable/picker from read-only display;
presentation is normally chromeless unless ShowPresentationFrame is enabled.
The mode selects Calendar, Clock or CalendarClock, themed through Ui. No second
popup model owns a competing value. AllowCopy/AllowPaste apply to keyboard and
explicit clipboard APIs; paste also requires editable mode.

AllowNull/ClearValue, SetRange/SetDateRange/ClearRange constrain values. CommitText
validates complete calendar/time input; invalid text restores the stored formatted
value and reports WhenInvalid instead of corrupting state. SetValue/GetValue,
SetDate/GetDate/SetTime/SetNow/SetToday and SetData/GetData share the same authority.
WhenChanging, WhenAction, WhenInvalid and WhenOpenPicker are distinct notifications.
All locale/ISO/12/24 formatting paths must honor ShowSeconds.

## Containers and layout

UiPanel/UiScrollPanel/UiTitleCard each host one content root; UiTitleCard uses its
adjacent SetContentCell. UiGroupPanel has independently replaceable header-content
and body-content roots. Use a box/grid/absolute layout inside a root for several
children. Parenting still does not imply deletion ownership.

GroupPanel header placement supports Top/Bottom/Left/Right, mode Outside/Center/
Inside, identity alignment and separate header-child alignment. Opposite/trailing
space is reserved for header content without covering the identity block. Public
GetHeaderContentRect/GetBodyRect match prospective geometry. Minimum measurement
includes identity, child, gaps, insets and surface; forced undersize stays valid.
In Center mode the frame avoids both occupied header rectangles. Retired SideTitle
stream fields remain only for compatibility, not runtime styling.

UiDirectContentHost supplies Fit/Fixed/Expand, fixed/min/max sizes and alignment
without painted styling. It borrows one child and ignores destroyed/externally
reparented content. ClearContent detaches only its own child. Self/ancestor parenting
is rejected. UiStack and UiTab own page-selection semantics; UiAccordion owns
section/collapse/reorder semantics. Layout helpers are not replacements for those
page states. Hidden pages must not be accidentally re-shown by flow participation.

UiScrollPanel's scrolling children belong under Content(). GetViewportRect is the
visible allocation and GetContentSize is the measured content extent. WhenScroll
reports a changed scroll origin after applying it, including programmatic changes;
GetScrollBarAt accepts panel-local coordinates and returns a borrowed scrollbar
for interaction routing. Wheel input uses the horizontal axis when it is the only
available scrollbar, or with Shift when horizontal scrolling is available.

## Progress, rings and custom painting

UiProgressRing is one current amount/total, optional percent/custom center text,
gradient, intro animation and determinate/indeterminate state. Set/Get/GetTotal/
GetRatio/GetPercent describe its value. Its centered square stays circular; stable
rasters cache, animation owns UiFrameTicker and stops with its lifecycle.

UiChartRing is several nonnegative segment values, labels and optional colors.
Positive values normally define total; a larger explicit total can leave track
remainder. Gap and cap-roundness are visible metrics. It has no implicit legend,
selection, nested/exploded-ring interaction. Both use shared native stroked arcs;
filled wedges/donuts use UiShapes helpers instead.

Controls with supported background/content/foreground or part-aware paint hooks
use their documented context/handled contracts. Slider Track/ActiveTrack/Thumb,
ScrollBar parts and Toggle Track/Thumb are examples; do not assume every control
has every hook. Generated code must reproduce a selected override with real APIs,
not unexplained demo-local overpainting. Explicit paint and hit bounds must agree.

## Collections, documents and supporting APIs

List/Gallery/Dropdown use UiListModel, Tree UiTreeModel, Table UiTableModel and
Menu UiMenuModel; ordinary content uses bounded UiItemRender pools. View-owned
selection, disclosure, command/check/radio state and editing remain appropriate to
the domain. GetMinSize/Layout/paint never allocate one Ctrl per logical item.
UiDoc/UiDocCore ownership, transactions and future extraction are in the Models
Guide. Graph usage and retained development have their own two guides.

UiList's natural height follows the active model's row count, with at least one
row when empty, plus styling insets. It no longer reserves four placeholder rows.
Constrain the containing layout or viewport when a large list must stay bounded.

Supporting public surfaces include UiBaseEdit and UiIndicatorBase, UiAxis,
UiLayoutCursor/UiMeasure, UiStyle/UiTheme, UiGeometry/UiShapePath/UiShapes/UiDraw,
UiRenderLayer/UiFrameTicker, UiIcons, model/render types and Graph/Doc core types.
The separate optional UiOsFileDialog package has platform-specific integration;
it is not evidence of native dialog validation on every platform. PropertyEditor
and PropertyEditorCore are reusable siblings, not dependencies on UiDesigner.

These helpers need documented responsibility and dependency/lifetime coverage,
not invented visual roles or a separate executable demo for every header. The
release inventory tracks missing dedicated/family demonstration, source review,
generated-code and platform evidence explicitly rather than inferring completion
from the catalogue or a general Ui compile.


## File: references/upstream/docs/02_UI_THEME_GUIDE.md

# 02 — Theme and Style

UiStyle provides value-only style primitives. UiTheme maps context and semantic
roles into concrete family styles. A control either follows those defaults or owns
an explicit custom-style snapshot. There is no separate demo-only theme system.

## Minimal baseline and roles

The universal semantic roles are `UiRole::Standard`, `Subtle`, `Accent`, `Alert`.
Every themeable visible control must give each role sensible Minimal Light/Dark
behavior. A role is emphasis, not an interaction state, renderer type or LOD.

| Role | Meaning |
| --- | --- |
| Standard | ordinary readable presentation and hierarchy |
| Subtle | reduced emphasis without losing readable/interactive affordances |
| Accent | emphasis using the theme's accent family |
| Alert | warning/destructive emphasis with usable contrast |

Different families apply emphasis to different parts. A label does not need a
button's filled face. Pure layouts, nonvisual models and helper geometry have no
invented colored face. Actual swatch/image/series data is separate from surrounding
control decoration; Alert must not alter a color being edited.

Family vocabulary remains supported: UiButtonRole, UiToolButtonRole, UiEditRole,
UiPanelRole and UiLabelRole. Typography roles (Body, Headline, Subheadline, Title,
Caption, Badge, Footnote; UiTextSize Body/H1/H2/H3) are distinct from universal
semantic emphasis. Label emphasis is geometry-neutral, not a hidden margin change.

Inherited RangeSegments palettes now distinguish all four roles: Standard retains
series colors, Accent uses the accent ramp, Subtle a light-to-mid-dark neutral ramp,
and Alert a theme-primary-to-orange ramp. Inherited tonal ramps span the actual
segment count. Explicit segment colors win; an authored Series palette retains
its deterministic cycle/tint behavior. These are range defaults, not a demand that
all controls have orange endpoints.

## Theme context and lifecycle

UiThemePreset: Minimal, Pill, Linear, Solid, Outline, Compact, Layered.
UiThemeMode: Light, Dark, System. System currently resolves to Light where the
platform-following policy is not wired; do not advertise universal OS-mode tracking.
UiThemeContext stores preset/mode and supports serialization.

The theme revision invalidates cached inherited styles. Theme-driven controls
re-resolve when context changes; explicit custom styles are not overwritten.
Read effective style through the control's documented style API. Never mutate the
shared StyleDefault. Presets tune family structure/metrics and role palettes through
the existing resolvers; do not introduce a parallel per-control theme registry.

The normal lifecycle is StyleDefault, GetStyle/effective resolution,
SetCustomStyle, ClearCustomStyle, HasCustomStyle, and OnStyleChanged invalidation.
Convenience styling setters may create a **complete snapshot**, not a per-field
live override. The header/family API must say which. ClearCustomStyle restores the
current theme, not the theme that happened to exist before the override.

A builder offering per-field inheritance owns an authored recipe: resolve a fresh
base and apply active authored fields. It must not silently freeze every inherited
color. That recipe is host/demo state, not a second production theme authority.

## Style primitives

StyledPalette has four slots: ST_NORMAL, ST_HOT, ST_PRESSED, ST_DISABLED. Each has
face (UiFill), frame, ink and icon. ResolveStyledState selects the interaction slot;
selection/focus/read-only semantics remain explicit for the control.

StyledMetrics contains font/use-font, content margin, radius, frame width/visibility,
face visibility, dashed frame/pattern, focus and shadow/highlight. StyledSkin describes
image-backed nine-slice drawing: `slice` affects painting; `content_inset` affects
geometry. Use the actual family image-mode behavior, not an invented second fit mode.

UiFill::None means intentionally no face. Solid and image-backed fills are explicit
choices. Inherited/absent override is different from an explicit None. Never substitute
OS light-face colors merely because a resolved fill is transparent. UiTab's active
cap/strip fix preserves that distinction and has a native pixel regression.

The common geometry is outer -> shadow-adjusted surface -> frame/skin-adjusted face
-> content margin. UiStyledInnerRect and UiStyledOuterSizeFromContent own that seam.
Layout, hit testing and generated code must agree on it. Apply DPI exactly once.

## Colors, icons and decoration

Default palette colors belong in StyleDefault or role construction, not arbitrary
RGB substitutions in Paint. Existing LtColor/DkColor/DisabledColor helpers produce
state variants; the resolved role must remain distinguishable and readable.
MinimalRole(mode, role), ApplyPalette and the established dark-palette path are the
shared vocabulary, not four local copies of each control.

UiIconRenderMode is Auto, MonoTint or PreserveColor. Icon ink falls back to normal
ink when no explicit icon color is supplied. PreserveColor is appropriate when an
image's colors carry content; mono action glyphs should use the state-aware tint.
A demo must check actual icons, not just the presence of a generic placeholder.

StyledShadow supports enabled, distance/offset, alpha/color, inset, hard/curve modes
and ShadowSoft/Tight/Linear/Gamma recipes. Margins include shadow geometry. Focus
and highlight use their existing metric contracts. Do not expose an override field
in PropertyEditor unless the actual paint path consumes it.

## Runtime mode changes and validation

Change UiTheme context through its API, update the host's native Light/Dark bridge
when needed, and refresh the complete shell and PropertyEditor palette. Do not call
SwapDarkLight blindly on every paint or on an unchanged mode. The canonical demo
shows the explicit host-level transition; reusable controls follow their theme.

For each themeable control inspect Minimal x four roles x Light/Dark, then Light ->
Dark -> Light without reconstruction. Exercise relevant normal/hot/pressed/disabled,
selected/focused/read-only states, small sizes and representative DPI. Validate
explicit custom styles, ClearCustomStyle, None/transparent faces, image colors,
icon contrast and live theme revision. Other declared presets must remain buildable
and receive regression checks proportional to changed common code.

A numeric palette inequality or successful Paint call is not visual acceptance.
Native pixel tests can protect concrete seams; human review still owns readability
and affordance judgments. Record gaps in the release inventory/ACTIVE_WORK rather
than marking every role PASS because SetRole compiles.


## File: references/upstream/docs/03_UI_MODEL_GUIDE.md

# 03 — Models and Data

Models remove duplicated semantic state and support shared views. A programmer
should not need a separate model object for a simple control. PropertyEditor
schema, adapters and editing transactions are in its [guide](05_UI_PROPERTY_EDITOR_GUIDE.md).

## The active-model contract

`Model()` always returns the model currently driving a genuine model-backed
control. Each such control owns an internal model by default:

```cpp
UiList list;
list.Model().Add("Apple");
list.Model().Add("Banana");
```

External binding is a non-owning switch, not a data copy or merge:

```cpp
UiListModel fruit; // outlives the active binding
UiList list;
list.SetModel(fruit);
list.Model().Add("Apple", 100);
ASSERT(&list.Model() == &fruit);
list.UseInternalModel(); // retained internal data is still present
```

The shared vocabulary is Model (const/non-const), SetModel, UseInternalModel,
IsUsingInternalModel and ClearModel. ClearModel clears the **currently active**
model; it does not switch to the internal model. Model changes publish the
notification that updates bound views; no RefreshFromModel call is necessary.

| Control | Model | Identity |
| --- | --- | --- |
| UiList / UiGallery / UiDropdown | UiListModel | sequential index; optional stable application data key |
| UiTree | UiTreeModel | stable node reference |
| UiTable | UiTableModel | row/column coordinate and range |
| UiMenu | UiMenuModel | stable menu node and command semantics |
| UiNodeGraph | UiGraphModel | graph IDs/references |
| UiDoc | UiDocCore | document positions/anchors and transaction mapping |

Sharing ownership vocabulary does not force every domain into UiModelItem.
There is no separate widget-only List/Tree/Table family: the internal model
already supplies that experience. Small bounded value controls such as
UiMatrixSelector, UiColorMatrix and UiRangeSegments need no extra model object.
UiAccordion is a real-child composition, not a hidden list-view alternative.

## Authority and lifetime

Semantic records belong to the active model. Views own viewport, hover, focus,
selection visuals, gestures, transient editors and derived presentation. Shared
renderers own only prepared content inside the rectangle the view gives them.
Do not keep another item collection in a control merely to synchronize it.

External models must outlive active use. Bound mutations run on the GUI thread
unless the host provides synchronization. Rebinding reconciles interaction state
without clearing/copying either dataset. Notifications from inactive models are
ignored. Weak observer identity distinguishes a fresh model from an old object
that occupied the same address. See the coding guide for Ctrl parenting, which
does not by itself transfer C++ ownership.

## Request-first user mutation

A control first computes and emits intent. Rejected means no change; handled
means the host performed/scheduled the edit; unhandled plus enabled internal
mutation permits the control to edit the active model. Model notification then
updates every bound view. Observations are not authorization.

Examples: UiReorderRequest, UiTreeMoveRequest, UiMenuActionRequest,
UiTableEditRequest and Graph move/connect/delete/route requests. Simple local use
may EnableInternalMutation(true). Command-driven hosts disable it and implement
the corresponding When...Request. Do not mutate first and ask permission later.

UiDoc's positional transaction model is already authoritative; a richer generic
request interception layer for all user edits remains a separate future policy,
not an implemented List-style request contract. It must preserve UiDocCoreTransaction.

## Mutable records and notification scope

Publish mutable record edits through the model's mutation/Touch API:

```cpp
// Examples from the relevant model families:
// list.Model().Touch(first, count);
// tree.Model().Touch(node);
// table.Model().TouchCell(row, column);
// table.Model().TouchHeader(axis, index);
// menu.Model().Touch(node);
// graph.Model().TouchNode(node_id);
// graph.Model().TouchEdge(edge_id);
```

Prefer one truthful ranged/bulk event per semantic batch. Presentation-only edits
do not justify rebuilding an entire projection. Guard feedback loops rather than
recursively mutating a property from its own notification.

Do not use display labels as application identity. Sequential views remap indices
on insertion/removal/move; application data keys can restore identity after a full
reset. Tree/Menu/Graph keep their native stable references. Table remains coordinate
based unless its public model is deliberately changed. UiDoc uses position maps.

## Scale boundary

Logical size is independent from live Ctrl/renderer count. List/Gallery/Table use
visible-range arithmetic; Tree may retain a visible hierarchy projection. Normal
scroll/paint/hit work is bounded to useful visible/overscan content. Explicit
Select All/export/filter/structural rebuild can legitimately be O(N). Model switches
must not copy N records just to display them. Images are prepared for visible
items and signalled through bounded model notifications, not eagerly decoded for
all records. See [Drawing and Performance](07_UI_DRAWING_GUIDE.md).

## UiDoc: document-specific contract

UiDocCore owns positional text, sparse style runs, blocks, annotations, resources,
embeds/inline images, tables, anchors/metadata, revisioned transactions, position
maps, undo/redo and import/export. UiDoc is its Ctrl view, with independent caret,
selection, viewport, active object and paragraph/layout caches. Several views can
share one model without copying the document.

UiDocCoreTransaction, UiDocApplyResult and UiDocPositionMap express actual
positional edits. A committed transaction remaps each active view's transient
state and invalidates deleted active objects. History depth is model policy;
changing a theme must never change shared document history.

Agents edit against an expected revision, apply a bounded transaction, inspect
its result and allow bound views to consume it. Pixel positions are disposable
view geometry, not durable document identity. Resources remain model/provider
semantics rather than a second editor-owned store. Preserve the current sparse,
paragraph-cached/viewport-driven implementation; do not allocate a Ctrl per character.

The existing engine is not U++ RichText. A **future**, separately authorized
extraction may separate UiDocView (layout/geometry/mapping), UiDocRenderer (painting)
and UiDocEditSession (caret/semantic commands) from UiDoc's platform input/focus/
clipboard/capture host. Those names describe direction, not shipped replacement
APIs. Extract from proven implementation without inventing another document model,
then validate the full document suite before using it in a Timeline or other dense
view. Do not build a second compact document engine or one live UiDoc per card.

## PropertyEditor: specialized schema, not application authority

PropertyEditorModel stores typed property values, defaults, mixed/inherited state,
help/group/unit metadata, validation and refresh-impact information. It emits
WhenStructureChanged, WhenValueChanged, WhenPreview, WhenCommit, WhenReset and
WhenGroupMetadataChanged. The visual editor owns editing lifetime and delegates
application commands/undo to the host. Core stores custom adapter/provider IDs
without importing concrete GUI or domain implementations.

Use one active semantic collection for a demo Data page. PropertyEditor rows may
project that collection; they must not become a competing editable collection
with a separate synchronization protocol. Clear ownership is more important than
making unlike domain model classes inherit the same base.


## File: references/upstream/docs/04_UI_DEMO_GUIDE.md

# 04 — Demo Guide

A demo is executable documentation for a control, not another application
framework. `examples/UiLabelDemo` is the canonical shell/reference. Full demos
use real Ui controls and the production PropertyEditor, remain self-contained,
and generate useful C++ that uses the public control API.

## Keep the construction visible

No mandatory DemoBase or shared DemoFramework. A small amount of repeated header,
exit and layout setup is preferable to hiding the example behind another layer.
Share production controls, styles, drawing and PropertyEditor adapters; do not
invent a convenience abstraction merely to remove harmless shell duplication.
Demos never depend on UiDesigner or another demo executable.

A substantial demo normally has a class header, implementation, tiny main.cpp and
.upp. Separate Inspector/Overrides/Code implementation only when that improves
navigation; do not split a small example into dozens of fragments. Suitable method
names are BuildHeader, BuildPreview, BuildRightRail, BuildInspectorModel,
BuildOverrideModel, ConfigureEditors, ConnectEvents, ApplyProjection, ApplyTheme
and UpdateGeneratedCode. Source comments explain those responsibilities and state
ownership, not every assignment. New names follow the coding-guide prefixes.

## Shell and page contract

Use a UiTitleCard with the control name and a useful one-line purpose. Keep
Light/Dark, Help and Exit actions compact, correctly labeled/tipped and consistent
with UiLabelDemo. Each action has its own appropriate icon; verify actual tint,
contrast, sizing and selection rather than merely checking that SetIcon is called.

Provide a generous live preview of the actual control. Size/layout, content,
interaction and state must be inspectable; do not shrink it to fit more properties.
An ordinary demo is not a selectable mini-Designer canvas.

The right rail contains Inspector / Theme Overrides / optional Data / Code.
One page occupies the available rail at a time. Selection is persistent page
state, not a hover/focus trick. Hidden pages must not be re-shown by a flow-layout
pass or reserve blank space. Resizing, page changes and theme changes keep the
chosen page and editor state usable. Ordinary controls do not show an empty Data
page. A Data page is appropriate for real model/domain collections.

## Inspector and PropertyEditor

Inspector rows map to meaningful supported public APIs: content/value, roles,
fonts, enabled/read-only states, selection, orientation, geometry, ranges/steps,
icons/media and control-specific behavior. Use the production factory/adapters
rather than hand-built property rows. Prefer spatial matrices to text dropdowns
for direction/side/position; use suitable range, color/palette, font, icon, image,
curve and compound editors. Expose only values the preview can actually apply.

Property models own authored configuration. The preview and code generator read
that same configuration; a short local projection is fine, a second competing
mutable configuration store is not. On an ordinary property edit, update values
and the affected preview rather than clearing/rebuilding the entire inspector.
Preserve selection/filter/scroll/expanded state. Rebuild schema only when the set
of properties genuinely changes, and guard queued work against stale selection.

A model-backed Data page operates on the **same active production model** as the
preview: List/Gallery/Dropdown -> UiListModel, Tree -> UiTreeModel, Table ->
UiTableModel, Menu -> UiMenuModel. Graph/Doc may retain specialized authoring tools;
do not flatten their data into generic row mirrors.

Providers, domain interpretation, persistence and application commands stay in
the demo/host. Do not add demo-only meaning to PropertyEditorCore or the factory.
See [PropertyEditor](05_UI_PROPERTY_EDITOR_GUIDE.md) for adapter and transaction details.

## Theme and authored overrides

Minimal Standard/Subtle/Accent/Alert in Light/Dark is the common visual baseline.
Preserve family-specific roles and data colors. Apply the complete shell/panel/
PropertyEditor palette when switching modes, following the reference's U++ theme
bridge; changing only the preview is not a complete demo theme change.

Inactive style rows inherit the current theme; active rows apply only the local
recipe the control supports. Explicit None is not the same as inheritance. Reset
restores current theme behavior, not a stale resolved snapshot. Some control APIs
own a complete custom-style snapshot: project active authored fields onto a newly
resolved base when the demo intentionally offers per-field inheritance.

Expose real palette states and supported metrics/skins/focus/shadows only. Match
actual runtime nouns and nesting: Face/Skin, Frame, Ink, Icon, Typography, Content
Margin, Focus, Shadow, Highlight and real subparts such as Track/Thumb or Popup.
Do not present an unused style field just because it is in a struct. Preview and
generated C++ must express the same authored behavior.

## Generated C++ is part of acceptance

Generate readable usage code, not the whole demo shell. With no overrides, use a
concise role/theme-based example. With active overrides, emit those authored
settings and any necessary public paint-hook wiring. Include required headers,
resources and object lifetimes; no unexplained demo helper dependencies.

A generator must escape arbitrary text, preserve numeric precision and use the
selected concrete control type. It must distinguish authored settings from current
interaction state. Resource/provider and application callback code that cannot be
serialized must be explicitly identified as host-supplied, not silently omitted
under a claim of complete reproduction.

Where useful offer Usage / Current changes / Full explicit recipes. This does not
require destabilizing a clear existing Code page simply to add another selector.
Paint examples use production Draw/Painter/UiShapes APIs, never private demo-only
shape tessellation or a parallel allocator.

The acceptance sequence is: configure preview, generate output through the actual
generator, compile it **unchanged** in a minimal declared-dependency package, and
check equivalent configuration/behavior. A hand-written lookalike is not generated-
code evidence. Cover default, changed values, explicit style overrides and supported
paint hooks. Reset/inheritance, special text and each family type also matter.

## Family demos and retirement

Combine controls only when they share a genuine production/style/teaching foundation.
The existing UiEditDemo covers Line/Password/Mask/Multi-line; UiIntFloatDemo is the
numeric companion. Slider/RangeSlider can share their existing family demonstration.
Do not create a third edit-family demo or put every widget into an enormous switch.

When changing a family type, show only applicable properties, preserve/restore its
own authored state deliberately and generate that type's actual C++. Common shell
code stays explicit within the package. Grouping should make reading easier, not
hide each control behind a generic adapter class.

Old demos are removed only after the replacement retains their useful examples,
public behavior, generated-code options and a working compile/run path. File age,
shorter code or a newer toolbar is not proof of replacement. Benchmarks, behavioral
regressions and specialized authoring tools are not redundant merely because a
canonical control demo exists. Track disposition in the release inventory rather
than accumulating separate migration reports.

## Focused manual acceptance

Open each canonical demo, exercise every page and relevant control interaction,
change role and Light/Dark/Light, resize, and close normally. Check icon identity,
readable contrast, all displayed property effects, reset/inheritance, disabled and
focus behavior, AA corners, and at least representative DPI settings. For model
views edit/reorder data without a second collection; for edit families test partial
input and cancellation. Confirm generated output through the compile gate above.

An idle demo has no unintended repeating repaint/diagnostics loop; intentional
caret/animation is bounded and stops correctly. Build success is not visual PASS.
Preserve the exact tested SHA/toolchain and state what remains untested. Gary's
surgical compile smoke is an implementation checkpoint, not the entire release gate.


## File: references/upstream/docs/05_UI_PROPERTY_EDITOR_GUIDE.md

# 05 — PropertyEditor Guide

The reusable property system has a headless model (`Utilities/PropertyEditorCore`)
and a Ui-backed visual editor (`Utilities/PropertyEditor`). Neither depends on
UiDesigner, SymbolPicker or a demo executable. The host owns application semantics,
commands/undo, resource providers and persistence.

## Boundaries and lifetime

PropertyEditorModel describes typed values, defaults, ranges, validation, groups,
help/units, mixed/inherited state, visibility, indentation/row spans and refresh
impact. Core may store opaque custom-editor/provider identifiers without depending
on their concrete GUI implementation. It is useful to GUI, CLI, agent and tests.

PropertyEditor owns row/filter rendering, selection, inline/popup editor lifetime,
viewport-limited creation and preview/commit/cancel interaction. PropertyEditorFactory
is the kind/adapter factory; do not introduce a second competing factory or duplicate
advanced-editor implementations. Bind a model with SetModel(&model); detach with
SetModel(nullptr) before destroying/replacing the borrowed model as required by
its lifetime contract. Replacing the model intentionally resets active editing.

Production callers normally use RegisterPropertyEditorEditors to install the
complete standard adapters. The existing V1 registration entry remains compatible;
do not remove it without sweeping its actual callers.

## Schema and values

Choose a property type/adapter for what a value means, not just its storage type.
Ordinary schema supports text/multiline, integer/double, Boolean, choice, color,
palettes, fill recipes, paths, sliders, vectors, curves, read-only and custom values.
Semantic adapters cover date/time/date-time, duration, point/size/rect, linked
four-sided insets/radii, flags, ordered strings, gradients, key chords, optional
nullable values and application-owned references/resources.

Default/range/unit/help and visibility/read-only/enablement belong to the property
model. Unsupported metadata must not pretend to change the preview. Mixed means
multiple differing authored values; it is not a corrupt sentinel value. Optional
Null, inherited/theme value, resettable default and explicit None are separate
states. Never display U++'s null double sentinel as a real large negative number.

Vector2/Vector3 are numeric ValueArrays. Generic curves are ValueArrays of two-
number points; the Bezier adapter uses four scalars [x1,y1,x2,y2]. Use the existing
AddBezierCurve helpers: x is normalized time, while the declared y range can permit
overshoot. Duration stores canonical seconds even when displaying ms/s/min/h;
changing the display unit alone must not emit a false value commit.

Gradient recipes preserve ordered stops, alpha, angle and interpolation. Date/time
uses the production local-value UiDateTime policy; do not invent a time-zone contract
inside the editor. Incomplete floating mantissa/exponent input stays editor-local
until valid; signed scientific notation remains accepted.

## Preview, commit, reset and cancellation

WhenPreview is temporary editing; WhenCommit is a durable authored change.
WhenCancel restores the edit origin, including mixed/inherited state where needed.
WhenReset expresses the reset operation. WhenUndoRequest delegates to host history;
the generic editor does not own a second application undo stack.

Callbacks may synchronously rebuild the inspector. Snapshot in-flight preview/
commit callbacks where a preview rebuild could clear a later commit; guard lifetime
before subsequent access. Prevent redundant same-property reconstruction while an
inline editor is committing, particularly when a modal picker is involved. A
successful picker opening is not permission to lose the committed callback.

Ordinary edits should refresh dependent values/summaries without destroying the
entire model. Preserve selection, filter, scroll and expanded rows. A genuine
schema/type change may reconstruct under a guarded selection/page/revision and
restore relevant state. Delayed work must not overwrite a newer selection.

## Inherited and local overrides

An inactive override shows inherited state. Editing its value first requests local
override activation through WhenOverride(true), then commits the authored value;
the host owns that transition. Reset returns to inheritance where the host schema
says so. Passive inherited markers are not the same action as ordinary Reset.
A fallback commit path must preserve activation even when a value editor's usual
mouse path is bypassed.

A control's SetCustomStyle often owns a full snapshot. A per-field Overrides page
therefore projects active authored fields onto a newly resolved theme base. Do not
freeze resolved values in inactive rows or conflate explicit None with Use theme.
Generated C++ and the preview must read the same authored recipe.

## First-class adapter choices

Use the existing helpers/factory for Matrix, Range, Adjustable Range, Color,
Color Palette, Fill Recipe, Icon, Font, Image, Curve/Bezier and numeric slider/text
editing. Spatial side/position choices should use UiMatrixSelector when clearer
than a textual dropdown. Cardinal4 excludes illegal center/diagonal choices.

Range and adjustable-range use the real slider/edit controls. Adjustable values
remain ordered lower-bound/lower-selection/upper-selection/upper-bound. Numeric
slider/text presentation is editing UI, not another semantic value. Boolean Check,
OnOff and TrueFalse are supported alternatives, not custom handwritten rows.

Icon/font catalog enumeration is lazy and shared across editor instances. Image
thumbnails are provider-driven, compact and aspect-preserving. Color palettes use
UiColorPicker's complete one-to-eight-slot contract; all slots synchronize, not
only the active chip. One-color callers explicitly request one slot on first open.

Core stores opaque custom_editor/editor_variant/picker_provider identifiers.
The visual factory supplies concrete implementations. Resource/reference providers
remain host-owned; the editor cannot infer application domain identity from a label.
RegisterPicker callbacks receive the current Value and owner Ctrl and return an
accepted result; RegisterThumbnailProvider supplies display resources. Do not add
hard dependencies on a particular asset browser, Designer or application dialog.

## Override page grammar

Use the control's actual runtime nouns and supported nesting:

- General and semantic role;
- Face/Skin, Frame, Ink, Icon, Typography, Content Margin;
- Focus, Shadow and Highlight;
- real subparts such as Track/Thumb/Popup/Selected only where supported.

Frame width/color/visibility are one coherent group; do not duplicate them under
several names. General styling changes do not silently become semantic data edits.
Only expose fields the control's real render/layout path consumes and generated
code can express. The presence of a member in an old Style struct is not proof
that every custom paint path honors it.

PropertyEditor should demonstrate useful standard controls/adapters, not a generic
bag of all properties. Domain-backed Data pages edit the same production model as
the preview. Graph and document authoring may use dedicated transactions instead
of pretending to be ordinary scalar rows. See the Models and Demo guides.

## Geometry, appearance and scale

SetRowSpan and SetExpandedRowSpan describe compact/expanded capacity. Expandable
Matrix, Curve, Image and Multiline editors use PropertyEditor-owned expansion
state and compact action rails. Rich editors may expand inline or use a provider
picker; the host chooses meaningful presentation rather than creating another
layout implementation per property.

Inline editor creation is viewport/overscan bounded, not one Ctrl per property.
GetInlineEditorCount and stress fixtures make this observable. Layout computes
row/column/action/editor rectangles and reusable summaries; Paint does not create
all editors or aggregate entire groups. Guard model/active-editor replacement and
preserve the actual selected/expanded property through ordinary value changes.

SetPaletteMode can follow UiTheme or select Light/Dark. Complete mode switching
refreshes editor surfaces, filter, rows and glyphs, not just the previewed control.
PropertyEditorStyle owns action imagery (reset/expand/collapse/dialog/browse),
filter spacing, nesting/indentation and label divider geometry. The divider gives
resize feedback; rich inline controls receive usable space. Color chips remain
crisp and actual colors, not a role-tinted substitute.

## Validation and documentation

Retain the existing PropertyEditorTests, V1/Semantic suites, OverrideCommit,
SortOrder, WorkingRange and CoreProbe coverage. PropertyEditorDemo is the capability
reference; SemanticDemo is a focused semantic matrix, not automatically obsolete.
Core must still compile without CtrlCore/CtrlLib/Ui. Visual changes need native
edit/cancel/rebuild, picker-slot, inherited activation, mixed/partial numeric,
filter/scroll and viewport-pool checks. Exercise repeated Light/Dark transitions.

The release inventory is a coverage register, not automatic acceptance. Compile
actual generated demo output unchanged in a minimal dependency package, including
active overrides/resources. Record source review separately from native execution
and visual usability. A build-only PASS does not prove commit ordering, glyph
readability, picker negotiation or a 1,000-row editor population bound.


## File: references/upstream/docs/07_UI_DRAWING_GUIDE.md

# 07 — Drawing, Geometry and Performance

Canonical rendering and scale rules for `upp_Ui`. Geometry defines where content
belongs; raster policy defines how pixels are produced/reused. Keep those concerns
separate. Graph-specific retained execution is in [Graph Development](09_UIGRAPH_DEVELOPMENT.md).

## Choose the cheapest correct representation

1. Direct Draw for cheap rectangles, straight lines, text and images.
2. Native Painter curves for smooth circles, ellipses, rounded rectangles and paths.
3. Shared exact raster caching for stable repeated AA/composed presentation.
4. Bounded live Painter/BufferPainter for changing vector content that cannot
   truthfully reuse a cached raster.
5. Explicit fallback for rich skins, shadows, image fills and unusual paths.

Do not route every control through a full-size BufferPainter for uniformity.
Conversely, painting aliased direct-Draw curves over an AA background does not
preserve smooth edges. RangeSegments uses a shared rounded content clip so even
very narrow end segments cannot leak square corners. Only its content strip and
thumbs are rasterized, not readout whitespace; unchanged strips/thumbs are cached
and live drag work bypasses position-cache pollution. Straight internal partitions
remain sharp and text stays direct Draw.

Bounds and invalidation must be correct before introducing caching. A cache cannot
repair a dirty-region bug or turn an unbounded workload into bounded work.

## Final-device-pixel geometry contract

Generated explicit geometry uses one library-owned flattened-centreline positional
budget: **0.35 final device pixels**, within the numeric/work envelope reported by
UiGeometry::TessellationStatus::IsExactContract(). This is not a blanket guarantee
of pixel-identical stroking, joins, caps, clipping or antialiasing across backends.
Integer Draw conversion makes one final nearest-pixel rounding (up to roughly
0.707 px Euclidean displacement); later raster semantics are a separate seam.

Apply authored units, DPI and camera/view transforms before deciding curve detail.
Apply DPI once. UiGeometry/UiShapePath/UiShapes accept final pixel-space values;
there is no hidden global DPI setting. VisibleExtentPx is presentation significance
policy, not a theorem that every subpixel feature has zero coverage.

No fixed 20/40/100-point circles, radius*2 subdivision, per-control sample count or
private quality slider. Native Painter curves stay native when no explicit points
are needed. Semantic labels/handles/anchors use parameters, analytic intersections
or arc length, not `vertices[count/2]`, because adaptive point counts may change.

## Responsibility stack

| Layer | Responsibility |
| --- | --- |
| UiGeometry | final-pixel math, containment, lengths/distances, adaptive explicit geometry |
| UiShapePath | authored Move/Line/Quadratic/Cubic/Arc/EllipseArc/Close topology |
| UiShapes | reusable parameterized silhouettes |
| UiDraw | Draw/Painter seam, appearance, fills, shadows, raster/cache policy |
| Control | semantic state, layout/hit policy, interaction and visible content |

This is a responsibility stack, not a mandatory call chain. Normal controls use
native primitives or stock UiShapes where appropriate. Dense Graph scenes may
use UiGeometry directly rather than allocate authored path commands per item.
Both paths obey the same final-pixel rule.

Stock silhouettes include Polygon/RoundedPolygon, Rectangle/RoundedRectangle/
Capsule/Ellipse, regular N-gons, stars, arrows, chevrons, chamfers, callouts, tags
with holes, cloud/document/database, RingSegment and Pie. Add a generally useful
silhouette to UiShapes; a genuinely private shape can be a local UiShapePath.

```cpp
UiShapePath shape = UiShapes::RoundedRectangle(Rectf(0, 0, width, height), radius);
p.Begin();
UiPainterShapePath(p, shape);
p.Fill(face);
p.End();
```

UiPainterShapePath forwards supported circular Arc and cubic commands natively.
Its authored elliptical-arc path uses UiGeometry flattening where no verified
direct Painter command is available. Flatten only for a consumer that needs
explicit points: hit testing, routing, clipping, retained geometry or a backend seam.

Authored polygon vertices are semantic topology. Do not silently simplify them.
Generated-polyline simplification needs a declared combined budget: flattening at
0.35 and independently simplifying at 0.35 is not a 0.35 end-to-end guarantee.

## Circular controls and clipping

UiProgressRing/UiChartRing use native stroked arcs through UiPaintCircularArc.
Filled wedges/donut sections use UiShapes::Pie/RingSegment. A complete RingSegment
has opposite-winding outer/inner contours so a stroke does not reveal a fake radial
bridge; ArcBandPath's single bridged contour is a fill-oriented helper.

Paint and hit testing share the appropriate prepared shape/capacity. Keep state
balanced and valid for zero/tiny rectangles, large dimensions, reversal and both
orientations. Corner clipping must cover all participating layers, not just the
first and last item. Test seams, fractional AA coverage, explicit None, alpha and
selected outlines against actual pixels where deterministic.

## Raster lifetime and cache policy

Audit temporary buffers, masks, blur, gradients, asset decoding, 9-slice composition,
cache keys and image lifetime separately from curve complexity. Stable repeated
AA work is a cache candidate only when all pixel-affecting inputs form an exact
key, size/memory are bounded and reuse beats rerasterization. Include resolved
colors/state, dimensions, radius/stroke, fill/skin/asset revisions and any other
consumed input. Do not scale exact cached edges from a quantized bucket unless
that approximation is explicitly part of the policy.

Use transparent premultiplied buffers correctly; clear newly allocated buffers.
A cache admission failure needs a bounded correct fallback, not an unbounded image
allocation under another name. Remember retained Image handles may keep memory
alive outside the cache's entry budget. Arbitrarily unique per-item images/styles
are not free merely because the cache has a size limit.

## Measurement and invalidation

GetMinSize, GetContentSize, width-aware measurement, Layout, paint and hit testing
must agree on the same geometry vocabulary. Expensive text/image preparation moves
to the narrowest appropriate invalidation seam. A small ordinary control may have
cheap measurement; high-scale views must not remeasure every record on every Paint.

Geometry changes invalidate layout and paint. Color-only changes invalidate paint.
A global theme revision refreshes inherited styles; explicit custom styles stay
explicit. No model mutation, event emission, loading or timer startup inside Paint.
Do not add a second per-item layout cache when prepared geometry can own the result.

## Large views: logical size is not live visual size

The semantic model feeds visible/overscan/spatial candidates, bounded prepared
presentation and then Paint/HitTest. A 100,000-row model must not create 100,000
controls or renderer objects. List/Gallery/Table use direct arithmetic for regular
layouts; Tree may retain a flattened visible projection; irregular Graph uses a
retained broad phase. Introduce a new spatial tree only when measured workloads
justify replacing the current strategy.

UiItemRenderData carries presentation, not universal domain semantics. UiItemRender
is a lightweight non-Ctrl renderer inside a rectangle assigned by its view. Pools
are bounded to useful visible/overscan surfaces; renderer Layout prepares data,
Paint/HitTest consume it. Tree disclosure, Table headers/editing, Menu commands,
Dropdown selection and Graph ports/routes remain with their own views.

A local record/appearance update should not rebuild uniform grid geometry or an
entire hierarchy/spatial projection. Structural changes may rebuild the structure
they invalidate. Explicit Select All, export, filtering and structural rebuild can
be O(N); ordinary scrolling, hover and hit testing must not silently become O(N).
Use bulk/ranged model notifications for a semantic batch. Do not duplicate the
model to solve a rendering problem.

Prepare expensive assets at the visible-range seam with stable keys and bounded
providers/caches. A transient actual editor is an explicit sparse escape hatch,
not one child Ctrl per ordinary logical item. Model replacement reconciles that
editor and view identity; inactive model notifications must not revive old state.

## Three distinct LOD questions

Population LOD chooses which objects need presentation. Presentation LOD chooses
which information is useful at projected size. Geometry LOD chooses explicit curve
detail at final pixels. None changes semantic topology or authored values.

Identity survives simplification: a diamond stays a diamond and a connector stays
attached to the same endpoints. Rich details/shadows/secondary text can disappear
before primary identity; a proxy is not fabricated readable content or a tiny
working editor. Thresholds and actual projected footprint are different inputs.

A camera change need not rebuild a compatible retained scene. Project from one
immutable exact baseline, not repeatedly rounded output. Unsafe coverage/capacity/
representation/LOD changes require exact fallback; quiet may trigger one exact
settle. Public SetZoom/SetPan/Fit remain exact unless explicitly documented.
Graph's named-component scale-reuse boundary remains documented separately; generic
camera principles are not proof that that path already reuses every wheel frame.

Dirty-region paint and hit testing use the same broad-phase authority. Query
intersecting candidates and then exact-test; do not add a second full-viewport scan.
For huge marquee previews, deferring expensive preview work until release may be
appropriate without changing committed semantic selection.

## Evidence, idle behavior and future backends

A static control with no animation/mutation/invalidation should settle idle.
Caret blinking and deliberate animation are exceptions with explicit lifecycle.
Before optimizing Paint, find any unwanted Refresh/timer loop. Disabled diagnostics
return before allocating strings, scanning data or scheduling another callback.

Prefer structural evidence: candidates/prepared/painted counts, layout/build serials,
renderer/active-control counts, cache hits/misses and path vertices. Timings are
machine/workload-specific evidence, not a portable FPS assertion. Record cold/warm
conditions, DPI, compiler, dataset and complete input-event cost, not only Paint.
Run only the relevant performance path for a changed subsystem.

No hard OpenGL/Vulkan/upp_render dependency is required by this library. A future
backend should consume the same retained semantic/presentation seams rather than
force controls into another model. Source simplification or fewer files is never
proof of runtime speed.


## File: references/upstream/docs/08_UIGRAPH_GUIDE.md

# 08 — UiGraph Guide

UiGraph is a generic graph model/editor/view. It owns topology, graph editing,
view state and presentation. Application execution, scheduling, budgets, retries
and AgentFlow behavior stay with the host. Internal layout/performance and workspace
contracts are in [Graph Development](09_UIGRAPH_DEVELOPMENT.md).

## Model and view

UiGraphModel owns scopes, nodes, ports, edges, backdrops, subgraph interfaces and
graph-document metadata. UiNodeGraph owns active binding/scope, camera, selection,
gestures, spatial/projection state, retained presentation, LOD and active embedded
controls. It owns an internal model unless an external one is bound:

```cpp
UiNodeGraph graph;
graph.Model();
// graph.SetModel(external_model); // borrowed; must outlive its active binding
// graph.UseInternalModel();      // original internal data remains
```

SetModel switches without copying/merging/clearing. Scope/model changes cancel
incompatible gestures and reconcile selection/attached controls so reused IDs
cannot inherit another graph's transient state. Model notifications update the
view; mutable node/edge edits must publish through TouchNode/TouchEdge or the
appropriate mutation API. See [Models](03_UI_MODEL_GUIDE.md).

## Shapes and actual controls

Eight canonical built-ins: Rectangle, Ellipse, Diamond, Triangle, Hexagon, Cloud,
Document, Database; Custom is the callback extension. Equal Rectangle dimensions
make a square, radius/aspect make a capsule, equal Ellipse dimensions make a circle.
Historical enum values remain compatible; do not multiply shape types for sizes.

Ordinary nodes/ports are painted geometry in one UiNodeGraph, not child Ctrl trees.
SetNodeCtrl is the sparse explicit escape hatch. Registration and activation are
separate: offscreen/out-of-scope/LOD-suppressed bindings can stay registered, but
only prepared, visible, useful-size controls attach. GetRegisteredNodeCtrlCount,
GetActiveNodeCtrlCount and GetLastNodeCtrlCandidateCount distinguish those costs.
Host code owns real-control lifetime; a painted Actions component is not a tiny
live button/editor.

## Shared templates and node content

Register a validated C++ UiGraphNodeTemplate once per class and use its style class
on nodes. Do not construct templates or parse authoring JSON per node paint.

```cpp
String error;
UiGraphNodeTemplate layout;
// Configure the shared layout using UiGraphNodeTemplate's public API.
if(!graph.SetNodeTemplateClass("asset", layout, error))
    Panic(~error);
UiGraphNode node;
node.style_class = "asset";
graph.Model().AddNode(node);
```

Identified components have stable nonempty IDs, not identity derived from slot
order. Text/Icon/Image/Progress/Fields/Tags/Actions are bounded painted kinds;
repeated Text or Icon components need no Title2/Icon2 enum. Bindings resolve node
fields, node data or explicit resources. Use IDs again after reordering rather
than retaining an array index. Legacy unnamed/rich hooks remain supported within
their documented bounds but do not run as a Micro fallback.

The structure is optional Header / Body / Footer. Body has independent Content
and Overlay layers, each with Left/Main/Right regions. Overlay paints last and
never consumes Content space. Ports remain graph-owned reservations. Width, height,
placement, alignment, component style and semantic binding are separate concepts.

Normal / LOD 1 / LOD 2 / LOD 3 are author-facing inclusion levels; representation
(Text/Bar/Dot/Hidden, etc.) also depends on real capacity, data readiness and budget.
On requests a legal representation, not unreadable text or escape from the shape.
Micro/Rich execution is a separate decision. The workspace displays the actual
representation/reason alongside authored inclusion.

## Camera, grid and size

Programmatic SetZoom, SetPan, PanBy and Fit remain exact. Compatible live pan may
project retained geometry; live zoom has explicit admission and exact fallback.
Named-component scale reuse is not generally enabled yet. See the development
guide before making performance claims about that path.

The hierarchical grid is world-origin aligned: fine levels fade as coarser levels
become useful. Grid presentation never changes authored grid size or snapping.
World coordinates have no arbitrary global cap; an inspector's bounded scrub range
is user-interface policy, not a topology limit.

Changing LOD thresholds with UiRangeSegments changes presentation policy, not camera
zoom or world-space node size. Fit, 1:1 and LOD-jump camera actions are explicit.
The existing collapsed flag suppresses body content/port labels; a future size-only
disclosure must not silently reinterpret it.

## Edges and interaction

Built-in routes are Straight, Bezier and Orthogonal, with Custom as an extension.
Route detail is adaptive in final pixels. Orthogonal stock lead is zero unless the
host authors a positive lead. Endpoint markers are None, Open, Triangle, Tee,
Square, Circle and Diamond; their wire values stay stable/append-only.

Route edits use UiGraphEdgeRouteRequest. Near-direct straight waypoints normalize
to direct routes; Bezier midpoint movement respects useful port-forward half-planes;
orthogonal corridor editing retains its existing orientation/hysteresis policy.
Midpoint/label handles use visible arc length, not a tessellation vertex index.

Selection is semantic, separate from ordinary frame styling. Clicking an already
selected member can preserve the group during drag; plain release can collapse the
selection. Modifier add/toggle/subtract and marquee preview/commit are explicit.
Application-owned mutations use the request-first policy rather than mutating before
asking permission. Port glyphs, edge arrows and semantic port anchors are distinct.

## Backdrops and hierarchy

A Backdrop is same-scope presentation organization, painted behind content. It does
not own nodes/edges, change topology or prevent objects crossing its bounds.

A Subgraph owns one child scope. Each node belongs to one scope; ordinary edges
connect endpoints within that scope; nested child positions are local. Scope cycles
are rejected. The parent presents the subgraph as an ordinary group node with an
authoritative stable input/output interface mirrored as normal outer ports.

Inside, Group Inputs exposes external inputs as internal outputs; Group Outputs
accepts values for external outputs. Parent edges never connect directly to child-
internal nodes. Interface changes preserve IDs/metadata/multiplicity and reject
self-containment. Enter/Exit changes the view's active scope, not topology.

## Examples and boundaries

UiGraphDemo remains the general graph/10k reference; UiGraphHierarchyDemo teaches
scope hierarchy. UiGraphComponentStudio is the current single-preview family/node
workspace with real PropertyEditor, diagrams, structure/LOD table, files and C++.
DesignMatrix is retired, not another active editor.

Workspace files are authoring interchange, not runtime layout programs. Base layout
and appearance inherit independently into eight shapes. Explicit detach creates a
section snapshot. Inherited sections are read-only; selecting a shape alone does
not author an override. The generated C++ has no workspace/PropertyEditor dependency.

Use node data/provider hooks for application tags, thumbnails and status rather
than universal graph fields. Declare extension paint/hit bounds before painting
outside stock geometry. Read the development guide for exact size/budget, source,
file schema, projection and validation contracts. ACTIVE_WORK is the current
publication/platform boundary, not a promise that every proposed workspace feature
or optimization is already complete.


## File: references/upstream/docs/09_UIGRAPH_DEVELOPMENT.md

# 09 — UiGraph Development

Current retained execution and node-workspace contracts. This consolidates earlier
layout/component/workspace/audit/checkpoint notes. Superseded V3/V4 four-preview
instructions and old validation narratives remain in Git history, not a competing
work queue. ACTIVE_WORK records current publication and Windows validation.

## One prepared authority

NodeGeometry.presentation is the evaluated per-node layout consumed by paint,
picking, attached controls and compatible camera projection. Shared template
recipes are registered/owned once. No second layout-result cache, runtime JSON
compiler, ordinary per-node Ctrl tree or workspace-only allocator is permitted.
An immutable exact camera baseline may copy prepared records: one semantic/layout
authority does not mean only one physical copy of all bytes.

UiNodeGraph.cpp includes its internal .inc implementation parts exactly once.
The suffix preserves shared helper linkage and the existing translation-unit
boundary; it is not a second backend or a reason for a cosmetic rewrite.

| Responsibility | Source |
| --- | --- |
| styles/lifetime/notifications/attached controls | UiNodeGraphCore.inc |
| exact geometry/anchors/preparation | UiNodeGraphGeometry.inc |
| retained allocation and components | UiNodeGraphPresentation.inc |
| world queries/scope filtering | UiNodeGraphSpatial.cpp |
| public camera/view batches | UiNodeGraphCamera.inc |
| compatible projection/exact settle | UiNodeGraphProjection.inc |
| hierarchy/fit/selection/backdrops | UiNodeGraphHierarchy.inc |
| model authority switch | UiNodeGraphModelBinding.inc |
| paint orchestration | UiNodeGraphPaint.inc |
| backend admission/Micro drawing | UiNodeGraphPaintMicro.inc |
| rich drawing/ports/routes | UiNodeGraphPaintRich.inc |
| common projected-size/backend policy | UiNodeGraphLod.h |
| gestures/capture/cancellation | UiNodeGraphInteraction.cpp |
| templates and component resolution/paint | UiGraphNodeTemplate.*, UiGraphNodeComponent* |

Keep the sole scope-aware world spatial hash. Query useful candidates for view,
dirty rectangle, point hit or marquee, then exact-test. Local model updates rebuild
affected nodes/routes where safe; structural/scope/model changes rebuild what they
invalidate. Do not restore historical alternate spatial sources or renamed-method
copies to fix a regression.

## Bounds, projection and backend admission

ExtensionBounds declares final-device-pixel node paint/hit margins, dynamic port
hit radius/edge hit width and edge-overlay paint margin. Custom route escape margin
is world-space so the world broad phase stays valid across camera changes. Update
bounds before paint/hit and invalidate through the existing public path.

ProjectLiveView checks semantic change, coverage, LOD/representation and other
compatibility before reuse. It projects from the immutable exact baseline, never
from repeatedly rounded output. The baseline is captured lazily after admission;
rejected frames must not deep-copy records only to rebuild them immediately.
Programmatic camera methods stay exact; quiet may settle an admitted live frame.

**Named-component wheel scale reuse remains disabled.** Compatible pan is supported,
but that does not certify wheel/hover/update performance. Removing the admission
guard requires a separate measured correctness change for capacity, font/image
representation, Micro budgets and activation. Keep exact fallback; do not bolt on
another per-node cache to avoid understanding admission.

Camera reuse and Micro/Rich drawing admission are different decisions. Complete
backend preflight before drawing so Painter-only edges do not disappear after a
partial frame. Mixed-size scenes can contain Micro and rich nodes at the same zoom;
Micro nodes still omit rich content even when the scene needs the rich backend.
GetLastPaintPath/GetLastPaintFallbackReason describe actual execution, not merely
configured thresholds. Semantic editing may legitimately choose rich paint.

## Template/component contract

UiGraphNodeTemplate is a fixed-capacity ordered shared description (at most 16
slots). Nonempty unique component IDs identify independent bindings after reorder.
Registration validates and owns a copy; an invalid candidate preserves the previous
class. Empty/absent class follows the fallback rules. Do not retain caller pointers
to mutable descriptions. Evaluation currently may revalidate registered entries;
registration alone is not proof that every per-node schema check was eliminated.

Identified kinds: Text, Icon, Image, Progress, Fields, Tags, Actions. Legacy unnamed
slots retain their established rich-content path. Registered templates provide the
native Micro route; rich resolver callbacks never become a Micro escape hatch.
Text roles share a renderer; repeated kinds use IDs, not duplicate semantic enums.

Bindings are node fields/data or explicit authored/static resources. Text supports
literal/field/data sources, Icon supports node/data/static imagery, Image preserves
its fit policy, Progress requires a finite normalized value, Fields/Tags/Actions
use bounded group data. Missing data is not a fabricated placeholder. Local font
face/height/weight flags inherit when unset; preparation resolves effective values.

Placement allocates Left/Right/Top/Bottom/Fill slots in explicit order; alignment
positions content inside the result. Reserve lanes before Fill. On/Off/Inherit
belongs to each Normal/LOD1/LOD2/LOD3 policy, not the preceding LOD. Stable keeps an
excluded slot's reservation; Reflow releases it. A visible proxy retains allocated
capacity. Representation is not another editable LOD band.

## Capacity, readable text and native Micro

On cannot bypass missing data, legal shape capacity, readable minima or drawing
budgets. Prepared results distinguish NoSpace, TooSmall and other suppression
reasons. Single-line Ellipsis text retains its projected font when it fits; otherwise
a bounded search (at most 16 height probes) may find a readable smaller line without
crossing readable_min_px. Width still ellipsizes. Clip/Wrap keep their own contracts.
Protect later readable minima in the same region before flexible earlier rows take
space; enlarging a preceding Subtitle must not unnecessarily starve Title Fill.
A genuinely overfull header remains a capacity failure, not permission to lower
floors, enlarge the node or falsify LOD inclusion. Micro exits before rich font fit.

Reduced text is a bounded measured-ink footprint bar/dot, not fake readable content
or one permanent pixel per letter. Icon and image proxies preserve only the meaning
they can express. Prepare thumbnails/overview resources outside paint; do not decode
or resample images merely because Micro painting asks for them. Actions are painted
cues; actual controls remain sparse SetNodeCtrl bindings with useful-size minima.

Native Micro hints are opt-in with a 0..16 primitive budget per node. A bar/dot costs
one; a small mosaic can cost four. Collision/budget rejection has an explicit reason.
Do not require a previous rich frame for direct-to-overview parity. Dynamic image
hints require ready bounded overview data (up to 4x4) or a defined fallback. Ordinary
node outline/topology is separate from the optional hint count.

The hint paint budget is **not** a total allocation/preparation budget. Template
lookup, source summaries, containment and records still cost work; font-name lookup
and retained decoration images need measurement. No per-node timer, arbitrary group
scan, rich callback, Ctrl activation or full text fitting is admitted to Micro.

## Content, Overlay, ports and ellipse bands

Body contains sibling Content and Overlay layers with independent Left/Main/Right
cursors. Overlay paints after Content regardless of template order. Adding/removing,
hiding, moving, sizing or reordering Overlay components must not change underlying
Content rectangles or image rasters. A true adjacent sidebar belongs to ContentRight;
OverlayRight is an upper layer, not width subtracted from ContentMain.

Image allocation and painted footprint differ. Contain preserves the whole image
with possible gaps; Cover fills its existing allocation by aspect-preserving crop.
New Media families use Thumbnail Cover plus right-aligned State in OverlayRight so
superposition is visible. Generic images still default Contain. Existing saved
families are not silently migrated to new defaults.

The Overlay diagram shows a subdued labeled Content footprint beneath its guides,
clipped from the actual retained/painted result. Contain gaps stay empty; hidden
Content creates no footprint. These guides are not another image renderer/allocator,
retained cache, Content selection target or drop surface.

Optional ellipse_bands fits independent Header/Footer rectangles for identified
non-Micro Ellipse/Circle components. ellipse_band_width_percent is 20..100 of the
conservative band width, not node width. Preserve authored heights; narrow/move a
band outward only when the **entire rectangle** fits the silhouette and respects
port reservations. A failed fit keeps that band's conservative location. safe stays
contained and is never inflated; Body can reclaim free space inside safe between
bands. Other shapes and physical Micro retain conservative capacity.

NodeComponentClip governs production paint and preview hits: valid independent
Header/Footer bands use their own region; other components/synthetic snapshots use
safe. Legacy hooks remain conservative. Top/bottom labeled reservations block the
corresponding band movement; side reservations remain protected. Anchors, IDs and
connections do not change because label rectangles change.

**Open V8 boundary:** current left/right Body-only booleans and top/bottom full-safe
reservation are not the complete four-side Body only / Full edge model. In that
proposed model both Content and Overlay must share the same post-port interior.
Existing body-side lanes can overlap the column/layer allocation. Do not advertise
an equivalent new mode with a preview-only toggle. Resolve production allocation,
anchor distribution, all sides, persistence and export together in a separate task.

## Workspace ownership and transactions

Utilities/UiGraphWorkspace owns authoring documents, strict JSON, C++ export and
validated document transactions. UiGraphComponentStudio owns dialogs, selection,
undo and PropertyEditor lifetime. UiNodeGraph alone owns real layout/rendering.
The workspace is an authoring application, not code that runs per production node.

One named family has Base layout and Base appearance plus eight independently
optional shape layout/appearance snapshots. Null means inherit, not a hidden clone.
Base is an edit scope, not a ninth silhouette. Shape selection does not create an
override. Inherited sections stay read-only until explicit detach; reset affects
only that section; clone is independent; Copy-to-all requires explicit confirmation.
Component-local style belongs to layout; node appearance inherits independently.

PlaceComponent validates revision, scope, identity, target, insertion and legal
capacity before committing. Move preserves data/style/ID. New Icons default Left;
new text defaults Top; insertion precedes the first Fill. Imported/moved placements
are not rewritten. Accepted authoring does not prove drawable capacity. Explicitly
confirm unreserved-region creation; reject port-lane drops without redirection.

Diagrams/table/preview share selection IDs. Visible footprints win preview picking,
then a small slot/shape-clipped tolerance for thin proxies. Hidden items remain
available in the table, not invented as visible preview pixels. Diagram inventory
for every unallocated/hidden component is still an open usability item.

Delete routes to the component command only when structure/diagram/preview has
focus; text/filter/PropertyEditor keeps its own Delete semantics. Preview deletion
must not invoke graph-topology deletion. Undo and inherited-scope rejection remain
transactional. Palette DND disarms button click state before a modal drag. LOD cells
do not initiate component moves; horizontal structure scrolling preserves LOD3 hits.

## Workspace interface and persistence

Current native UI: left family/shape/scope/preview-data/palette; central thresholds,
region/overlay diagrams and **one** production preview; wide hierarchy/placement/
LOD table; right Inspector/Template/Style/C++ pages. V8 HTML is current design input,
not proof of every native feature or a production API/schema.

Ordinary commits refresh dependent values without destroying the PropertyEditor
model/selection/filter/scroll/expanded state. A genuine renderer-kind/schema change
may rebuild under guarded revision/page/selection and reselect the relevant row.
Typography sits near identity; summaries show actual representation and cause.
Code owns the full available rail and cannot share height with a hidden editor.
Page selection is persistent state, not hover/focus styling.

Threshold edits never move the camera or resize the node. LOD jump buttons explicitly
move the camera; manual camera edits clear jump selection. Fit/1:1 are explicit.
Preview state is saved but not exported into production templates. Existing collapsed
behavior must not be silently repurposed as size-only disclosure.

Schema: `uigraph.workspace`, writer version **2**, units `logical96`. Layout,
appearance and preview are separate sections. V2 requires Boolean ellipse_bands
and integer ellipse_band_width_percent (20..100) in Base/non-null shape layouts.
V1 imports conservatively (bands false, width80); only explicit save emits v2.
Unknown versions/fields, invalid types/enums/ranges, duplicate IDs/keys and excessive
nesting reject the candidate before replacing live state. Maximum JSON is 8 MiB;
portable premultiplied RGBA resources are bounded to 256x256. HTML mockup JSON is
not this schema and must not be accepted through lossy fallback.

Save uses a temporary sibling and atomic replacement. Failure preserves previous
file and dirty state. Layout and style factories, shape fallback, registration and
node configuration are generated from the same validated family. C++ uses actual
Ui APIs with no runtime JSON/authoring dependency; preview data/ports/camera/size
stay out. Register factories once; re-register theme-relative styles when the host
changes theme. Node corner_radius remains the silhouette radius authority.

## Performance evidence and unresolved optimization

The old 10k legacy PanProfile is not a named-component benchmark. UiGraphScaleTests
--components compares matched 10k nodes/9,900 row edges, eight shapes, compact/card
sizes, legacy/registered passes and near/mid/overview views. Input-event and Paint
timing, rebuild/population/hint/backend counts are separate. Nine samples give only
a coarse diagnostic p95 (the maximum), not a strong statistical estimate. Cold cache,
image-heavy/dense-edge/arbitrary-host workloads are not covered by that fixture.
No-argument suites and --pan-profile remain separate; invalid arguments fail.

Open costs include registered-template revalidation, named-component wheel fallback,
Micro preparation/record/baseline memory, up to four decoration states, hover rebuilds
and low-zoom model-update fallback. These are measured follow-ups, not authorization
to redesign geometry during release documentation cleanup. Keep 03C lazy-baseline
capture and existing structural tests. Never equate passing counters with a timing
PASS or source/file reduction with speed.

GraphDemo observation uses one replaceable 200 ms callback. Normal status updates
regardless of diagnostics visibility; sampling checks enabled/visible at execution.
After running, no repeating observer remains. Batch activity/model feeds; do not
execute an agent engine in Paint or schedule one timer per node.

## Validation and remaining direction

Retain Graph Model/View/Render/Scale, Workspace, standalone regressions and the
existing ValidateUiGraphWorkspace.ps1 runner. Its accumulated native gate covers
render suites, strict authoring/export, unchanged generated C++ compiled with Ui/
CtrlLib only, view/band/startup summaries. The current OVERLAY summary must also be
positive with failed=0; older source PASS does not cover newly added overlay work.
Read ACTIVE_WORK for the exact tested/published boundary, not old hardcoded SHAs.

Manual checks: fresh Media on Rectangle/Ellipse; both readable text rows and two
icons; disabled/enabled independent bands; selection in outer bands; inherited
edit rejection; Delete/Undo without topology/text-editor interference; Code full
rail/page state; State overlay hide/move/width/delete leaves image rect/raster
unchanged; Contain versus Cover persistence; no Content drop targets in Overlay.
Held-button Escape and broader physical DND remain separately unverified until
actually exercised. Preserve real failures and logs; do not weaken tests/floors.

A future size-only disclosure, complete V8 port interior, unallocated-component
inventory and finer invalidation/scale reuse are distinct bounded tasks. No general
constraint solver, fine-grained dependency engine, four-copy template model, rich-
Micro fallback, Timeline-specific graph engine or GPU dependency is implied by
this guide. Current production contracts take precedence over historical sketches.


## File: references/upstream/provenance.json

{
  "source_repository": "upp_Ui",
  "version_header": "#ifndef _Ui_UiVersion_h_\n#define _Ui_UiVersion_h_\n\n/*\n    Ui release identity\n    ===================\n    Author: C Edwards (dodobar)\n    License: Apache License 2.0 (see LICENSE).\n\n    One version for the Ui library, not a separate counter for each control.\n    An RC identifier does not certify platform/visual acceptance. Independently\n    versioned sibling packages and serialized-data schemas retain their versions.\n    No GUI dependency; safe to include in build and diagnostic tools.\n*/\n#define UPP_UI_VERSION \"1.0.0-rc.1\"\n\nnamespace Upp {\ninline const char* UiGetVersion() { return UPP_UI_VERSION; }\n}\n\n#endif\n",
  "sha256": {
    "docs/00_UPP_CODING_GUIDE.md": "932785d260725823cfa43ba98f549c5fe0926b9eda1481e573591399e3e8c363",
    "docs/01_UI_CONTROLS_GUIDE.md": "469763042d916d504845bcf666c1598fd3c487fae1109c0f702cd7dbcbefafe2",
    "docs/02_UI_THEME_GUIDE.md": "0ac2043c4e3dd63aa6a29ffaabdb8c0827579961708db1b787311783bf8c9c64",
    "docs/03_UI_MODEL_GUIDE.md": "da1717e78c9d6e37d136fd91100dc55a3aced7c79a988e54d3307989c6851b35",
    "docs/04_UI_DEMO_GUIDE.md": "6eefd1394a15e73ff5001cd440f5f7945a52c41143af832dc74d13aacf96ca4a",
    "docs/05_UI_PROPERTY_EDITOR_GUIDE.md": "41178df6207e01cfdb08055954500a55c6915d8a40d742e0f3d2fd7cb035af45",
    "docs/07_UI_DRAWING_GUIDE.md": "2bfb4747b025b946a6b4e752fffe223f8c7cc75e7861270c8970f5168f3f253e",
    "docs/08_UIGRAPH_GUIDE.md": "016950d2baa5b49f946079092f240cd5ccf74520e0347fcf731e4bace374b951",
    "docs/09_UIGRAPH_DEVELOPMENT.md": "0cbf3cf80f53b8a5ec3292b72d22c63b3488902b6d3f38717cbda4370187a69d",
    "LICENSE": "1eb85fc97224598dad1852b5d6483bbcf0aa8608790dcc657a5a2a761ae9c8c6",
    "GETTING_STARTED.md": "58f04fe448fa583e82acc62c02bd5e24b519b21e4fc4662553247686374d84a0"
  },
  "note": "Working-tree snapshots; headers and examples are resolved in the target checkout."
}

