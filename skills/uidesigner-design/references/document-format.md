# Native document format (schema 4)

The file is a document object, not an array of widgets or an assistant proposal.
Required shape:

```json
{"format":"upp-ui-designer-next","schema":4,"ordering":"explicit-children",
 "document_id":"unique-design-id","revision":0,"virtual_size":{"cx":800,"cy":600},
 "nodes":[{"id":1,"parent":0,"type":"Window","name":"Window","flags":9,
 "children":[],"properties":{},"actions":[]}],"resources":[]}
```

Use the complete examples to start. The first node is the Window root. Each node
has a positive unique integer `id`, `parent` ID, exact catalogue `type`, unique
C++-identifier `name`, `flags`, ordered `children`, `properties`, and `actions`.
Every non-root node occurs once in its parent's children. No cycles, missing
parents, duplicate IDs or unreferenced nodes. Array order is not the visual order;
the parent's children array is authoritative. Use parent-before-child array order
for readability. The root's parent is 0. Do not create a second Window.

`virtual_size` is the design canvas size. Use sensible desktop dimensions; it is
not a substitute for layout constraints. Node properties contain raw JSON values,
not `{type,value}` wrappers. Booleans are JSON booleans, numbers are numbers.
Exact enum strings are case-sensitive. Omitted supported properties use defaults.
Include sizing explicitly for important layout regions. A node name lives at the
node level; do not confuse it with visible `text` or `title`.

For children of UiGridLayout, `grid_row` and `grid_column` are integer **node
properties** (zero-based rows/columns). Do not invent row/column span properties.
This differs from the embedded assistant's `prepare_composition` envelope, which
places paired grid coordinates beside `properties`. Do not copy that envelope
into a design file. Keep cells within the grid's declared dimensions.

Spacer is semantic, with flags 24 (Structural|SemanticItem), and must belong to
a supported layout. Controls use flags 0 in the portable examples. Never represent
a Spacer as an empty Button or Panel. Use its own schema (`h_sizing`, `v_sizing`),
not control width/height modes.

UiTitleCard has one content slot: place one layout inside it when several controls
must be hosted. UiTab children are UiTabPage, and UiAccordion children are
UiAccordionSection. Ordinary controls cannot accept children. Read child_adapter
and capabilities in the schemas before choosing other container relationships.

`theme_overrides` is a map of registered theme-field IDs to encoded values.
`theme_override_saved` holds inactive saved overrides; preserve it when editing.
`data` is separate from properties and must match the control's data schema.
Images/resources require real encoded bytes and valid hashes; do not fabricate
resource IDs or base64. Use an exposed icon choice or text if no asset is available.

Actions default to `[]`. A visual button label does not implement Accept/Cancel.
Only add action bindings from a verified existing example or canonical event/action
schema. Do not invent JavaScript callbacks or handler implementations.

A project envelope can also contain a document, theme and generation settings.
For a new portable mock-up, use document JSON as above; Designer can load it.
For an existing project, preserve its envelope rather than stripping its theme.
