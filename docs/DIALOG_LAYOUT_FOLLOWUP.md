# Dialog creation and direction-change follow-up

Follow-up to UID-ASSISTANT-FIRSTUSE-02, based on Designer
`43ef29aab3eb19437e33b6b558e671e1893b4bae` and shared Ui
`ed635bc1dcbbd0d23524d1184843995c239c0963`. Both were clean at intake;
origin/main was fetched before changes. No shared Ui changes are needed here.

## Reproduced defects and corrections

The screenshots show Box first in the Grid's child order, followed by Panel and
TitleCard, even though their rows are bottom/middle/top. Rebuilding Box after a
direction change removes its managed Grid item. Removal shifts later item
indices, but surviving preview instances retained the old indices. Projection
then assigned sibling rectangles using stale positions, putting TitleCard over
the buttons. The same bookkeeping applies to managed Box parents.

The regression now reproduces that insertion order and switches V/H four times.
Before repair: TitleGridRegressionTest 37 checks, four overlap failures. After
repair: all 37 pass. Surviving managed indices are updated on removal before the
rebuilt item is attached. This is a projection fix, not an authored hierarchy
rewrite. The prior test inserted TitleCard first and therefore missed the defect.

The prior assistant example only represented a simple label/buttons composition.
The requested TitleCard/body/footer variant now has a separately validated
example: three-row, one-column Grid; Fit TitleCard; Expand Panel; Fit horizontal
Box; Fill Spacer; Fit OK/Cancel. Guidance requires tool-prepared proposals for
creation, honors requested control types, and prefers layouts over absolute
coordinates. It is guidance and capability information, not model training.

Grid placement was not exposed by the composition tool. Row/column are placement
metadata, not ordinary TitleCard properties; passing them as properties fails
canonical validation. Optional paired item-level grid_row/grid_column now route
through the existing drop planner. The target must be Grid and coordinates must
be in range. No arbitrary document JSON mutation or weakened validation is added.

Typing `apply`, `apply it`, or `apply proposal` is now an explicit local human
command. Exactly one pending proposal applies through the existing host Apply
method; none produces a useful explanation; several require selecting a proposal
and using its button. The model has no Apply tool. Generation/revision/Theme
checks, one-step Undo and stale-target rejection remain intact. The status row
distinguishes a ready proposal from a prose-only reply with no canvas changes.

These changes do not establish what happened in the user's original assistant
conversation, whose transcript was not supplied. The layout defect and missing
composition placement were independently reproduced. The final live and visible
acceptance results are recorded below rather than inferred from those defects.

## Validation and publication

Deterministic tests cover missing/ambiguous/repeated text Apply, valid requested
structure, no mutation before approval, one Undo, invalid Grid coordinates and
the repeated direction-change overlap. Existing budget/safety tests remain.
Both original exact first-use and new TitleCard/bottom-buttons prompts are run
separately on blank designs against the configured OpenRouter profile.

The tool budget remains 16 calls and six provider rounds. This bounds execution;
no budget increase was used as a substitute for fixing capabilities or guidance.
The result remains a visual template; button actions are not automatically wired.

Touched areas: Preview managed-item detachment; Services composition insertion;
Assistant schemas, guidance and trusted Apply; AssistantUi composer/status;
TitleGridRegressionTest, AssistantDesignerTests, AssistantLiveTest; usage,
architecture, active-work and this report.

Required Debug and Release Embedded validations passed at
`build/Assistant-Debug-20260925-103712` and
`build/Assistant-Release-20260925-103827`: AppChat 23/0, AssistantDesigner 63/0,
TitleGrid 37/0, Regression 85/0, ThemeDocument 31/0, ThemeStudioRole 380/0,
ExportedThemeContract 27/0; canonical application build PASS in both.

Separate live test (`build/DialogFollowup-live.stdout.log`) passed 15/15 checks,
exit 0, OpenRouter `deepseek/deepseek-v4-flash`:

- Original exact prompt: round 1 retrieve_skill + inspect_hierarchy; round 2
  describe_controls twice; round 3 prepare_composition; round 4 final response.
- TitleCard/bottom-buttons prompt as written in the user's follow-up: round 1
  retrieve_skill + list_presets; round 2 describe_controls twice; round 3
  prepare_composition; round 4 final response.

All tools OK, five calls per request, no retries or limit failures. Both produced
valid proposals without mutation, applied once, and undid to the exact blank
authored document. The second additionally verified TitleCard, three-row Grid,
expanding Panel and bottom horizontal action row. Credentials/private reasoning
were not logged. This is bounded live evidence, not a guarantee for every request.

Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
SHA256: `8405578145BB82980A6D15D02F0D452C6C1012DD88CF86E07C034EAA7D3201AF`.
Release PID: `407132`. No user design was closed; the user had already closed it.

Visible native drawer acceptance also passed on that executable: the user's
TitleCard request produced a pending proposal while the canvas remained blank.
Typing `apply` then pressing Enter immediately created the TitleCard, middle
Panel and right-aligned bottom OK/Cancel buttons on the canvas, without another
provider request. The status read Applied and the receipt showed one commit.
One document Ctrl+Z restored the blank canvas/hierarchy; Ctrl+Y restored the
template, which is left open unsaved for Curt to inspect. This is separate from
the scripted driver's trusted Apply checks above.
