# ACTIVE WORK
Remote GitHub main is authoritative. Fetch both repositories before work/publish; never force-push.

BASE / CURRENT INTEGRATION
- Exported-theme source base: UiDesigner 122beb77363c6da09e33644af4009e33abf4ff18.
- Current required reusable source observed: upp_Ui 4e01b005e170dba3aa2a9eb4f222bf014d9ea8e8.
- Current Designer regression-repair checkpoint before this recovery update:
  432d06dcc69d0c16419073b218eaf8b4c0ecffd5.
- Canonical Windows exe: E:\apps\github\upp_uidesigner\build\UiDesigner.exe.

TASK
Close deterministic aggregate regressions exposed by the expanded RC gate, then finish
exported-theme generated-app/manual validation. Do not start remaining controls or AI UI yet.

EVIDENCE ALREADY OBTAINED
- ExportedThemeContractTest Debug on Windows: PASS, checks=24 failed=0, exit 0.
- UiDesigner Debug application build on the prior validation head: PASS.
- Full supervisor gate previously reached deterministic tests but stopped on PropertyEditor,
  RegressionTests and FoundationTests; generated/manual closure was therefore not attempted.

REUSABLE UPP_UI REPAIRS
- 8a31447cdfa5775c82bb863916fd77909474c5b0:
  PropertyEditor inactive override body clicks now activate the inherited override before edit.
- 78740b219ca8f9dd482713c25523cb9c9de6edcb:
  themed PropertyEditor odd/even/hover/selected rows remain visually distinguishable.
- Current main adds only later UiGraph performance work/docs on top of those contracts.

DESIGNER REGRESSION REPAIRS PUBLISHED
- 029260bb70a96017cfdf0973d91a1448ffa0d290:
  preserve ThemeDocument baseline when local overrides are suppressed; emit direct role-only
  Button/ToolButton SetCustomStyle without an unnecessary patched Style block.
- 196df1cf6e38ef1b6eef76fc76104d285dfd0eca:
  keep hierarchy parent surface routable to the real UiTree for synthetic/automation clicks.
- bbbc6967026109801ee27493823768f6db7ba2a7:
  align aggregate tests with current contracts: no unbacked Panel skin Theme field; require all
  eight named layout presets without forbidding the additional Demo preset; expose Foundation
  fixture construction error text.
- 432d06dcc69d0c16419073b218eaf8b4c0ecffd5:
  derive radius override click from live PropertyEditor geometry; TabPage expects the intentional
  read-only theme.status row rather than an empty ThemeOverrideModel.

EXPORTED THEME CONTRACT
- preset/mode applies before generated controls build.
- ThemeDocument style_overrides inherit by appearance + domain + type + role.
- active local visual override wins; disabled/reset local override inherits again.
- studio_preview never reaches runtime/generated output.
- design.json remains canonical source; theme.json is optional authoring metadata only.
- generated executable has no working-directory dependency on theme.json.
- ComponentOnly uses the same BuildGeneratedUi theme initialization and normal Ui linkage.

NEXT WINDOWS GATE
1. Pull current upp_Ui and upp_uidesigner mains; record exact SHAs/worktrees.
2. Run focused PropertyEditorTests, Tests, RegressionTests, FoundationTests and
   ExportedThemeContractTest before the full supervisor script.
3. If any focused deterministic test fails, stop with every exact FAIL line plus summary.
4. If all focused tests pass, run RunSupervisorValidation.ps1 completely.
5. Then launch UiDesigner and complete generated-app/CWD/ComponentOnly/re-export/manual Theme
   comparison from the previous handoff.

STATUS
REGRESSION REPAIR PUBLISHED — WINDOWS VALIDATION PENDING.

AFTER PASS
Close exported-theme blocker, then continue UiDateTime, UiColorMatrix, UiMatrixSelector,
UiChartRing and UiGallery. AI/AgentFlow Assistant implementation remains post-RC.
