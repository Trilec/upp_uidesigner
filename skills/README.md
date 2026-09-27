# Skills: which files to use

The latest uploadable packages are always directly in this directory:

- `uidesigner-design.zip` — create and edit Designer JSON layouts.
- `upp-ui-development.zip` — implement and repair native U++ / Ui applications.

Upload each ZIP separately to ChatGPT Skills. Each includes its `SKILL.md` and
all supporting references, scripts or example assets. No additional file is needed.

The matching folders (`uidesigner-design/` and `upp-ui-development/`) are the
editable sources used to build those packages. ZIPs do not belong inside them.
Their supporting folders differ because the two skills need different resources.

The canonical native development skill now lives in `upp_Ui/skills/upp-ui-development`.
This repository keeps a distribution copy refreshed by PackageSkills.py. Edit the
Ui source, then package here to refresh this copy. The companion HTML-mockup skill
is distributed from `upp_Ui/skills/upp-ui-html-mockup.zip`.

The two `*-chat.md` files are optional flattened references for chat systems that
cannot install ZIP skills. Do not upload them in addition to an installed ZIP.

Run `python PackageSkills.py` from the repository root to refresh both packages
and chat references in this directory. `build/` is not a skill distribution folder.
