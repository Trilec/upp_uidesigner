# ACTIVE WORK

Remote GitHub `main` is authoritative. Fetch both repositories before work/publish; never force-push.
Recovery state only; Git history is implementation history.

BASE: `e071343bee0bb53a23a432551f831683972c1036`
TASK: **Close Foundation generated-project regression, then finish RC validation**
TOUCHED:
- `UiDesigner/Catalog/UiDesignerCatalog.cpp`
- `tests/FoundationTests/main.cpp`
- `tests/Tests/main.cpp`
- `docs/ACTIVE_WORK.md`
STATUS: **SOURCE REPAIR PUBLISHED — WINDOWS VALIDATION PENDING**
PUBLISHED: `b9bbebb913636d639240152f422c115c19bf323b`
VALIDATION: source/diff/API/package review complete; Windows rerun pending.
NEXT ACTION: rerun Foundation first, then ExportedThemeContractTest and the complete supervisor gate.

## VERIFIED / REPORTED INPUT

Gary validated the reusable PropertyEditor first-wheel repair on Windows:
- required PropertyEditor repair `b9c3a863ee424d49f8897b1970897200726c94fc` was an ancestor;
- tested upp_Ui HEAD `0968129f882e5ffc8bf65a3ed87fdfc5f3f5a357`;
- PropertyEditorTests Debug + Release: `77/0`;
- PropertyEditorOverrideCommitTest Debug + Release: `6/0`.

At UiDesigner `e071343...`:
- Tests Debug: PASS;
- RegressionTests Debug: PASS;
- FoundationTests built but returned `73 checks / 16 failures`.

The Foundation failures were one cascade. Its fixture used the normal drop planner to place
ordinary UiPanel controls directly below UiTab. The plan succeeded, but
`ValidateDocument()` correctly requires UiTab direct children to be semantic UiTabPage nodes.
Code generation therefore stopped before any source/package emitters ran.

## CURRENT REPAIR

- `CanParent/CanInsert` now preserve the same semantic-owner structure enforced by
  `ValidateDocument`: UiTab accepts only UiTabPage directly and UiAccordion accepts only
  UiAccordionSection directly.
- Foundation content is placed inside the two real default UiTabPage nodes created with UiTab.
- Tests protect both rejection of invalid direct owner drops and valid content drops into
  UiTabPage / UiAccordionSection.
- Foundation now prints CodeGen diagnostics on a future generated-project validation failure.
- CodeGen/export expectations were not weakened or rewritten.

## REQUIRED NEXT GATE

1. FoundationTests Debug. Require all checks PASS and exit 0.
2. ExportedThemeContractTest Debug. Require `failed=0` and exit 0.
3. If both pass, run `RunSupervisorValidation.ps1` completely.
4. Then complete the remaining generated/manual Theme fidelity audit.
5. Preserve the separate upp_Ui final graph/performance/manual acceptance path; current upp_Ui
   may contain newer unrelated UiGraph work, so fetch current main and preserve it.

## CONTRACTS TO PRESERVE

- UiTab direct children are UiTabPage; page content lives below the page.
- UiAccordion direct children are UiAccordionSection; section content lives below the section.
- A successful drop plan must not create a document that canonical validation rejects.
- Designer document/data -> Preview -> CodeGen -> generated application must preserve authored state.
- ThemeDocument preset/mode and inherited style recipes compile into generated output;
  active instance override wins; disabled/reset local override inherits again.
- `theme.json` is optional authoring metadata, never a generated runtime/CWD dependency.
- Reusable `upp_Ui` defects are fixed in `upp_Ui`, not through Designer workarounds.
- Do not start remaining controls or AI/AgentFlow work before RC/theme closure.
