# UiDesigner embedded assistant architecture

**Status:** post-RC design authority; no assistant UI/runtime dependency is part of the current release-candidate gate.  
**UiDesigner source inspected:** `90ee92301f147bf8513b2b22d7abc101b9a38e65` plus release-cleanup descendants.  
**AgentFlow reference inspected:** `60cfeb6bcbb697096d8eeae0238a63130fe95756`.  
**DeepSeek Harness reference inspected:** `47f943859bef60e4160492346772ded9b24f765a`.

## Purpose

UiDesigner should eventually provide a conversational assistant that can understand the current document, selection, property schema, Theme state, validation state and generated output, and can safely propose or apply changes through the same authoring rules as the graphical UI.

This feature must not create a second document model, a second command history or an MCP-shaped internal architecture.

## Architectural decision

When the assistant is part of UiDesigner, prefer **embedded AgentFlow**.

```text
UiDesigner Assistant UI
        |
        v
embedded AgentFlow session/runtime
        |
        +--> AgentFlowProviders --> cloud/local model
        |
        v
UiDesigner assistant tool gateway
        |
        v
UiDesignerAutomationService
        |
        v
canonical Session / Commands / Theme / Catalog / Export
```

The existing `UiDesignerAutomationService` is already the correct application-control plane. It exposes document/selection inspection, property preview/commit, Theme preview/commit, drop planning/application, undo/redo, validation, code generation, save/load and export through the canonical services.

The existing `UiDesignerMcpEndpoint` remains an **external transport adapter**:

```text
external ChatGPT / Codex / Claude / other host
        -> MCP
        -> UiDesignerMcpEndpoint
        -> UiDesignerAutomationService
```

Do not route embedded AgentFlow through MCP merely because MCP already exists.

This matches AgentFlow's current North Star: embedded AgentFlow is preferred when it is the product intelligence; MCP is primarily for an external host operating an application. Agent Bridge is used later for capabilities that live outside the UiDesigner process.

## What to reuse from DeepSeek Harness

DeepSeek Harness is a useful reference, not a UiDesigner dependency.

Useful patterns:

- provider/model, tools, session/event state and UI integration are separable capability seams;
- provider implementations can be swapped without rewriting the agent loop;
- model-visible context should be reconstructable from owned records rather than hidden SDK session state;
- UI adapters drive an agent service and render from explicit events;
- local and remote execution can share one semantic control surface;
- tool execution and policy live behind explicit registrations rather than arbitrary application mutation.

AgentFlow remains the authority for our implementation because its contracts already separate Runtime, Providers, capabilities, Agent Bridge, MCP and UI projection in the way our U++ applications require.

## UiDesigner authority rules

The assistant must obey the same rules as every other authoring path:

1. `UiDesignerDocument` remains canonical persistent document state.
2. Theme remains a separate canonical Theme document/history.
3. Selection and preview are session/projection state, not saved authority.
4. Durable edits go through command/application services.
5. Every mutation is revision-aware.
6. Conversational edits are undoable/redoable.
7. Drop/reparent operations use the existing planner and validation.
8. The assistant never writes arbitrary document JSON as a shortcut.
9. Code generation and export remain projections of canonical state.
10. Provider/API credentials never become part of a UiDesigner project document.

## Recommended UI

Do not add a permanently visible fourth column.

The first production assistant surface should be:

- one small Assistant icon in the existing shell chrome;
- opens a **collapsible, resizable bottom drawer** across the main workspace;
- the drawer can later support **Pop out** into a separate modeless window;
- closing/hiding the drawer does not stop an active agent unless the user explicitly cancels it.

The bottom drawer is preferable because the right side already carries Hierarchy/Inspector/Theme/Data/Events/Code/Diagnostics responsibilities and must remain usable.

Initial drawer contents:

```text
+-----------------------------------------------------------------------+
| Assistant | provider/profile | context status | Stop | Clear | Pop out|
+-----------------------------------------------------------------------+
| conversation / proposals / tool results                               |
|                                                                       |
+-----------------------------------------------------------------------+
| [ Ask or describe a change...                                  ] Send |
+-----------------------------------------------------------------------+
```

Useful message rendering:

- normal assistant/user messages;
- compact tool/action cards;
- proposed change summary;
- validation error/warning;
- applied/undone state;
- links/actions to select affected nodes or open relevant Inspector sections.

