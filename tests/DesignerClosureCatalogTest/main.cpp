#include <Core/Core.h>
#include <Ui/Ui.h>
#include <UiDesigner/Services/UiDesignerSession.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include "../../UiDesigner/UiDesigner/UiDesignerVersion.h"

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
    const UiDesignerPropertySpec *tool_text = tool
        ? tool->FindProperty("text") : nullptr;
    Check(tool_text != nullptr,
          "ToolButton exposes text metadata");
    Check(tool_text && AsString(tool_text->default_value).IsEmpty() &&
          tool->defaults.Find("text") >= 0 &&
          AsString(tool->defaults.GetValue(tool->defaults.Find("text"))).IsEmpty(),
          "ToolButton defaults to icon-only empty text");

    const UiDesignerControlSpec *group = catalog.Find("UiGroupPanel");
    Check(group && group->FindProperty("subtitle"),
          "GroupPanel exposes subtitle metadata");
    const UiDesignerPropertySpec *group_icon = group
        ? group->FindProperty("icon") : nullptr;
    Check(group_icon && group_icon->custom_editor == "property.icon" &&
          AsString(group_icon->default_value) == "None",
          "GroupPanel exposes canonical icon chooser metadata");

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

    const UiDesignerControlSpec *doc = catalog.Find("UiDoc");
    const UiDesignerPropertySpec *doc_value = doc
        ? doc->FindProperty("value") : nullptr;
    Check(doc && doc->runtime_kind == UiDesignerRuntimeKind::UiDoc &&
          doc->data_capability == UiDesignerDataCapability::Scalar &&
          doc->data_adapter_id == "scalar" && doc_value,
          "UiDoc exposes one scalar Designer data contract");
    Check(doc && doc->data_defaults.Find("value") < 0 &&
          doc->defaults.Find("value") >= 0,
          "UiDoc scalar value is normal authored state, not duplicate data state");
    if(doc) {
        UiDesignerNode scalar_node;
        scalar_node.type = "UiDoc";
        scalar_node.SetProperty("value", "Projected scalar value");
        PropertyEditorModel scalar_projection;
        const bool projected = UiDesignerBuildScalarDataPropertyModel(
            *doc, scalar_node, scalar_projection);
        const PropertyEditorItem *value_item = scalar_projection.Find("value");
        Check(projected && value_item &&
              AsString(value_item->value) == "Projected scalar value" &&
              value_item->group == "Scalar",
              "UiDoc Data pane projection uses the canonical authored value property");
    }

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

    if(doc) {
        One<Ctrl> preview = UiDesignerPreviewFactory::Create(*doc);
        UiDoc *runtime = preview ? dynamic_cast<UiDoc *>(preview.Get()) : nullptr;
        Check(runtime != nullptr,
              "UiDoc preview creates the reusable runtime control");
        if(runtime) {
            const UiDesignerApplyResult applied =
                UiDesignerPreviewFactory::Apply(*runtime, *doc, "value",
                                                "Scalar preview contract");
            Check(applied != UiDesignerApplyResult::Rejected &&
                  AsString(runtime->GetData()) == "Scalar preview contract",
                  "UiDoc scalar value applies through the runtime SetData contract");
        }
    }

    if(group) {
        One<Ctrl> preview = UiDesignerPreviewFactory::Create(*group);
        Check(preview &&
              UiDesignerPreviewFactory::Apply(*preview, *group, "subtitle",
                                               "Metadata") !=
                  UiDesignerApplyResult::Rejected,
              "GroupPanel subtitle is live-previewable");
        if(preview) {
            Check(UiDesignerPreviewFactory::Apply(
                      *preview, *group, "icon", "ICON_DESIGN_WIDGETS_48") !=
                      UiDesignerApplyResult::Rejected,
                  "GroupPanel icon is live-previewable");
            Check(UiDesignerPreviewFactory::Apply(
                      *preview, *group, "icon", "None") !=
                      UiDesignerApplyResult::Rejected,
                  "GroupPanel icon can be cleared through Preview");
        }
    }

    UiDesignerCodeGenerator generator(catalog);

    session.NewDocument("blank");
    const UiDesignerNodeId range_id = session.AddControl("UiRangeSlider");
    Check(range_id != 0, "RangeSlider can be authored in a Designer document");
    UiDesignerGeneratedProject project = generator.Generate(
        session.Document(), "GeneratedRangeWindow");
    Check(project.IsValid(), "RangeSlider document generates a project");
    Check(project.generated_header.Find("UiRangeSlider") >= 0,
          "generated header declares UiRangeSlider directly");
    Check(project.generated_source.Find(".SetData(") >= 0,
          "generated RangeSlider setup uses its normal ValueArray data contract");
    Check(project.package.Find("\tUi;") >= 0,
          "generated package needs only the existing Ui dependency");

    session.NewDocument("blank");
    const UiDesignerNodeId doc_id = session.AddControl("UiDoc");
    Check(doc_id != 0, "UiDoc can be authored in a Designer document");
    session.Select(doc_id);
    error.Clear();
    Check(session.CommitProperty("value", "Scalar codegen contract", error),
          "UiDoc scalar value commits through the normal property command");
    UiDesignerGeneratedProject doc_project = generator.Generate(
        session.Document(), "GeneratedDocWindow");
    Check(doc_project.IsValid(), "UiDoc scalar document generates a project");
    Check(doc_project.generated_source.Find(
              ".SetData(\"Scalar codegen contract\")") >= 0,
          "generated UiDoc code uses the same scalar SetData value");

    session.NewDocument("blank");
    const UiDesignerNodeId group_id = session.AddControl("UiGroupPanel");
    Check(group_id != 0, "GroupPanel can be authored in a Designer document");
    session.Select(group_id);
    error.Clear();
    Check(session.CommitProperty("icon", "ICON_DESIGN_WIDGETS_48", error),
          "GroupPanel icon commits through normal Designer property state");
    UiDesignerGeneratedProject group_project = generator.Generate(
        session.Document(), "GeneratedGroupWindow");
    Check(group_project.IsValid(), "GroupPanel icon document generates a project");
    Check(group_project.generated_source.Find(
              ".SetIcon(ICON_DESIGN_WIDGETS_48())") >= 0,
          "generated GroupPanel code preserves the authored header icon");

    Check(String(UI_DESIGNER_VERSION) == "v1.0.1-RC2",
          "Designer closure bumps the visible release candidate version");

    Cout() << "DESIGNER_CLOSURE_CATALOG_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
