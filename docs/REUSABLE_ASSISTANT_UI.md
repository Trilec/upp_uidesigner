# Reusable native assistant UI

This is the reference integration for other U++ applications. No model training
or Designer dependency is required by the presentation package.

## Package boundaries

| Package | Owns | Must not own |
|---|---|---|
| AppChat | Provider protocol, bounded turn loop, cancellation | Widgets or application mutations |
| AppChatUi | Literal message display, measured folding, activity disclosure, action layout, scrolling/navigation | Provider configuration, tools, proposal validity, Apply/Undo |
| Application host (UiDesigner/Assistant here) | Canonical tool schemas, proposal collection, captured targets, validation, commit receipts | Transcript widgets |
| Application controller (UiDesigner/AssistantUi here) | Request/message lifecycle, profile UI, stable proposal references, action callbacks | Direct arbitrary document mutation |

`examples/AppChatUiExample` is a standalone consumer of `AppChatUi` and `Ui`.
It has no Designer or network dependency. Build with:

```powershell
E:\upp-18468\umk.exe github AppChatUiExample CLANGx64 -br +GUI build\AppChatUiExample.exe
```

## Integration recipe

1. Add an `AppChatConversationView` to the application's panel.
2. Append user and assistant messages with `AddMessage(role, text, stable_id)`.
   Retain original message text with the application's conversation state. The
   card retains its full supplied text; folding does not truncate it.
3. Attach host actions with `AddAction(id, label, callback)`. Only the callback
   owns authority. `JumpTo(stable_id)` expands/navigates and never invokes actions.
4. Update status/action availability from the authoritative host collection.
   Hide Apply when stale or settled. Keep applied records as receipts, not live
   replayable commands. The Designer's label is explicitly "Applied receipt";
   it does not claim the transaction remains in effect after Undo.
5. Put bounded tool diagnostics in `SetActivity`; never include credentials or
   provider-private reasoning. Call `Arrange(true)` for a new message, not during
   ordinary polling, so users can read older content without losing their place.
6. Stop the provider turn before clearing messages or destroying the controller.
   Clear UI/model conversation state together. Keep application documents and
   normal Undo separate from discussion history.

UiLabel does not expose width-constrained text wrapping. The message card wraps
literal Unicode text at words (long words may break), measures outside Paint and
caches by width/content/theme. Ampersands are escaped for label mnemonic handling.
The collapsed view shows three measured lines. Activity is a separately scrollable
diagnostic view. A modest conversation uses real child controls; virtualization
can be added when a measured workload requires it, not pre-emptively.
Message content is literal text; Markdown is not rendered as rich text.

## Designer behaviour

The lower drawer is one chronological conversation. Explicit History navigates
to proposal cards. Apply, Show code, Refine, Dismiss and Select affected belong
to each proposal. Prose-only messages have no Apply. Send becomes Stop while
running. Profile/model lives below the composer in a separate settings dialog.
The application footer has one Assistant/Collapse toggle.

Show code is a read-only **Proposal JSON** window, not generated C++. Editing
JSON in-place is intentionally not supported: refinement creates a newly
validated proposal. Large payloads are no longer tooltips. For a pending draft,
the old proposal becomes revised only after a fresh proposal has been prepared.
For an applied composition, Refine passes actual surviving node IDs and requests
edits to those nodes, avoiding a duplicate insertion. Unsupported type changes
must be explained, not pretended to be supported.

Clear all stops current work and forgets discussion, draft and proposal records;
the design and Undo history remain. Undo is explicitly the latest **Document**
change, not a historical card action. Theme changes still use Theme Studio history.

No TitleCard extension was necessary for this use case. The reusable unit is
the conversation composition, built from ordinary Ui controls, so it can evolve
without introducing chat-specific semantics into general title/content controls.
