#include "ModelDataFixture.h"
#include <Core/Core.h>
#include <Ui/UiProgressRing.h>
#include <Ui/UiRangeSliderEdit.h>
#include <Ui/UiTheme.h>
#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
    int checks = 0, failed = 0;
    auto Check = [&](bool ok, const char* label) {
        checks++;
        if(!ok) { failed++; Cout() << "FAIL: " << label << '\n'; }
    };
    UiDesignerSession session;
    session.NewDocument("blank");
    const auto* spec = session.Catalog().Find("UiProgressRing");
    Check(spec && spec->runtime_kind == UiDesignerRuntimeKind::UiProgressRing,
          "ProgressRing has an explicit production kind");
    if(!spec) { SetExitCode(1); return; }
    Check(spec->codegen && spec->preview && spec->inspector && spec->theme,
          "ProgressRing declares the complete authoring path");
    Check(spec->events.IsEmpty(), "presentation-only progress invents no action events");
    String error;
    Check(session.Catalog().Validate(error), "application catalog validates");
    auto id = session.AddControl("UiProgressRing");
    if(!id || !session.Document().Find(id)) { SetExitCode(1); return; }
    session.Select(id);
    Check(id != 0 && session.CommitProperty("total", 200, error) &&
          session.CommitProperty("value", 75, error), "canonical scalar progress commits");
    Check(session.CommitProperty("animate_on_show", false, error), "animation is document-owned");
    Check(session.CommitThemeOverride("cap_roundness", 35, error), "current cap-roundness override commits");
    Check(!spec->FindThemeOverride("total") && !spec->FindThemeOverride("value") &&
          !spec->FindThemeOverride("animate_on_show"), "theme does not own progress/configuration");
    PropertyEditorModel data;
    Check(UiDesignerBuildScalarDataPropertyModel(*spec, *session.Document().Find(id), data) &&
          data.Find("value") && (int)data.Find("value")->value == 75,
          "Data projects the same canonical scalar as Inspector");
    const auto* theme = UiDesignerGetThemeAdapter(*spec);
    Check(theme && theme->Supports(spec->runtime_kind), "ProgressRing theme adapter resolves");
    if(theme) for(const auto& p : spec->theme_overrides) {
        PropertyEditorModel model;
        p.AddTo(model, p.default_value);
        Check(theme->HasField(p.adapter_field_id) && model.Find(p.id), "theme field projects through PropertyEditor");
    }
    UiDesignerPreviewCanvas canvas;
    canvas.SetCatalog(&session.Catalog());
    canvas.SetDocument(&session.Document());
    canvas.RebuildDocument();
    auto* ring = dynamic_cast<UiProgressRing*>(canvas.FindRuntime(id));
    Check(ring && ring->Get() == 75 && ring->GetTotal() == 200 &&
          ring->GetCapRoundness() == 35 && !ring->IsAnimateOnShow(),
          "Preview uses real UiProgressRing with authored state and style");
    if(ring && theme) {
        const auto& node = *session.Document().Find(id);
        theme->ApplyPreviewStyle(*ring, node, *spec);
        Check(ring->Get() == 75 && ring->GetTotal() == 200 && !ring->IsAnimateOnShow(),
              "theme reapply preserves semantic state and animation configuration");
        UiDesignerPreviewFactory::Apply(*ring, *spec, "center_text", "Loading");
        Check(ring->GetText() == "Loading", "center text applies");
        UiDesignerPreviewFactory::Apply(*ring, *spec, "center_text", String());
        Check(ring->GetText().IsEmpty(), "empty center text restores automatic text");
    }
    UiDesignerCodeGenerator generator(session.Catalog());
    auto project = generator.Generate(session.Document(), "RingFixture");
    Check(project.IsValid() && project.generated_header.Find("UiProgressRing") >= 0 &&
          project.generated_source.Find(".Set(75, 200)") >= 0 &&
          project.generated_source.Find(".cap_roundness = 35") >= 0 &&
          project.generated_source.Find(".AnimateOnShow(false)") >= 0,
          "generated setup preserves current ring API/value/configuration");
    Check(project.package.Find("\tUi;") >= 0, "generated ring links reusable Ui package");
    UiDesignerDocument loaded;
    Check(UiDesignerDeserialize(UiDesignerSerialize(session.Document(), true), loaded, error) &&
          loaded.Find(id) && (int)loaded.Find(id)->GetProperty("total", 0) == 200,
          "ring state round-trips");
    Check(session.CommitProperty("total", 0, error), "indeterminate mode commits through total");
    project = generator.Generate(session.Document(), "IndeterminateFixture");
    Check(project.generated_source.Find(".Set(75, 0)") >= 0, "indeterminate generation retains canonical value");

    session.Document().SetThemeOverrideActive(id, "cap_roundness", false);
    project = generator.Generate(session.Document(), "InactiveOverrideFixture");
    Check(project.generated_source.Find(".cap_roundness = 35") < 0,
          "disabled ring override is not generated");
    session.NewDocument("blank");
    Check(session.AddControl("UiRangeSlider") != 0 && session.AddControl("UiNodeGraph") != 0,
          "adapter-backed controls author successfully");
    for(const char* type : {"UiRangeSlider", "UiNodeGraph"}) {
        const auto* adapter_spec = session.Catalog().Find(type);
        Check(adapter_spec && UiDesignerPreviewFactory::Adapter(*adapter_spec),
              "production adapter resolves through real registry");
        if(adapter_spec) {
            auto runtime = UiDesignerPreviewFactory::Create(*adapter_spec);
            Check(runtime && (String(type) == "UiRangeSlider"
                  ? dynamic_cast<UiRangeSlider*>(runtime.Get()) != nullptr
                  : dynamic_cast<UiNodeGraph*>(runtime.Get()) != nullptr),
                  "production adapter constructs the authoritative Ui Ctrl");
        }
    }
    project = generator.Generate(session.Document(), "AdapterFixture");
    Check(project.IsValid(), "registered RangeSlider and NodeGraph remain exportable");

    session.NewDocument("blank");
    auto range_id = session.AddControl("UiRangeSliderEdit");
    Check(range_id != 0, "RangeSliderEdit authors");
    if(!range_id) { SetExitCode(1); return; }
    const auto* range_spec = session.Catalog().Find("UiRangeSliderEdit");
    Check(range_spec && range_spec->codegen && range_spec->adapter_backed_runtime &&
          !range_spec->FindProperty("role") && range_spec->theme_overrides.IsEmpty(),
          "composition has an explicit adapter contract and no invented style family");
    Check(session.CommitProperty("range", PropertyEditorMakeVector(-100.0, 500.0), error) &&
          session.CommitProperty("step", 0.5, error) &&
          session.CommitProperty("precision", 2, error) &&
          session.CommitProperty("value", PropertyEditorMakeVector(-20.5, 120.5), error) &&
          session.CommitProperty("direction", "V", error) &&
          session.CommitProperty("field_width", 88, error) &&
          session.CommitProperty("gap", 9, error) &&
          session.CommitProperty("inset", 4, error), "range value and configuration commit separately");
    Check(UiDesignerBuildScalarDataPropertyModel(*range_spec, *session.Document().Find(range_id), data) &&
          (double)data.Find("value")->minimum == -100 &&
          (double)data.Find("value")->maximum == 500 &&
          data.Find("value")->decimals == 2,
          "Data editor uses authored domain and precision");
    Check(session.InspectorModel().Find("value") &&
          (double)session.InspectorModel().Find("value")->maximum == 500 &&
          session.InspectorModel().Find("value")->decimals == 2,
          "Inspector rebuild honors configuration schema impact");
    canvas.RebuildDocument();
    auto* range_runtime = dynamic_cast<UiRangeSliderEdit*>(canvas.FindRuntime(range_id));
    Check(range_runtime && range_runtime->GetLowerValue() == -20.5 &&
          range_runtime->GetUpperValue() == 120.5 && range_runtime->GetMin() == -100 &&
          range_runtime->GetMax() == 500 && range_runtime->GetStep() == 0.5 &&
          range_runtime->GetDirection() == UiDirection::V &&
          range_runtime->GetFieldWidth() == DPI(88) && range_runtime->GetGap() == DPI(9) &&
          range_runtime->GetInset() == DPI(4), "real range composition preserves complete configuration");
    project = generator.Generate(session.Document(), "RangeEditFixture");
    Check(project.IsValid() && project.generated_header.Find("UiRangeSliderEdit") >= 0 &&
          project.generated_source.Find(".SetRange(-100, 500)") >= 0 &&
          project.generated_source.Find(".SetStep(0.5)") >= 0 &&
          project.generated_source.Find(".SetDirection(UiDirection::V)") >= 0 &&
          project.generated_source.Find(".SetFieldWidth(DPI(88))") >= 0 &&
          project.generated_source.Find(".SetPrecision(2)") >= 0,
          "generated range setup uses current composition API");
    Check(UiDesignerDeserialize(UiDesignerSerialize(session.Document(), true), loaded, error) &&
          loaded.Find(range_id) && loaded.Find(range_id)->GetProperty("value").Is<ValueArray>() &&
          ((ValueArray)loaded.Find(range_id)->GetProperty("value")).GetCount() == 2,
          "range remains one two-element value across serialization");

    session.NewDocument("blank");
    UiDesignerNodeId model_box = session.AddControl("UiBoxLayout"), list_id, tree_id;
    Check(AddAuthoredModelFixture(session, model_box, list_id, tree_id),
          "authored list and nested tree fixture commits through Commands");
    canvas.RebuildDocument();
    auto* list_runtime = dynamic_cast<UiList*>(canvas.FindRuntime(list_id));
    auto* tree_runtime = dynamic_cast<UiTree*>(canvas.FindRuntime(tree_id));
    Check(list_runtime && list_runtime->Model().GetCount() == 1 &&
          list_runtime->Model().Get(0).description == "Detail" &&
          list_runtime->Model().Get(0).right_text == "Badge" &&
          list_runtime->Model().Get(0).checked && !list_runtime->Model().Get(0).enabled,
          "real Preview preserves list item fields");
    Check(tree_runtime && tree_runtime->Model().Get(tree_runtime->Model().Root()).text == "Authored root",
          "real Preview preserves authored tree root");
    project = generator.Generate(session.Document(), "ModelFixture");
    Check(project.IsValid() && project.generated_source.Find("Authored list row") >= 0 &&
          project.generated_source.Find("item.description = \"Detail\"") >= 0 &&
          project.generated_source.Find("item.right_text = \"Badge\"") >= 0 &&
          project.generated_source.Find("Color(12, 34, 56)") >= 0 &&
          project.generated_source.Find("model.Set(model.Root(), UiModelItem(\"Authored root\"") >= 0 &&
          project.generated_source.Find("Nested authored leaf") >= 0 &&
          project.generated_source.Find("model.AddChild(child_1, item)") >= 0,
          "generated models preserve authored items, hierarchy and typed nested payloads");

    UiDesignerControlSpec placeholder;
    placeholder.type_id = "UnimplementedFixture";
    placeholder.display_name = "Unimplemented fixture";
    placeholder.runtime_cpp_type = "Ctrl";
    placeholder.default_base_name = "unimplemented";
    placeholder.preview_adapter_id = "runtime:UnimplementedFixture";
    placeholder.codegen_adapter_id = "control";
    placeholder.child_adapter_id = "none";
    placeholder.theme = false;
    AddUiDesignerCommonProperties(placeholder);
    session.Catalog().Register(pick(placeholder));
    session.NewDocument("blank");
    Check(session.AddControl("UnimplementedFixture") != 0, "placeholder fixture authors successfully");
    project = generator.Generate(session.Document(), "BlockedFixture");
    Check(!project.IsValid() && project.generated_source.IsEmpty() &&
          Join(project.diagnostics, " ").Find("Placeholder control") >= 0,
          "placeholder cannot masquerade as production code");

    auto* disabled = const_cast<UiDesignerControlSpec*>(session.Catalog().Find("UiProgressRing"));
    disabled->codegen = false;
    session.NewDocument("blank");
    Check(session.AddControl("UiProgressRing") != 0, "non-exportable fixture authors successfully");
    project = generator.Generate(session.Document(), "DisabledFixture");
    Check(!project.IsValid() && project.generated_source.IsEmpty(), "codegen=false is enforced at export");
    Cout() << "CURRENT_UI_INTEGRATION checks=" << checks << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
