# ChartRing Designer integration test

`UID-CHARTRING-01` covers the headless segment contract, disposable editor draft,
normal Session command/undo/reset, Data/Inspector binding, actual Preview control,
Theme recipe/local override precedence, persistence, code generation and export.
The GUI-initialized executable exits automatically; it does not open the modal editor.

Run the focused source + generated-runtime proof from the Designer repository:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/ChartRingIntegrationTest/BuildGeneratedFixture.ps1 -DebugBuild
powershell -NoProfile -ExecutionPolicy Bypass -File tests/ChartRingIntegrationTest/BuildGeneratedFixture.ps1
```

The script builds/runs this suite, exports a Pill/Dark fixture with custom and
automatic segment colours plus an empty chart, builds the complete application,
and compiles/runs independent generated-control verifiers for CompleteCppPackage
and ComponentOnly. The verifier programs link only Ui/CtrlLib and generated files.
Both execute from a foreign working directory after optional theme.json files
have been removed. Assertions inspect real generated controls, not metadata alone.
Logs are retained under build/UID-CHARTRING-01-<configuration>-<timestamp>.
The generated source subtree is retained only on failure.

Manual acceptance remains required in the actual Designer:
- Inspector and Data both open the same segment collection editor;
- edit label/value, automatic/custom colour, add/remove/reorder;
- Apply produces one undoable command; Cancel/escape/window-close produces none;
- collection reset, multi-selection, save/load and local/inherited styles work;
- resize the dialog and inspect its control layout;
- compare Designer Preview with the generated application.

All added check counts are reported by the executable. No test-count assumptions
are a substitute for failed=0 and process exit 0.
