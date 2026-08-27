#include <Core/Core.h>
#include <Ui/Ui.h>
#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
    int checks = 0;
    int failed = 0;
    auto Check = [&](bool condition, const char *label) {
        checks++;
        if(!condition) {
            failed++;
            Cout() << "FAIL: " << label << '\n';
        }
    };

    UiDesignerSession session;
    UiDesignerCatalog& catalog = session.Catalog();
    String error;
    Check(catalog.Validate(error), "application catalog validates");
    if(!error.IsEmpty())
        Cout() << "CATALOG: " << error << '\n';

    const UiDesignerControlSpec *tool = catalog.Find("UiToolButton");
    Check(tool && tool->FindProperty("text"),
          "ToolButton exposes text metadata");

    const UiDesignerControlSpec *group = catalog.Find("UiGroupPanel");
    Check(group && group->FindProperty("subtitle"),
          "GroupPanel exposes subtitle metadata");

    static const char *sizes[] = {
        "fixed_width", "fixed_height", "min_width", "min_height",
        "max_width", "max_height"
    };
    bool sizing_ok = group != nullptr;
    if(group)
        for(const char *id : sizes) {
            const UiDesignerPropertySpec *property = group->FindProperty(id);
            sizing_ok &= property &&
                         property->custom_editor ==
                             "property.numeric-int-working-range" &&
                         property->editor_variant == "0:500" &&
                         (int)property->maximum == 10000;
        }
    Check(sizing_ok,
          "common sizing keeps legal 10000 bounds with 0-500 working range");
    Check(group && (int)group->FindProperty("max_width")->minimum == 0 &&
                  (int)UiDesignerMapValue(group->defaults, "max_width", -1) == 0,
          "maximum size zero remains the unbounded sentinel");

    const UiDesignerControlSpec *range = catalog.Find("UiRangeSlider");
    Check(range && range->runtime_cpp_type == "UiRangeSlider" &&
          range->FindProperty("value") &&
          range->FindProperty("value")->custom_editor == "property.range.double",
          "real UiRangeSlider is exposed with range editor metadata");

    const UiDesignerControlSpec *graph = catalog.Find("UiNodeGraph");
    Check(graph && graph->runtime_cpp_type == "UiNodeGraph" &&
          graph->data_capability == UiDesignerDataCapability::Unsupported,
          "real UiNodeGraph is exposed without inventing Designer graph data");
    Check(catalog.FindPreset("Demo") != nullptr,
          "Demo preset is registered in the application catalog");

    if(range) {
        One<Ctrl> preview = UiDesignerPreviewFactory::Create(*range);
        Check(preview && dynamic_cast<UiRangeSlider *>(preview.Get()),
              "RangeSlider preview creates the reusable runtime control");
        if(preview) {
            ValueArray values;
            values.Add(12.0);
            values.Add(88.0);
            UiDesignerPreviewFactory::Apply(*preview, *range, "value", values);
            UiRangeSlider *runtime = dynamic_cast<UiRangeSlider *>(preview.Get());
            Check(runtime && runtime->GetLowerValue() == 12.0 &&
                             runtime->GetUpperValue() == 88.0,
                  "RangeSlider preview applies authored range values");
        }
    }

    if(graph) {
        One<Ctrl> preview = UiDesignerPreviewFactory::Create(*graph);
        UiNodeGraph *runtime = preview
            ? dynamic_cast<UiNodeGraph *>(preview.Get()) : nullptr;
        Check(runtime && runtime->Model().GetNodeCount() == 3,
              "NodeGraph preview uses a genuine three-node model sample");
    }

    if(group) {
        One<Ctrl> preview = UiDesignerPreviewFactory::Create(*group);
        Check(preview &&
              UiDesignerPreviewFactory::Apply(*preview, *group, "subtitle",
                                               "Metadata") !=
                  UiDesignerApplyResult::Rejected,
              "GroupPanel subtitle is live-previewable");
    }

    session.NewDocument("blank");
    const UiDesignerNodeId range_id = session.AddControl("UiRangeSlider");
    Check(range_id != 0, "RangeSlider can be authored in a Designer document");
    UiDesignerCodeGenerator generator(catalog);
    UiDesignerGeneratedProject project = generator.Generate(
        session.Document(), "GeneratedRangeWindow");
    Check(project.IsValid(), "RangeSlider document generates a project");
    Check(project.generated_header.Find("UiRangeSlider") >= 0,
          "generated header declares UiRangeSlider directly");
    Check(project.generated_source.Find(".SetData(") >= 0,
          "generated RangeSlider setup uses its normal ValueArray data contract");
    Check(project.package.Find("\tUi;") >= 0,
          "generated package needs only the existing Ui dependency");

    Cout() << "DESIGNER_CLOSURE_CATALOG_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