Do not expose private model chain-of-thought. Show explicit actions, inputs, evidence, tool results and concise rationale/decision summaries.

## Context model

Do not dump the entire project into every model request.

The assistant should begin with lightweight project/session context and use tools to retrieve exact information:

- current document revision;
- selected node ids and primary selection;
- control schema/catalog search;
- selected properties and defaults;
- Theme effective/authored state;
- hierarchy/document slices;
- validation errors;
- generated code only when needed.

This keeps prompts bounded and ensures the agent reasons from current canonical state rather than stale copied context.

## Editing flow

A conversational request such as:

```text
Make these three buttons equal width and use the Accent role on the primary action.
```

should become:

```text
conversation
    -> inspect selection/schema
    -> build typed proposal
    -> check current revision
    -> preview/plan where appropriate
    -> summarize proposed changes
    -> commit through existing commands
    -> receive canonical change events
    -> update UI/projection
```

For inherently destructive or broad operations, V1 should present the proposal before commit. Small reversible edits can later support an explicit user preference for direct apply because they still pass through command history.

## Local and cloud models

Provider choice belongs to AgentFlowProviders, not UiDesigner document state.

The UI may eventually offer profiles such as:

- cloud OpenAI-compatible/provider profile;
- DeepSeek;
- Anthropic;
- local model/gateway;
- future provider.

A UiDesigner project may retain a non-secret preferred assistant profile id if useful, but credentials remain in provider/application configuration.

## External capabilities

An embedded assistant should not gain arbitrary filesystem/network/application authority merely because the model can request it.

UiDesigner-native authoring capabilities are exposed through the Designer tool gateway.

External applications/services later use AgentFlow's capability broker / Agent Bridge:

```text
embedded AgentFlow
   -> authorised capability
   -> Agent Bridge
   -> external application/service
```

Examples might include a repository/build service, Dramatica, TaskTrack or another running application.

MCP remains available for the opposite direction: an **external** chatbot operating UiDesigner.

## Session and persistence

Assistant conversation/runtime state is separate from the authored UI document and Theme document.

V1 should treat assistant sessions as application/project companion state. Saving the UI project must not silently make a provider transcript part of generated application output.

Durable authoring effects are already captured by document/Theme state and command history; the assistant session may retain user/assistant/action summaries for continuity without becoming application authority.

## Concurrency and cancellation

Agent work must be asynchronous.

- GUI changes are marshalled onto the GUI thread.
- Every mutating tool call checks the expected/current document revision.
- A stale proposal is rejected and re-planned rather than blindly applied.
- Stop/cancel terminates the current agent operation without corrupting authoring state.
- Provider retries are AgentFlow Runtime policy, not hidden provider SDK loops.

## Post-RC implementation sequence

Do not begin this sequence until the current UiDesigner release gate is accepted.

### A1 — Assistant tool contract

Define an embedded tool gateway over `UiDesignerAutomationService` with direct U++ calls. Reuse its semantics; do not duplicate document operations.

Acceptance: the same edit made through UI and assistant gateway reaches the same command service and produces equivalent canonical state/history.

### A2 — Assistant session abstraction

Add provider-neutral AgentFlow session ownership, cancellation and event projection. No graphical chat yet.

Acceptance: fake/local test provider can inspect the document, propose one reversible edit and receive the resulting state.

### A3 — Drawer UI

Add Assistant icon + bottom drawer + conversation/event view + input + stop.

Acceptance: drawer can hide/show without affecting Designer geometry/state and without creating another document authority.

### A4 — Cloud/local providers

Wire AgentFlowProviders profiles. Keep credentials outside project files.

### A5 — Proposal UX and source mapping

Render proposals/tool results and allow selecting affected controls from chat actions.

### A6 — Optional external capability bridge

Add Agent Bridge capabilities only when a real external integration requires them.

## Current release boundary

The assistant architecture is now deliberately specified, but **not implemented in the current RC**.

Current priority remains:

1. build UiDesigner against current `upp_Ui/main`;
2. run the complete automated closure gate;
3. perform final interactive visual/drag/drop/Theme/export audit;
4. repair only real release blockers;
5. clean release documentation/status;
6. tag/accept the RC;
7. then begin A1.
