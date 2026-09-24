# UID-ASSISTANT-FIRSTUSE-02 and TitleCard/Grid repair

Designer base/required ancestor: `bb50fb84fd295844a29246651ea3b5d9b708069f`.
Shared Ui base: `9f1cf55481db2b5039a9fe3bd66daee91acce726`.
Both main worktrees were clean at intake; fetched remote histories had no advancement.
No branches, resets or history rewrites were used. Newer drawer work was preserved.
Shared Ui fix: `ed635bc1dcbbd0d23524d1184843995c239c0963` (pushed, fetched, contained in origin/main).
Designer tested source is the implementation commit containing this report; the
following publication record identifies its SHA. Tests ran on those identical
source contents before committing, with the shared changes present.

## Actual first-use failure

Exact prompt, unchanged for baseline and acceptance:

> Create a simple dialog box template with just an OK and cancel perhaps a with a heading that I can use as a template.

Configured provider/model: OpenRouter / `deepseek/deepseek-v4-flash`.
The credential remains outside project files in `OPENROUTER_API_KEY`.
Only a disposable blank design was sent. No credentials or private reasoning
were recorded in diagnostics.

Baseline live trace (`build/Firstuse-before.stdout.log`):

| Round | Executed tools | Used after round |
|---|---|---:|
| 1 | inspect_context, retrieve_skill, search_controls twice, list_presets: all OK | 5 |
| 2 | inspect_hierarchy, describe_control(UiButton): OK | 7 |
| 3 | search_controls three times: OK; describe_control(UiColumn): ERROR, unknown type | 11 |
| 4 | describe_control(UiPanel), describe_control(UiLabel), search_controls twice: OK | 15 |
| 5 | Requested two calls; entire batch blocked before execution | 15 |

The cause was inefficient broad discovery (seven searches) and an invented
UiColumn type, encouraged by generic layout guidance without concrete registered
types or a valid composition example. It was not an authentication failure.
The 16-call boundary correctly rejected 15 + 2; increasing it was not the repair.

`layout-v2` supplies a schema-valid heading/OK/Cancel example and the real
UiBoxLayout V/H names. `describe_controls` retrieves at most four relevant schemas
through the canonical automation service. Guidance prioritizes matching presets
or bounded relevant discovery, then one proposal. The six-round/16-call limits
remain unchanged. Activity reports rounds, names and results. Limit errors report
used/requested/limit/remaining and that the batch was not run. A pending valid
proposal survives a subsequent read-only failure. Human Apply, captured targets,
canonical validation and command/history ownership are unchanged.

After-fix automated live trace (`build/Firstuse-after.stdout.log`): round 1
retrieve_skill OK; round 2 describe_controls and list_presets OK; round 3
prepare_composition OK; round 4 final response. Four calls total, seven checks
passed: valid proposal with heading and OK/Cancel, no model mutation, trusted
Apply adds one history entry, one Undo restores the exact blank authored document.
An additional visible drawer run exposed identity-field retries: retrieve_skill
and inspect_nodes in round 1; describe_controls in round 2; prepare_composition
ERROR in rounds 3, 4 and 5; prepare_composition OK in round 6 (seven calls total).
It reached the six-round bound before a closing response. The valid proposal
remained pending and the canvas stayed blank. Clicking Apply created a heading,
body label and OK/Cancel buttons; one Ctrl+Z restored the blank canvas/hierarchy.
This is successful proposal/Apply/Undo acceptance, not a successful closing
provider response. The final correction explicitly excludes Designer-assigned
identity fields and identifies rejected control/property names in validation
errors. A deterministic regression rejects UiBoxLayout.name without a proposal.

Final separate live run after that correction (`build/Firstuse-live-final.stdout.log`):
round 1 retrieve_skill + inspect_context OK; round 2 describe_controls OK; round 3
prepare_composition OK; round 4 final response. Four calls, no retries, all seven
checks PASS, process exit 0. This run followed both final offline gates.

Final visible drawer run on canonical Release PID 404912 also completed without
errors: round 1 retrieve_skill + list_presets, round 2 describe_controls, round 3
prepare_composition, round 4 final response (four calls). Before Apply the canvas
and hierarchy remained blank and the proposal was pending. Clicking Apply showed
Dialog heading, OK and Cancel; the receipt reported one commit. One Ctrl+Z restored
the blank canvas/hierarchy. The application remains open for Curt after Undo.

