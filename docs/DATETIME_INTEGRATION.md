# UiDateTime Designer integration — UID-DATETIME-01

## Authority and supported surface

DateTime is a native UiDateTime adapter, not another calendar/clock implementation.
The document stores one `datetime_value` plus `minimum_value` and `maximum_value`.
Each is a local ISO `YYYY-MM-DDTHH:MM:SS` string or null. There is no timezone
conversion, epoch guessing, locale-dependent saved text or duplicate node.data model.
A shared headless decoder validates calendar fields before constructing U++ Time.
Catalog validation rejects malformed values and null with allow_null=false before
code generation/export. This does not add a new general pre-commit schema engine:
external callers using generic commands must still validate the resulting document.

Configuration covers Date/Time/DateTime mode, Locale/ISO display, clock format,
seconds, bounds, nullable policy, first weekday, picker-button role, editable versus
presentation mode, presentation frame, and copy/paste policy. All are document-owned,
not Theme overrides. WhenChanging, WhenAction and WhenOpenPicker expose the real
zero-argument events. The payload-bearing WhenInvalid event and per-control language
selection are not advertised in this tranche.

The deterministic initial/reset value is 2000-01-01T12:00:00. A saved null remains
null. Preview and generated initialization explicitly replace the reusable control's
construction-time clock value; they never invoke SetNow to repair authored data.
A human explicitly opening an empty picker retains UiDateTime's normal picker behaviour.

Mode and seconds are projections: switching to Date or hiding seconds does not
rewrite the saved full value. Switching back restores hidden fields. A deliberate
value edit writes the native control's mode-normalized result. Range ordering and
clamping remain UiDateTime responsibilities. Nullable policy is applied after the
saved value so allow_null=false cannot silently insert the current system time.

## Inspector and Data

The existing scalar binding points to datetime_value. Both panes use the same
`designer.date-time.value` editor, backed by real UiDateTime. Bounds reuse it without
imposing the main value's range or null policy. Its untouched result preserves hidden
source fields rather than rewriting them during focus/refresh. Native picker actions
commit normally. Typed input also has an explicit Apply button because the current
reusable control can omit WhenAction when its live value already matches the text.
No private child callbacks or second command history are introduced.

Normal PropertySpec gains opt-in `preserve_null` (default false). This makes explicit
null survive Inspector/Data projection and spec copying without changing existing
controls' fallback semantics. DateTime's value and bounds opt in.

The editor registers through an Editors package initialization unit; the existing
application link already includes Editors. No application-shell/local dark-theme
file is changed, and headless Services does not acquire a GUI dependency.

## Preview, Theme and export

Preview creates the actual UiDateTime. Configuration edits rebuild only its subtree
so mode, seconds, bounds and value are reapplied from canonical state in a stable order.
All DateTime configuration/value properties share one ordered catalog group.

The date_time Theme adapter exposes editable face/text/frame colours, frameless
presentation text colours, and font height/radius/frame width. Unpainted frameless
face/frame channels are not advertised. It resolves native role defaults,
reuses normal recipe/local-override precedence and leaves values/configuration alone.
Read-only surfaces use the selected Theme, not the OS paper fallback. There is no
new global palette synthesis and no added permanent Theme Studio gallery tile.

Code generation emits native Time(...) / Time(Null) initialization, with mode,
seconds and range preceding value and null policy following it. Generated applications
link Ui, not Designer or editor packages, and require no runtime theme.json or ISO parser.
Complete and ComponentOnly fixture verifiers inspect actual native values and styles.

## Evidence boundary

The remote implementation is source-reviewed; Windows Debug/Release, real picker
interaction and combined acceptance with the senior's published dark-theme changes
remain pending. The pre-existing closure at 7a01c6a passed its automated gates.
Curt reports the ProgressRing manual interaction/visual inspection passed.
That does not complete the separate Theme-role isolation or full Preview/export
visual comparison. The nine dark-theme/title files were published at a602b446 while this work was
prepared. This checkpoint is based on that tip and leaves those files untouched.

## Next action

Run tests/DateTimeIntegrationTest/BuildGeneratedFixture.ps1 -DebugBuild, then Release,
then the combined Designer acceptance including the published dark-theme slice.
Use the same dependency/toolchain recorded for the previous accepted run when starting;
record any newer dependency revision used for final acceptance. No graph-project gate.

Remaining control integrations: UiColorMatrix, UiMatrixSelector and UiGallery.
Agent runtime/UI remains a separate later checkpoint, not part of this change.
