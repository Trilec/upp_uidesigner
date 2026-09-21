# DateTime integration gate

From the Designer repository root, with the current github assembly and CLANGx64:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/DateTimeIntegrationTest/BuildGeneratedFixture.ps1 -DebugBuild
powershell -NoProfile -ExecutionPolicy Bypass -File tests/DateTimeIntegrationTest/BuildGeneratedFixture.ps1
```

The script compiles/runs the GUI-linked integration suite with a real GUI lifecycle,
exports and compiles a complete application, then compiles/runs separate complete
and ComponentOnly runtime verifiers. Those verifiers link only generated code and
Ui; their working directory differs from the package and theme.json is removed.
Both process exit 0 and explicit passing summaries are required. Logs and failed
fixtures are retained. The complete supervisor runner also includes the Release gate.

Expected markers (report the actual count, not a guessed baseline):
- DATETIME_INTEGRATION checks=<actual> failed=0
- DATETIME_GENERATED_RUNTIME failed=0 (complete and component)

The suite covers strict ISO/null decoding, catalog/automatic editor registration,
canonical Inspector/Data binding, native Preview, mode/seconds source preservation,
configuration, Theme/data isolation, undo/redo, JSON round-trip and generated Time
constructors. Invalid external data is blocked by catalog validation/export.

Desktop acceptance remains required: type/pick Date, Time and DateTime through
Inspector and Data, clear/reset, edit range/policies, change modes, undo/redo, save/load,
Light/Dark/Light, read-only presentation and Preview/export side-by-side comparison.
See docs/DATETIME_INTEGRATION.md for the authority and scope boundaries.