## TitleCard and Grid causes

The TitleCard adapter discarded the resolved role when no local overrides existed,
restoring Standard. It now retains the resolved role in preview and exported code.
Medium was absent from the inspector/parser/export mapping; both menus now expose
it. Medium measures the title text; Large horizontal lines reserve the hosted
content-cell lane and its configured gap. Small remains the short accent.

Grid natural measurement distributed tracks against a zero-sized viewport,
collapsing Fit. Natural measurement now uses an unconstrained dimension. Fixed
dimensions are applied independently and only when that axis is Fixed, including
wrapped children. Generated code now retains item sizing/alignment/min/max.
The existing repeated-click ancestor selection and hierarchy already allow a
covered Grid to be selected; the regression verifies that selection stack.
See [TitleCard/Grid usage](TITLECARD_GRID_BEHAVIOR.md) for the three-row setup.

## Offline validation

Requested commands both passed:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\RunAssistantValidation.ps1 -Configuration Debug -Scope Embedded
powershell -NoProfile -ExecutionPolicy Bypass -File .\RunAssistantValidation.ps1 -Configuration Release -Scope Embedded -Launch
```

Final evidence: `build/Assistant-Debug-20260925-011442` and
`build/Assistant-Release-20260925-011501`.
Both: AppChatTests 23/0; AssistantDesignerTests 56/0; TitleGridRegressionTest
29/0; RegressionTests 85/0; ThemeDocumentTest 31/0; ThemeStudioRoleTest 380/0;
ExportedThemeContractTest 27/0; canonical Designer build PASS.
Coverage includes the failing 5/2/4/4/2 discovery sequence, whole-batch rejection,
exactly 16 permitted calls, useful diagnostics, schema-valid example, pending
proposal preservation, rendered line pixels, mixed Grid sizing and wrapped Fixed
dimensions. Earlier broader Release run `build/Assistant-Release-20260925-010149`
also passed Tests 1292/0, FoundationTests 73/0 and ThemeAdapterCoverageTest 9177/0;
that run preceded the final wrapped-Fixed edge-case correction covered by both
final standard runs.

## Touched files and application

Shared Ui: `Ui/UiGridLayout.cpp`, `Ui/UiTitleCard.cpp`.
Designer: `AppChat/AppChat.{h,cpp}`, `UiDesigner/Assistant/Skills.h`,
`UiDesigner/Assistant/UiDesignerAssistant.cpp`,
`UiDesigner/AssistantUi/UiDesignerAssistantDrawer.cpp`,
`UiDesigner/Catalog/UiDesignerBuiltins.cpp`, `UiDesigner/Core/UiDesignerSizing.h`,
`UiDesigner/Preview/UiDesignerPreview.cpp`, `UiDesigner/CodeGen/UiDesignerCodeGen.cpp`,
`UiDesigner/Theme/UiDesignerThemeAdapter.cpp`, `RunAssistantValidation.ps1`,
`tests/{AppChatTests,AssistantDesignerTests,AssistantLiveTest}/main.cpp`,
new `tests/TitleGridRegressionTest/{main.cpp,TitleGridRegressionTest.upp}`,
`docs/{ACTIVE_WORK,ASSISTANT_USAGE,TITLECARD_GRID_BEHAVIOR,ASSISTANT_FIRSTUSE_02_REPORT}.md`.

Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
SHA256: `2B3E99B83F20C77D3820D7DD3E70C81B96C37BAF7B269D00E3734199D209AB69`.
Final Release launch PID: `404912`.
No alternate Redesign executable/shortcut was found in the repository/apps file
search or standard Desktop search; no alternate was changed or deleted. No
unsaved user design was terminated. Only the untouched disposable blank earlier Release
launch was closed to allow the canonical Release rebuild. The subsequent
disposable live fixture was closed only after Undo restored the blank design.

The result is a visual dialog template. OK/Cancel behavior is not automatically
wired. Live model responses remain nondeterministic; bounded failures now explain
what happened and retain valid proposals for review. No MCP or new harness added.
