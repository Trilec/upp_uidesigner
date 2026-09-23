# UID-ASSISTANT-01 — embedded assistant checkpoint A

Classification: PASS for the embedded functional checkpoint. Optional live MCP
attachment (checkpoint B) is pending. Curt retains final visual/design judgement.

## Source and ownership

Designer started at `ec6c541d2040cbd1b390339aec1e661844f89980`; that commit was
already on origin/main when fetched, and is preserved. Implementation publication
is recorded in ACTIVE_WORK.md. No reset, rebase or force push was used.

Reusable Ui started at `297beabdea87e3cc2c32282968e262ef68392abb`. During this task
another workflow advanced that checkout to
`d728d1c37ac112efad5321e775364fbc041c3d3b`. Final Release validation uses that newer
commit. Its local `examples/UiGraphComponentStudio/WorkspaceWindow.cpp` edit was
inspected and preserved; it is outside the Designer build and this task. No Ui
repository files were edited here. Both required handover ancestors remain present.

Touched paths: `AppChat/`, `UiDesigner/Assistant/`, `UiDesigner/AssistantUi/`,
`UiDesigner/Commands/UiDesignerCommands.*`, `UiDesigner/Services/UiDesignerAutomation.cpp`,
`UiDesigner/Services/UiDesignerPresetPlacement.cpp`, `UiDesigner/Services/UiDesignerSession.*`,
`UiDesigner/ThemeCore/UiDesignerTheme.*`, `UiDesigner/UiDesigner/UiDesignerWindow.*`,
`UiDesigner/UiDesigner/UiDesigner.upp`, three assistant test packages,
`RunAssistantValidation.ps1`, and the architecture, usage, status and handover docs.

AppChat depends only on Core and Windows WinHTTP. The Designer host owns schema
tools and proposals; the native UI delegates to that host. Durable changes use
existing Document command history and separate Theme history. No AgentFlow runtime
or new reusable-control dependency is introduced. The model cannot call Apply,
save/export, arbitrary JSON replacement, files or a shell.

## Automated validation

All listed runs used `E:\upp-18468\umk.exe`, assembly `github`, method `CLANGx64`.
Every passing test exited zero and printed a positive check count and zero failures.

| Gate | Debug | Release |
| --- | --- | --- |
| AppChatTests | 19/0 | 19/0 |
| AssistantDesignerTests | 47/0 | 47/0 |
| RegressionTests | 85/0 | 85/0 |
| ThemeDocumentTest | 31/0 | 31/0 |
| ThemeStudioRoleTest | 380/0 | 380/0 |
| ExportedThemeContractTest | 27/0 | 27/0 |
| Canonical UiDesigner build | PASS | PASS |

Debug evidence: `build/Assistant-Debug-20260923-203134` (Ui dependency 297beab).
Final Release evidence: `build/Assistant-Release-20260923-212201` (Ui dependency d728d1c).
Build metadata records the baseline Designer SHA plus working-tree status because
validation preceded the implementation commit.

Broader integration run: `build/Assistant-Release-20260923-202335`:
Tests 1292/0, FoundationTests 73/0, ThemeAdapterCoverageTest 9177/0. These passed
before the final provider-only fixes. That run's new live-test build failed on a
missing GUI header; this was corrected and the final Release run passed. Earlier
failed network probes exposed a UTF-32/UTF-16 WinHTTP header conversion error and
an undersized SSE wire-byte limit; both are corrected and live-tested. Failed
attempt logs remain evidence, not passing results.

Focused coverage includes typed color/font edits, exact authored Undo/Redo,
atomic batches, stale document/Theme tokens, duplicate Apply, invalid fields and
parenting, semantic preset insertion, typed symbolic subtrees, cancellation,
malformed/partial streaming arguments, missing credentials, and drawer geometry.
Generated-code eligibility is checked; this task does not claim a separately
compiled exported executable for the assistant-created fixture.

## Live and native acceptance

Provider: OpenRouter. Model: `deepseek/deepseek-v4-flash`.
Credential reference: `OPENROUTER_API_KEY` in the Windows user environment.
Only its presence was inspected; its value was never printed or saved to the repo.
The test/application inherited it through their process environment.

AssistantLiveTest: 4/0 in the final Release evidence. It created a new disposable
button, invoked real inspection/schema tools, produced a validated proposal and
confirmed the provider did not mutate the document. No existing project was sent.

Native drawer acceptance was then exercised on the freshly launched Release app:

- Discussed a blank test design, retrieved layout guidance and listed presets.
- Prepared a SPA settings-shell proposal. The design stayed blank until the
  trusted GUI Apply click. Apply created the shell; Ctrl+Z removed it in one step;
  Ctrl+Y restored it.
- Inspected the shell and prepared a Courier New local font replacement scoped
  to its Save button. Apply visibly changed that button; Ctrl+Z restored it.
  Automated tests additionally verify size, bold and italic preservation.
- Cancelled an active long read-only response. The drawer displayed
  "Cancelled; unapplied proposals remain unapplied." Subsequent workspace
  navigation and editing remained responsive. An earlier short probe completed
  before Stop and is not counted as cancellation evidence.
- Switched Theme Studio's sample control role from Accent to Alert, returned to
  Designer and observed the authored shell unchanged. Separate automated gates
  cover preview/recipe/document ownership.
- Switched Light/Dark/Light with the drawer open; text and controls remained
  usable. Collapsed/reopened the drawer with conversation/proposal state intact.
  Earlier native checks also exercised draft preservation, Shift+Enter and resize.

These are functional checks performed by the implementation engineer through the
native UI, not a claim that Curt has supplied final aesthetic acceptance. The
runner's human-acceptance NOT RUN entry correctly describes its automated scope;
the subsequent native acceptance above supplies the separate evidence.

## Executable and remaining boundaries

Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
SHA-256: `91AECA38348F1B77B5F55A1468373552C75C48D972CD2C1E3BC0FA8E10512D9D`.
Launched PID: `383476`; left open with the disposable test shell and drawer.
Launch identity is also stored in the final Release `launch.json`.

Optional attached MCP is NOT IMPLEMENTED/NOT RUN. Existing standalone MCP is
unchanged; its existing foundation checks pass. Actual AgentFlow integration is
outside scope and is not claimed.

Profiles support explicit DeepSeek and OpenRouter endpoints; only the above
OpenRouter/model combination received live acceptance. Structured collection-data
editing remains unsupported and is reported as such. Transcript/proposal text uses
native UiMultiEdit's horizontal scrolling, without Markdown rendering or word
wrapping. Stop suppresses late dispatch immediately; a new request or close can
wait for the bounded network worker to finish cancellation. Conversation is
memory-only; no durable replay, shell access or background jobs exist.

Full source diff, package membership and declarations were reviewed. Whitespace
checks passed. Designer cleanliness and remote containment are verified after
publication; the unrelated reusable-Ui example edit remains user-owned.
