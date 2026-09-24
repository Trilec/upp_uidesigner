# Native UiDesigner assistant architecture

Status: UID-ASSISTANT-01 implementation; live acceptance tracked separately.

## Decision (supersedes the previous embedded AgentFlow requirement)

UiDesigner contains a small native application assistant. No AgentFlow runtime,
provider package, workflow scheduler, subagent system or plugin dependency is
embedded. AgentFlow may later operate Designer externally through MCP.

The packages are:

- `AppChat`: Core-only messages, provider interface, bounded streaming turn loop,
  cancellation and mailbox projection. Windows HTTPS uses WinHTTP. No Ctrl,
  Designer or AgentFlow dependency. AppChatTests supplies an alternative host.
- `UiDesigner/Assistant`: captured live-session context, compiled versioned skills,
  one allowlisted operation registry, typed validation, immutable proposals and
  receipts. It calls UiDesignerAutomationService and canonical authoring services.
- `UiDesigner/AssistantUi`: native bottom drawer, composer, transcript, scope,
  provider profile and human proposal actions. GUI events delegate editing to host.

## Authority and approval

Document and Theme remain separate canonical authoring domains. Selection and
Studio sample preview are projection state. Durable Document edits use Commands;
Theme recipes use Theme history. Typed batches are prevalidated and create one
Document undo entry. Presets use the existing insertion planner and semantic
ownership checks. No model-supplied whole-document replacement is accepted.

Model tools inspect or prepare; Apply is absent from the registry. A human Apply
checks document generation, revision and monotonic Theme revision again. Changed
state rejects the proposal instead of rebasing. Explicit node IDs survive selection
changes. A proposal can apply only once; duplicate clicks return its receipt.
Theme and Document groups are separate. Old cards do not invoke generic Undo.

The composer also recognizes the exact human commands `apply`, `apply it`, and
`apply proposal`. These are handled locally, never sent to the model, and apply
only when exactly one proposal is pending. Zero pending proposals produce an
explanation; several require explicit selection and the Apply button. This path
uses the same captured-target and revision checks as the button.

The external standalone MCP host remains independent. Optional attached MCP must
reuse this registry and its checks; it must not expose the legacy Handle escape
hatch. Attachment is disabled/unimplemented until its separate checkpoint.

## Provider and lifecycle

The worker owns immutable request data and a shared mailbox, never window/session
pointers. The GUI polls at 100 ms and owns tool dispatch. Complete streamed tool
arguments are validated before dispatch; correlated tool messages continue a
bounded turn. Provider-private reasoning is retained only when required by the
protocol and is never rendered. Cancellation prevents late tool dispatch.
Window destruction cancels and joins the bounded network worker before members
are destroyed. Document switches reject old tools and proposals.

Only explicit DeepSeek and OpenRouter profiles are implemented. Trusted
application settings store provider, endpoint, model and credential-variable
reference. Credentials stay in the process environment; TLS validation is on and
redirects are disabled. No model-selected endpoints, automatic retries, saves,
exports, filesystem or shell execution exist. Model support is verified per live
acceptance, not assumed from a compatible API label.

Conversation, credentials and proposal records are outside project serialization
and generated application output. Conversation is in memory and never replays
pending mutations after restart. Hiding the drawer preserves the request and draft;
Stop cancels. Drawer geometry does not alter virtual canvas dimensions or zoom.

## Capability truth

Catalog and Theme adapter schemas are the only field authority. Control descriptions
include defaults, choices, bounds, null semantics, adapters, parenting, data and
preview/export support. Compiled skills provide short composition guidance and
are retrieved by versioned ID regardless of working directory. Project text and
skill examples cannot grant execution privileges.

Typography operates only on registered font-family mappings, including nested
fields, preserving other attributes. Recipe scope and local override scope remain
distinct. Studio roles are independently controlled by its existing toolbar.
Structured collection data remains an explicit
unsupported boundary in this initial implementation; registered presets and typed subtrees provide
editable compositions. See ASSISTANT_USAGE.md and ACTIVE_WORK.md for evidence.
