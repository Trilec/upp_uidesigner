#include <Core/Core.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

namespace {
int checks = 0;
int failed = 0;

void Check(bool ok, const String& what)
{
    checks++;
    if(ok)
        Cout() << "PASS: " << what << '\n';
    else {
        failed++;
        Cout() << "FAIL: " << what << '\n';
    }
}

const UiDesignerThemeOverrideSpec* Find(const UiDesignerControlSpec& spec,
                                        const char *id)
{
    return spec.FindThemeOverride(id);
}

void CheckEntry(const UiDesignerControlSpec& spec, const char *id,
                const char *label, const char *group)
{
    const UiDesignerThemeOverrideSpec *p = Find(spec, id);
    Check(p != nullptr, String(id) + " exists");
    if(p) {
        Check(p->label == label, String(id) + " label");
        Check(p->group == group, String(id) + " group");
        Check(p->adapter_field_id == id, String(id) + " adapter field");
    }
}
}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);

    String error;
    Check(catalog.Validate(error), "catalog validates: " + error);

    const UiDesignerControlSpec *spec = catalog.Find("UiLabel");
    Check(spec != nullptr, "UiLabel catalog spec exists");
    if(!spec) {
        Cout() << "LABEL_THEME_ADAPTER_SUMMARY checks=" << checks
               << " failed=" << failed << '\n';
        SetExitCode(1);
        return;
    }

    Check(spec->theme_adapter_id == "label", "UiLabel uses label adapter id");
    const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*spec);
    Check(adapter != nullptr, "Label theme adapter resolves");
    if(!adapter) {
        Cout() << "LABEL_THEME_ADAPTER_SUMMARY checks=" << checks
               << " failed=" << failed << '\n';
        SetExitCode(1);
        return;
    }

    Check(String(adapter->Id()) == "label", "dedicated adapter id is label");
    Check(adapter->Supports(UiDesignerRuntimeKind::UiLabel),
          "adapter supports UiLabel");
    Check(spec->theme_overrides.GetCount() >= 56,
          "reference Label schema has full non-resource coverage");
    Check(spec->theme_overrides.GetCount() > 0 &&
          spec->theme_overrides[0].id == "radius",
          "reference Label schema starts with General");

    Check(spec->FindThemeOverride("skin_image") == nullptr,
          "Designer does not fake resource-backed Skin before adapter resource resolution exists");

    CheckEntry(*spec, "radius", "Radius", "General");
    CheckEntry(*spec, "transparent", "Transparent", "General");
    CheckEntry(*spec, "high_contrast", "High contrast", "General");
    CheckEntry(*spec, "face_enabled", "Enabled", "Face");
    CheckEntry(*spec, "face.normal", "Normal", "Face");
    CheckEntry(*spec, "frame_enabled", "Enabled", "Frame");
    CheckEntry(*spec, "frame_width", "Width", "Frame");
    CheckEntry(*spec, "ink.normal", "Normal", "Ink");
    CheckEntry(*spec, "icon.normal", "Normal", "Icon");
    CheckEntry(*spec, "margin_left", "Left", "Content Margin");
    CheckEntry(*spec, "focus_enabled", "Enabled", "Focus");
    CheckEntry(*spec, "shadow_curve", "Falloff curve", "Shadow");
    CheckEntry(*spec, "highlight_style", "Style", "Highlight");

    const UiDesignerThemeOverrideSpec *face_normal = Find(*spec, "face.normal");
    Check(face_normal && face_normal->kind == PropertyEditorKind::FillRecipe,
          "Face state uses shared FillRecipe editor contract");

    for(const UiDesignerThemeOverrideSpec& p : spec->theme_overrides) {
        Check(p.group != "Frame and Face",
              "retired Frame and Face group absent: " + p.id);
        Check(p.group != "Text Ink",
              "retired Text Ink group absent: " + p.id);
        Check(p.group != "Icon Ink",
              "retired Icon Ink group absent: " + p.id);
    }

    UiDesignerNode node;
    node.id = 7;
    node.type = "UiLabel";
    node.SetProperty("role", "Standard");
    node.theme_overrides.Set("radius", 17);
    node.theme_overrides.Set("frame_width", 3);
    node.theme_overrides.Set("ink.normal", Color(12, 34, 56));

    UiDesignerFillRecipe face_recipe;
    face_recipe.mode = "QuadGradient";
    face_recipe.top_left = Color(1, 2, 3);
    face_recipe.top_right = Color(4, 5, 6);
    face_recipe.bottom_left = Color(7, 8, 9);
    face_recipe.bottom_right = Color(10, 11, 12);
    face_recipe.tile_size = 28;
    face_recipe.blur = 2;
    node.theme_overrides.Set("face.normal", face_recipe.ToValue());

    Check((int)adapter->ResolveFieldValue(node, *spec, "radius") == 17,
          "radius override resolves through adapter");
    Check((int)adapter->ResolveFieldValue(node, *spec, "frame_width") == 3,
          "frame width override resolves through adapter");
    Check(Color(adapter->ResolveFieldValue(node, *spec, "ink.normal")) ==
              Color(12, 34, 56),
          "ink state override resolves through adapter");

    UiDesignerFillRecipe resolved_face = UiDesignerFillRecipe::FromValue(
        adapter->ResolveFieldValue(node, *spec, "face.normal"));
    Check(resolved_face.mode == "QuadGradient",
          "Face recipe resolves without lossy UiFill round-trip");
    Check(resolved_face.top_left == Color(1, 2, 3) &&
          resolved_face.bottom_right == Color(10, 11, 12),
          "Face recipe preserves authored gradient colours");
    Check(resolved_face.tile_size == 28 && resolved_face.blur == 2,
          "Face recipe preserves authored gradient geometry");

    String code;
    adapter->EmitSetup(code, "label", node, *spec);
    Check(code.Find("UiLabel::Style label_style = UiTheme::ResolveLabel(UiRole::Standard);") >= 0,
          "codegen starts from inherited Label role");
    Check(code.Find("label_style.metrics.radius = 17;") >= 0,
          "codegen emits radius");
    Check(code.Find("label_style.metrics.frame_width = 3;") >= 0,
          "codegen emits frame width");
    Check(code.Find("label_style.palette.ink[ST_NORMAL] = Color(12, 34, 56);") >= 0,
          "codegen emits state ink");
    Check(code.Find("label_style.palette.face[ST_NORMAL] = UiFill::ImageFill(MakeQuadGradientTile(") >= 0,
          "codegen emits authored Face FillRecipe");
    Check(code.Find("label.SetCustomStyle(label_style);") >= 0,
          "codegen applies resolved custom style");

    Cout() << "LABEL_THEME_ADAPTER_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
