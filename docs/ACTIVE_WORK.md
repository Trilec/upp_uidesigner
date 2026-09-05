# ACTIVE WORK
Remote main is authoritative. Fetch both repositories before work/publish; never force-push.

BASE:
- UiDesigner: 122beb77363c6da09e33644af4009e33abf4ff18
- upp_Ui: 408c82c418fb87fb56cce4cac6752dd69add9dcf

TASK: Close exported ThemeDocument fidelity between Designer Preview and generated applications.
STATUS: IMPLEMENTATION COMPLETE — WINDOWS / VISUAL VALIDATION PENDING.
PUBLISHED:
- Runtime/export implementation: 45eb5a7575b68a04532c6d1a67f8bcd153d4c27b
- Focused regression/generated-fixture gate: de610d1dc1361a32089c4b0af382f49667bcc5bc
- Final source-review hardening: c9770b1db2d1c96bc2edbd3a4a9527180a087596
- This recovery-doc update is expected to be a direct descendant.

VALIDATION:
- Full source/diff/package review performed in supervisor environment.
- Windows U++/CLANGx64 execution and visual comparison are still pending.
- No Windows/runtime PASS is claimed here.

TOUCHED:
- Services runtime Theme recipe resolver and export compilation path.
- Preview runtime Theme snapshot inheritance.
- CodeGen optional compiled UiTheme preset/mode startup.
- UiDesigner shell passes effective ThemeDocument to Preview.
- Export dialog wording; Theme/ThemeCore/CodeGen/build documentation.
- ExportedThemeContractTest; Foundation generated themed fixture; supervisor runner.

RUNTIME THEME CONTRACT:
- ThemeDocument preset/mode applies before generated controls build.
- style_overrides inherit by appearance + control/panel domain + type + role.
- active instance Theme overrides apply after inherited Theme recipes.
- disabled/reset instance overrides inherit Theme recipes again.
- studio_preview is Theme Studio sample content/layout only and never reaches runtime.
- generated C++ contains effective recipes; design.json remains the canonical source document.
- theme.json is optional source metadata, not a runtime dependency; no CWD lookup exists.
- ComponentOnly uses the same BuildGeneratedUi initialization and normal Ui linkage only.

FIELD CLASSIFICATION:
- Production runtime: preset, mode, style_overrides.
- Document-owned instance authority: role and active per-control overrides.
- Studio-only: studio_preview.
- Serialized metadata with no production-control bridge: palettes/role slots/accent/global spacing,
  radius and shadow metrics. Do not invent a runtime mapping that Designer Preview does not use.

NON-REGRESSIONS:
- Structural/configuration properties remain document-owned.
- Existing ProgressRing, RangeSliderEdit, GroupPanel, List/Tree and export-refusal contracts remain.
- Services has no new dependency on UiDesigner/Theme and no Theme->Preview->Services cycle was added.
- Generated applications do not link UiDesigner Theme/Services packages.
- User-owned generated .h/.cpp files remain preserved on re-export.

WINDOWS VALIDATION:
1. Pull current upp_Ui and upp_uidesigner mains; record exact SHAs.
2. Run RunSupervisorValidation.ps1 with U++ 18468 / github / CLANGx64.
3. Build/run ExportedThemeContractTest explicitly in Debug; require exit 0 / failed=0.
4. Foundation generated fixture must compile and launch with compiled Pill/Dark theme.
5. Manual compare Designer Preview vs generated app for Light/Dark, recipe, active/disabled/reset
   instance overrides and unchanged structural configuration.
6. Launch generated executable from a different working directory; appearance must be unchanged.
7. ComponentOnly re-export must preserve user-owned code and initialize its compiled theme.

NEXT ACTION:
After exported-theme Windows/visual PASS, continue remaining public-control inventory:
UiDateTime, UiColorMatrix, UiMatrixSelector, UiChartRing, UiGallery.
No AI drawer/provider/AgentFlow runtime work before RC closure.
