# UiDesigner CodeGen

Deterministic C++/JSON generation from the canonical document and catalog.

The generator reads the same defaults and property IDs used by the Inspector and preview.
It emits stock U++ controls using their actual C++ classes and native Ui controls through
their runtime class names.


## Compiled ThemeDocument startup

Code generation remains ThemeCore-independent. Export may set the optional compiled-theme fields on `UiDesignerCodeGenerationOptions`; when enabled, `BuildGeneratedUi()` calls `UiTheme::Set(preset, mode)` before `BuildControls()`.

This is also the component-only initialization contract. The generated user constructor already calls `BuildGeneratedUi()`; an embedding application that uses the generated base directly must call that same build path before relying on control appearance. No runtime `theme.json` lookup is required. Theme recipes are already flattened into generated per-control style setup, with active instance overrides taking precedence.
