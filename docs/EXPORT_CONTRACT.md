# UiDesigner export contract

## Profiles

UiDesigner exposes five distinct export profiles:

1. **Complete C++ package** — generated base, preserved user subclass, entry point, package manifest, source design, project metadata and optional Theme JSON.
2. **C++ component/class only** — generated base and preserved user subclass without an application entry point or package manifest.
3. **UiDesigner project JSON** — canonical document, Theme and generation options in one project file.
4. **Document JSON** — canonical design document only.
5. **Theme JSON** — Theme Studio document only.

The split-button and CLI/MCP profile names map to these operations directly. JSON profiles do not create unrelated C++ files.

## Naming

Package name, C++ class name and namespace are validated independently as canonical identifiers. The package manifest filename uses the package name; the source inventory inside it uses the class name. This allows, for example, `GeneratedPackage.upp` to compile `GeneratedUiWindow.generated.cpp`.

## Ownership

Generator-owned files:

- `<Class>.generated.h`;
- `<Class>.generated.cpp`;
- `<Package>.upp`;
- generated design/project/Theme metadata.

User-owned files:

- `<Class>.h`;
- `<Class>.cpp`;
- `main.cpp`.

User-owned files are created when absent and preserved on regeneration unless the caller explicitly selects replace-all behavior.

## Generated behavior

The generated base constructs controls, attaches layout children, emits semantic Spacer/Separator APIs and wires declarative actions. The user subclass owns additional event wiring and named handler bodies.

### Window settings

Select Window in Hierarchy, click empty workspace outside the canvas, or grab a
canvas resize edge to inspect Window settings. Width/height define the initial
client size (the restored size when starting maximised). Window root properties
are stored in canonical document JSON and edited through normal document Undo:

- `window_title`: native title; empty uses the generated class name.
- `startup_appearance`: `Light`, `Dark`, or `Preview` (default for compatibility).
  Explicit Light/Dark selects the matching theme recipes before export; it does
  not change the editor's appearance. Preview follows the current editor view.
- `window_resizable`: defaults true. False creates a fixed-size window.
- `window_maximize_box`: defaults true; only effective when resizable.
- `window_start_maximized`: defaults false; only effective when resizable.

The inspector's Theme choice selects the existing active project theme, including
custom recipes, through the Theme workspace. It is not a second theme copy on the
Window node. Save Project persists that selection; Theme Studio manages available
themes. Export, Build and the Code pane share the same generation path.
Fixed-size is not modal: modal execution and application behaviour remain the
host application's responsibility.

## Preview

Before writing, the export service validates identifiers and the canonical document and returns the exact file inventory, including files that will be preserved. The GUI dialog displays this inventory before terminal export.

## Transactional publication

C++ exports follow a recoverable transaction:

1. validate every target and overwrite rule;
2. generate every file in memory;
3. write all new content to a staging directory;
4. back up every destination that will be replaced;
5. publish through temporary sibling files;
6. on any failure, remove newly published files and restore every backup;
7. remove staging data after success;
8. retain no `.uidesigner-tmp-*` or staging files.

Single JSON exports use a temporary sibling and atomic move.

## Overwrite policies

- `RefuseExisting`: fail before writing if any selected target exists.
- `ReplaceGenerated`: replace generator-owned files and preserve user-owned files.
- `ReplaceAll`: replace all files selected by the profile.

The caller also carries an explicit preserve-user-files flag. The GUI defaults to `ReplaceGenerated` with preservation enabled.

## Determinism

For the same document, Theme and generation options, generated file names, ordering and generator-owned content are deterministic. User-owned files are excluded from byte-comparison requirements after the first export.

## Verification

`FoundationTests` exercises inventory, semantic code emission, package/class separation, all profiles, refusal preflight and user-code preservation. `BuildGeneratedFixture.ps1` exports a fixture, compiles it through `umk`, launches the generated process briefly and cleans the fixture on success while retaining it on failure.
