# Portable UiDesigner design skill

Source: `skills/uidesigner-design/SKILL.md`. This package produces the existing
`upp-ui-designer-next` schema-4 JSON document, directly loadable in Designer.
It does not introduce a competing document format. The embedded assistant's
typed editing requests remain commands against that same document, preserving
captured targets, validation and Undo.

The package contains three complete examples (plain dialog, TitleCard dialog,
workbench), outside-in layout guidance, format/role references, a portable Python
preflight, and per-control schemas exported from the canonical catalogue. Read
only relevant control schemas rather than attaching the whole catalogue to every
request. The canonical CLI validator and a visual check remain stronger evidence
than the portable preflight; it deliberately does not claim complete behavior or
resource validation.

For a skill-capable coding host, use the `uidesigner-design` folder as the skill
package. A Codex personal installation belongs in `~/.codex/skills/uidesigner-design`.
For a chat host that does not load skill folders, supply SKILL.md, the format and
layout references, the closest example, and the relevant control schema files as
reference material. This is portable guidance, not a claim that every ChatGPT or
OpenCode deployment exposes the same skill-installation UI.

Suggested request:

> Follow the attached UiDesigner design skill. Turn this image into an editable
> Designer mock-up. Use the existing schema-4 document JSON and native layouts.
> Preserve the reference's hierarchy and text; state any unsupported effects.
> Return a complete .uidesign.json file, and say which validation actually ran.

If the host can inspect images, attach the image with that request. For HTML, send
the source as reference data. This does not add image/HTML upload to the embedded
Designer assistant. External AI output is imported through Designer's normal Load.

Maintenance: rebuild `UiDesigner/CLI` and run `RefreshDesignSkill.ps1`. This exports
current schemas rather than maintaining another hand-written field catalogue.
Run all example files through both `scripts/validate_design.py` and the CLI's
`validate`, then generate/inspect representative output. Skill metadata is checked
with the standard skill-creator validator in UTF-8 mode on Windows.

## Two reusable skills

`uidesigner-design` creates loadable editable documents. `upp-ui-development`
covers U++ programming, Ui composition, models, themes, PropertyEditor and builds.
The latter includes maintained copies of six engineering guides; its header and
demo links are resolved against the target Ui checkout. It does not bundle U++.

Run `python PackageSkills.py` after refreshing schemas. It updates engineering
reference snapshots with SHA-256 provenance and writes these directly under `skills/`,
alongside the source skill folders:

- `uidesigner-design.zip` and `upp-ui-development.zip`: one skill folder per ZIP.
- `uidesigner-design-chat.md` and `upp-ui-development-chat.md`: flattened references
  for chat sessions without folder access. The design reference includes exact
  schemas and examples. It is a reference file, not text to inject in every turn.

For a new coding session, install the folders using that host's skill mechanism
and explicitly ask to use `uidesigner-design` or `upp-ui-development`. For a chat,
attach the corresponding Markdown reference and screenshot/HTML, then ask:

> Follow the attached UiDesigner skill. Recreate this reference as editable
> .uidesign.json using native Ui controls. Read the relevant schemas, preserve
> the requested layout, and report unsupported details and checks actually run.

A ZIP attachment alone is not proof of skill installation; the host must be able
to read its contents. ChatGPT skill availability and installation depend on the
host/workspace. The plain Markdown option is supplied as an explicit reference
workflow. No provider credentials are included in either bundle.

The native Assistant remains a separate integration: installing these skills
does not add direct OpenAI transport, saved profile selection or image upload.
