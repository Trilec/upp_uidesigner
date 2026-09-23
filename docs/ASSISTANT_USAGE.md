# Native Designer assistant (UID-ASSISTANT-01)

Click **Assistant** in the footer. Drag its top edge to resize, or Collapse to
hide it. The draft, conversation and active request survive hiding. Enter sends;
Shift+Enter inserts a newline. Stop cancels the request and pending proposals.

Select DeepSeek or OpenRouter, enter an explicit tool-capable model ID, and enter
the name of a user environment variable holding that provider's API key:
`DEEPSEEK_API_KEY` or `OPENROUTER_API_KEY`. Never enter the key in the drawer.
Click **Use profile**. Sending then shares the displayed captured context with
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

Try these requests on a disposable design:

- “Inspect this design and suggest improvements without changing it.”
- “Replace mapped fonts on the captured selection with an installed family.
  Preserve size, bold and italic; prepare local overrides.”
- “List the presets and propose a suitable application shell under the root.”
- “Suggest a settings dialog using supported presets and properties.”
- “Inspect the active Theme recipe and propose typography for that recipe.”

Select a proposal in the right-hand proposal panel. Review its exact scope and
status. **Show affected** selects its captured nodes. **Apply** commits once,
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
