# Assistant conversation UI and reuse reference

2026-09-25. Base Designer `d95d50462872fa10aa2ae70bdb410ac9fdd60fd1`.
Shared Ui `ed635bc1dcbbd0d23524d1184843995c239c0963` unchanged.

## Result and boundaries

The lower drawer uses chronological, folding message and proposal cards, labelled
History, Clear all, read-only Show code, Refine, a bottom profile dialog and one
Assistant/Collapse toggle. Apply still uses captured targets and canonical
validation/history. Clearing discussion leaves authored design and Undo intact.
Completed proposals survive Stop or a later read-only failure. Stale proposals
show Needs review; applied cards are receipts, never historical Undo commands.

The reusable `AppChatUi` package owns presentation only and depends on Ui.
`examples/AppChatUiExample` compiles without Designer or a provider.
`REUSABLE_ASSISTANT_UI.md` explains integration and ownership. No TitleCard
extension was needed. Message text remains plain text; code review is proposal
JSON, not C++. Rich Markdown and in-place JSON editing are not implemented.

Explicit Label headings now override the optional TitleCard example in guidance.
Refinement of applied compositions supplies actual node IDs and requests edits
instead of reinserting the original tree. The existing bounded tool loop remains
16 calls/six rounds; validation expectations and human Apply are unchanged.

## Live evidence

`build/Conversation-live.log`: OpenRouter `deepseek/deepseek-v4-flash`, 26 checks,
zero failures. Credentials and private reasoning were not recorded.

| Request | Calls / rounds | Trace |
|---|---|---|
| Original exact first-use prompt | 5 / 5 | retrieve_skill, inspect_hierarchy, describe_controls, rejected prepare_composition, corrected prepare_composition |
| TitleCard and bottom actions | 5 / 4 | retrieve_skill, inspect_context, two describe_controls, prepare_composition |
| Explicit Label heading/body/actions | 5 / 5 | retrieve_skill, inspect_context, describe_controls, rejected prepare_composition, corrected prepare_composition |
| Rename existing heading | 3 / 3 | inspect_context, inspect_nodes, prepare_edits |

The original prompt tried `UiLabel.font_bold` as a configuration field; the Label
scenario tried unsupported `UiPanel.text`. Both were rejected, then corrected
within the existing budget. This is successful bounded recovery, not evidence
that the model always generates valid arguments first time.

Checks require valid heading/OK/Cancel proposals, no pre-Apply mutation, canonical
Apply and one-step Undo. The explicit Label case rejects TitleCard substitution.
Refinement checks unchanged node count and the same Label ID. Templates are visual;
OK/Cancel behavior must be configured separately.

Native drawer checks separately verified the Label template on a blank canvas,
Send/Stop state, read-only JSON window, labelled History navigation, Apply, Refine
to Account settings on the existing heading, and one-step Undo back to its previous
text. A second Undo restored blank. Clear semantics are covered by deterministic
host tests; a manual click on Clear all was not part of this native run.

## Touched areas

New AppChatUi package and standalone example; Designer assistant host/controller
and footer toggle; deterministic AssistantDesignerTests and live AssistantLiveTest;
architecture, usage, active-work, package index and reuse documentation.

## Final offline evidence and executable

Required Debug/Release Embedded commands passed on the final source tree:
`build/Assistant-Debug-20260925-140357` and
`build/Assistant-Release-20260925-140423` (with `-Launch`).
AppChat 23, AssistantDesigner 73, TitleGrid 37, Regression 85, ThemeDocument 31,
ThemeStudioRole 380 and ExportedThemeContract 27 checks; zero failures in each.
Standalone example build: `build/AppChatUiExample.build.log`.
These offline runs did not call the provider; the separate live evidence above did.

Canonical executable: `E:\apps\github\upp_uidesigner\build\UiDesigner.exe`.
SHA256: `2D13035C2A1CA345E305A1201FD29B5428D1CD992FE1AEE42347059EEAC738E4`.
Launched PID: `59524`. Left open for Curt on a blank design.
No alternate executable/shortcut was changed and no user-authored work was closed.
Publication SHA is reported with the task completion; this report is part of that
same implementation commit. Logs record the base SHA and dirty source inventory
because validation preceded the commit.
