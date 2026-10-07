# Designer project fonts

The shared contract and acceptance handoff are in the sibling Ui repository:
`docs/PROJECT_FONTS.md` and `Ui/UiFonts.h`. This integration requires Ui commit
`2cc2df0` (or a descendant containing the shared font contract).

Load > Import Project fonts copies selected TTF/TrueType-outline OTF bytes and a
licence text into the existing project resource model. The import is an ordinary
session command with Undo/Redo. Theme Studio exposes Body / Heading / Code using
PropertyEditor's refreshed `property.font` selector; Project is the initial source
when populated, and System remains an explicitly labelled optional source.

Use `UiDesignerSession::ImportProjectFont` for authoring and
`UiDesignerImportFont` for building a detached/prepared document. Assets persist
stable IDs/hash/family/style metadata; native private aliases and face indices are
not authored identities. Legacy family-name fields still mean system fonts until
an explicit Project selection replaces them. Missing selections are retained.

`UiDesigner/Fonts` connects copied resources and ThemeCore's additive typography
map to the shared Ui catalogue. The project exporter packages selected families
and genuine styles with licences and a manifest. The generated first font
bootstrap member initializes before generated member controls/first layout,
including component-only exports. No source path, global installation, download
or user-owned main.cpp edit is needed at runtime.

`list_fonts` now returns objects with `selection`, `label`, `source`; assistant
validation, preview, local adapters and generated statements resolve the same
selections. The picker must follow UiFonts revision and show fallback diagnostics,
not maintain its own static face list or loading/theme authority.

Focused package: `tests/ProjectFontTest`. Its first argument chooses the export
fixture directory, optional second argument locates the Ui bundled font fixtures.
Example after building it:

```powershell
Start-Process build/ProjectFontTest.exe -WindowStyle Hidden -Wait -ArgumentList `
  'E:/apps/github/upp_Ui/build/font-work/DesignerAcceptance', `
  'E:/apps/github/upp_Ui/examples/UiTypographyDemo/fonts'
```

The resulting `project.uidesign.json`, `PackagedFonts` C++ package and `results.txt`
are actual save/load/export evidence. The main Ui handoff records fresh-process
compile/run and native checks. Windows private GDI/static TrueType is currently
the implemented adapter. CFF, collections, variable/color fonts, other platforms
and portable shaping remain unsupported/separate work. Real multi-monitor DPI
transitions and complex-script caret/selection are not claimed.

Final acceptance: ProjectFontTest 43/0, AssistantDesignerTests 318/0, existing
ExportedThemeContractTest 53/0, and the fresh-process generated application 9/0.
Logs and the generated fixture are under `E:/apps/github/upp_Ui/build/font-work`.
The full Release application is built and copied to `bin/UiDesigner.exe`.
