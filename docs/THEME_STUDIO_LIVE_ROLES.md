# Theme Studio live roles — UID-STUDIO-LIVE-01

## Defects and repair

The two V2 role dropdowns listened to WhenAction. UiDropdown selection emits
WhenSelectData instead. Both now use that selection event, remove the obsolete
base callbacks, and synchronize the dropdown by data silently to avoid recursion.
All existing panel and control sample adapters rerun immediately; the two role
axes and their appearance/type/role recipe keys remain independent.

The compound SliderEdit children and Breadcrumbs were absent from the live role
application list. They now receive the selected Control Role using their existing
child adapters / role API. Catalog rebinding finishes with V2 universal roles,
not the legacy panel-role interpretation.

The Table previously allocated an empty 4x2 model. It now has Item / Status / Count
headers and six populated rows. Sample construction happens once, not on Theme
changes. Contents and active-cell selection survive role and mode changes. The
Table has no editable catalog recipe here: its sample style starts with the full
current-mode native Table style, then uses existing Panel/Label/List/Dropdown
role resolvers for table, header, ink and selection presentation. The enclosing
GroupPanel remains on Panel Role. No new global palette synthesis or document
schema is introduced.

The active Tab defect was in reusable UiTab painting, not a missing Designer
colour override. A transparent body fell back to OS SColorFace for the active cap;
the strip also mixed in OS SColorPaper. upp_Ui checkpoint
`8114269abd91cc33569f68117bef4fd4d537897a` removes those substitutions while retaining
transparent and explicit tab fills. No layout/hit/selection behaviour changes.

## Focused validation

Source/diff/API review and local whitespace checks passed. New Windows tests have
not been executed by the remote supervisor. Do not report visual acceptance yet.

Build/run these two GUI-linked packages with CLANGx64 and +GUI:
- `Utilities/UiTabThemePaintTest`: real pixel checks for active caps and strip.
- `ThemeStudioRoleTest`: actual dropdown selection callbacks, live sample styles,
  independent axes, Light/Dark/Light, table data/header persistence and recipes.

Require exit 0 and positive-check summaries ending in failed=0:
`UITAB_THEME_PAINT checks=<actual> failed=0`
`THEME_STUDIO_ROLE checks=<actual> failed=0`

Gary's current assignment is limited to these focused checks, compiling the
canonical Release Designer and leaving it open for Curt. No full supervisor
matrix, DateTime re-audit or graph-project gate is required by this task.
Curt performs the visual checks: independently change both roles in Controls and
Containers, repeat in Dark and back to Light, switch active tabs, and inspect
populated table headers/data/selection. Existing explicitly authored recipe
colours remain intentional overrides; role preview must not erase them.
