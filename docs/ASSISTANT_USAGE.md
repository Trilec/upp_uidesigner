# Native Designer assistant (UID-ASSISTANT-01)

Click **Assistant** in the footer. Drag its top edge to resize, or Collapse to
hide it. The draft, conversation and active request survive hiding. Enter sends;
Shift+Enter inserts a newline. Send becomes Stop during a request. Stop cancels
further work; already prepared proposals remain available for review.

Select DeepSeek or OpenRouter, enter an explicit tool-capable model ID, and enter
the name of a user environment variable holding that provider's API key:
`DEEPSEEK_API_KEY` or `OPENROUTER_API_KEY`. Never enter the key in the drawer.
Open **Profile** below the composer, then **Save profile**. Sending shares design context with
the selected service. Browser/Codex subscriptions are not API credentials.
Restart Designer after changing its inherited environment.

Only non-secret profile metadata is saved in the application configuration file
`uidesigner-assistant.json` (U++ ConfigFile directory). Nothing is written to a
project, skill, generated source or transcript by credential configuration.
Model availability and tool support depend on the provider; no default model is
hard-coded. Other purportedly compatible endpoints are not accepted or claimed
tested. TLS uses Windows trust validation; redirects are disabled.

The transcript is in memory. Conversations have one active request, bounded
rounds, calls, input and output. AppChatLimits defines those bounds. No background
jobs or filesystem/shell tools exist. Provider failures do not retry mutations.

The activity row and transcript show the current round, tool calls and success or
error results. Tool-limit failures report calls already used, the size of the
rejected batch and the limit; that entire batch is not executed. A previously
prepared proposal remains available for review after a later read-only failure.
The default is 24 calls and eight provider rounds, including room for validation
repairs. Activity shows the remaining budget. A failed reply offers Retry with fix;
this starts another bounded attempt without applying changes. A ready reply exposes
Apply beside Activity as well as on its Proposal card.

Creation works from a blank design. A prepared proposal must be committed with
the Apply button, or by typing exactly `apply`, `apply it`, or `apply proposal`
when only one proposal is pending. These text commands are local human actions;
the model still cannot apply. With several pending proposals, select one and use
its Apply button. The status row distinguishes a proposal ready to apply from a
prose-only response that created nothing.

For a TitleCard dialog, layout guidance includes a complete three-row Grid:
Fit TitleCard, expanding body Panel, and Fit horizontal action Box with a Fill
Spacer and Fit OK/Cancel buttons. Composition placement uses optional paired
`grid_row`/`grid_column` item fields through the normal insertion planner, not
arbitrary control properties. Invalid or occupied placement is rejected.

Simple dialog requests use the versioned `layout-v2` guidance and its validated
heading/OK/Cancel composition example. `describe_controls` reads at most four
relevant registered schemas together. A visual template does not wire Accept or
Cancel behavior; configure that separately in the Behavior Inspector.

Try these requests on a disposable design:

- “Inspect this design and suggest improvements without changing it.”
- “Replace mapped fonts on the captured selection with an installed family.
  Preserve size, bold and italic; prepare local overrides.”
- “List the presets and propose a suitable application shell under the root.”
- “Suggest a settings dialog using supported presets and properties.”
- “Inspect the active Theme recipe and propose typography for that recipe.”

Proposals appear as cards in the chronological discussion. **History** navigates
to a proposal without applying it. **Show code** opens read-only Proposal JSON,
not generated C++. **Refine** lets you request changes to a pending draft or to
the existing affected controls after Apply. Explicit Label requests use a Label
heading, rather than the optional TitleCard example.

**Clear all** clears discussion, drafts and proposal records while retaining the
design and normal Undo. **Select affected** selects captured nodes. **Apply** commits once,
**Dismiss** prevents application. Apply is unavailable during a request.
Undo/Redo uses the normal Document or Theme history. A receipt does not offer an
unsafe “undo this old card” action. Manual edits, Theme edits and document switches
invalidate proposals; the assistant must prepare a new one. Theme and Document
proposals are separate groups with separate undo histories.

The compiled skill index is `UiDesigner/Assistant/Skills.h`; retrieval works from
any working directory. The catalog and adapter schemas are capability truth.
Structured collection-data edits are not
yet exposed; use the Data editor. Typed subtree proposals use registered controls, symbolic parent references, canonical placement and one Document undo entry. Studio sample
roles remain controlled by the existing Studio toolbar, independently of durable
recipes and document structure. Unsupported field types return an explicit error.

## Validation

Run from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\RunAssistantValidation.ps1 -Configuration Debug -Scope Embedded
powershell -NoProfile -ExecutionPolicy Bypass -File .\RunAssistantValidation.ps1 -Configuration Release -Scope Embedded -Launch
```

Logs and source metadata go under `build/Assistant-*`. The canonical application
is `build/UiDesigner.exe`. Offline tests do not contact providers. Live acceptance
requires a configured provider, real inspection/proposal and human Apply/Undo;
it is not implied by scripted tests. MCP attachment is a separate optional
checkpoint and is currently NOT RUN. Existing standalone MCP behavior is unchanged.

The accepted OpenRouter profile used `deepseek/deepseek-v4-flash`; the model field
remains editable. See [the implementation report](ASSISTANT_IMPLEMENTATION_REPORT.md)
for offline counts, live/native acceptance, executable identity and limitations.

Protocol references: [DeepSeek tool calls](https://api-docs.deepseek.com/guides/tool_calls/)
and [OpenRouter tool calls](https://openrouter.ai/docs/guides/features/tool-calling).
