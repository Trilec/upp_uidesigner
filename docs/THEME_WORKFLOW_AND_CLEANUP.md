# Theme Studio lifecycle repair and workflow proposal

## Implemented: selection hang and focused cleanup

Reproduced in a blank project by selecting Button, interacting with Accordion,
selecting List and ProgressBar, clicking the Buttons group heading, then selecting
Line Edit. The canonical application became unresponsive in both Release and Debug.
The matching-symbol stack (`build/InspectorHang-debug-stack.log`) alternated
`SyncSelectedTarget`, `SyncSelectedTargetV2`, `SetActiveStyleTarget` and
`ApplyThemeToShell`: the legacy and universal panel-role keys differed, and selecting
an Inspector target incorrectly broadcast a rendered-theme preview change.

There is now one gallery binding lifecycle. Its concrete gallery supplies target
selection, property-model projection and style application. Target changes notify
the Inspector separately from actual theme changes; cancelling a real transient
preview still notifies rendered-theme observers once.

The focused old/new-code audit traced tracked source, package manifests, tests and
examples in Designer, plus consumers in shared Ui. Removed:

- Duplicate V2 catalog/document binding and property-provider registration.
- Legacy gallery target synchronization and style application, including its
  unused sample-style helper. The shared base is abstract, preventing fallback to
  the obsolete implementation.
- Unreferenced, unbuilt `UiDesigner/DesignOverlay/UiDesignerDesignShell.h` and
  `UiDesignerDesignMetrics.h`. These were orphan declarations, not the production
  interaction overlay.

Retained shared gallery sample construction/layout, toolbar palette-dialog wiring,
and the production InteractionOverlay/V2 pair: these still have live consumers.
File age or a V2 suffix alone is not proof that code is unused. This is a focused
audit, not a claim that every unused function in the repository has been removed.

Regression coverage repeats sample selection and gallery rebinding 40 times,
checks stable role targets and no fake preview broadcasts, verifies unchanged
authored Theme/history, and checks cancellation of genuine transient previews.

The Controls preview also removes the obsolete reference panel/button and their
bindings. Its left column is now Buttons, Data, Rings. The tree sample shows
expanded Assets with Red/Green/Blue children, connector lines and a narrow On/Off
column using the existing tree model/rendering API. Container-page panel samples
remain available for independent panel styling.

## Proposed integrated workflow (not implemented by this repair)

The main theme dropdown should select a complete theme document, not conflate a
built-in style preset with the current document's authored customisations.

| Dropdown section/state | Meaning |
| --- | --- |
| Proposal — unsaved | Temporary AI candidate, with Compare, Keep and Discard |
| My themes | Named theme files registered after a successful save |
| Built-in themes | Read-only starting points such as Minimal and Solid |
| Selected name followed by `*` | Current theme has unsaved edits |

Show the built-in base as secondary information, for example “Workshop · based on
Solid”. Changing that base is an explicit editing action. Selecting another saved
theme loads its complete recipe set; it must not carry unrelated overrides across.

- **Save Theme** saves the active working copy to its known file. First save asks
  for a name/location; built-ins use Save As. Register in My themes only after a
  successful write. Saving never requires compiling.
- **Save Theme As** creates a separate named theme. Project Save continues to
  include its Theme snapshot. Track standalone-theme and project save checkpoints
  separately so saving one cannot incorrectly mark the other saved.
- Switching themes must retain modified working drafts or offer Save/Discard/Cancel.
  Crash-recovery drafts should be automatic and explicitly separate from saved
  library themes; failure must not overwrite the last good file.
- An AI proposal captures its source theme revision and stays separate from the
  current working copy. Refinements change requested fields only. Keep accepts it
  as one Theme-history entry; Keep and Save combines acceptance with normal save.
  Choosing another theme for comparison must not silently lose the proposal.
- **Preview in Designer** temporarily uses the candidate on application chrome as
  well as samples, with a clear return action. **Use for Designer** is a separate
  persistent app preference, not an accidental consequence of browsing candidates.
- **Export/compile theme** produces reusable application style output from the
  same saved Theme source. It is a deployment/export concern, not a saving state.

Existing Save Theme As currently serializes a snapshot without recording a named
library entry or standalone path/checkpoint. The header dropdown currently selects
only the preset. Those ownership gaps explain why the current workflow feels bolted
on. A library service, draft identity/save checkpoints and temporary candidate
projection should be designed together before adding a “Proposal” dropdown entry.

Example: choose Solid, reduce frames/corner radii through a typed AI recipe patch,
see “Untitled theme * · based on Solid”, refine individual controls visually,
Save Theme As “Workshop”, then select Workshop later or choose Use for Designer.
Palette generation, role mapping and rich per-control overrides retain the contract
in `ROLE_DEFAULT_REPAIR.md`; this proposal does not replace controls with a reduced
palette-only style model.
