# ACTIVE WORK

Remote GitHub `main` is authoritative. Fetch both repositories before work/publish; never force-push.
Recovery state only; Git history is implementation history.

TASK: **Close Foundation/generated Theme RC and finish validation**
STATUS: **SOURCE FIXES PUBLISHED — WINDOWS VALIDATION PENDING**
CURRENT SOURCE CHECKPOINT: `2343679341e46fb128b71596f5255b972253bfaa`
REQUIRED upp_Ui SOURCE CHECKPOINT: `dc196091ba1452bc7bd2091124cc4391d22503a3`

## VERIFIED INPUT

PropertyEditor first-wheel repair was already Windows-validated:
- PropertyEditorTests Debug + Release: `77/0`;
- PropertyEditorOverrideCommitTest Debug + Release: `6/0`.

Foundation previously failed `16` assertions because its normal drop fixture could put ordinary
controls directly below UiTab while canonical validation requires UiTabPage ownership.

Repair already published:
- `CanParent/CanInsert` enforce the same UiTab / UiAccordion semantic-owner structure as
  `ValidateDocument`;
- Foundation content lives inside the real default UiTabPage nodes;
- CodeGen/export expectations were not weakened;
- generation failures now print their actual diagnostic.

## LATEST WINDOWS BLOCKERS AND REPAIRS

Validation at UiDesigner `f777eb5822f633217432fd29f3d9106536295559` stopped compiling `Tests`.

Blocker 1 was reusable upp_Ui:
- `Vector<UiGraphPortRef>::Reserve()` relocation assertion in the compiled H2 backend.
- fixed in upp_Ui `dc196091ba1452bc7bd2091124cc4391d22503a3` with the supported `is_upp_guest` relocation contract while
  preserving aggregate initialization.

Blocker 2 was local test source:
- the new semantic Accordion regression reused `accordion_drop_session`,
  `accordion_drop`, and `accordion_drop_node` names already present later in the same
  `CONSOLE_APP_MAIN` scope;
- fixed in `2343679341e46fb128b71596f5255b972253bfaa` by renaming only the new fixture identifiers;
- no behavior or expectation changed.

## REQUIRED NEXT GATE

1. Fetch both mains and verify the checkpoints above are ancestors.
2. Build/run Designer `Tests` Debug first.
3. If PASS, run `RegressionTests` Debug.
4. Run `FoundationTests` Debug; require all checks PASS / exit 0.
5. Run `ExportedThemeContractTest` Debug; require `failed=0`.
6. Run `RunSupervisorValidation.ps1` completely.
7. Finish generated/manual Theme fidelity:
   - Preview -> CodeGen -> generated application;
   - Light / Dark / non-default preset;
   - inherited Theme recipe;
   - active local override wins;
   - disabled/reset local override inherits;
   - no runtime `theme.json` CWD dependency;
   - ComponentOnly parity;
   - re-export preserves user code.
8. Continue current upp_Ui automated + manual UiGraph acceptance only after Designer focused gates pass.

## CONTRACTS

- UiTab direct children are UiTabPage; page content lives below the page.
- UiAccordion direct children are UiAccordionSection; section content lives below the section.
- A valid drop plan must not create a document canonical validation rejects.
- Designer document/data -> Preview -> CodeGen -> generated app preserves authored state.
- ThemeDocument inheritance/override precedence remains unchanged.
- `theme.json` is optional authoring metadata, not a generated runtime dependency.
- Reusable defects are fixed in upp_Ui, not through Designer workarounds.
- Do not start remaining controls or AI/AgentFlow work before RC/theme closure.
