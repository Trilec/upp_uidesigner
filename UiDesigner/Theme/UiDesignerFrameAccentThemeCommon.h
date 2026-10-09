#ifndef _UiDesigner_Theme_UiDesignerFrameAccentThemeCommon_h_
#define _UiDesigner_Theme_UiDesignerFrameAccentThemeCommon_h_

#include "UiDesignerNormalizedThemeCommon.h"

namespace Upp {
namespace UiDesignerFrameAccentTheme {

inline String FieldId(const String& prefix, const String& leaf)
{
    return prefix.IsEmpty() ? String(leaf) : prefix + "_" + leaf;
}

inline int Edge(const String& prefix, const String& id)
{
    if(id == FieldId(prefix, "frame_accent_top")) return StyledFrameAccent::Top;
    if(id == FieldId(prefix, "frame_accent_bottom")) return StyledFrameAccent::Bottom;
    if(id == FieldId(prefix, "frame_accent_left")) return StyledFrameAccent::Left;
    if(id == FieldId(prefix, "frame_accent_right")) return StyledFrameAccent::Right;
    return StyledFrameAccent::None;
}

inline bool HasField(const String& prefix, const String& id)
{
    return Edge(prefix, id) != StyledFrameAccent::None ||
           id == FieldId(prefix, "frame_accent_thickness") ||
           id == FieldId(prefix, "frame_accent_color") ||
           id == FieldId(prefix, "frame_accent_alpha");
}

inline void AddFields(UiDesignerControlSpec& spec, const String& prefix,
                      const String& root, const StyledMetrics& metrics)
{
    using namespace UiDesignerNormalizedTheme;
    const String group = root.IsEmpty() ? String("Frame Accent") : root + " / Frame Accent";
    static const char *sides[] = { "top", "bottom", "left", "right" };
    static const char *labels[] = { "Top", "Bottom", "Left", "Right" };
    for(int i = 0; i < 4; ++i) {
        const String id = FieldId(prefix, "frame_accent_" + String(sides[i]));
        Add(spec, id, labels[i], group, PropertyEditorKind::Boolean,
            (metrics.frame_accent.edges & Edge(prefix, id)) != 0)
            .Help("Accent this edge along the existing corner radius. Independent of the normal frame; all edges off disables the accent.");
    }
    Add(spec, FieldId(prefix, "frame_accent_thickness"), "Thickness", group,
        PropertyEditorKind::NumericInt, metrics.frame_accent.thickness).Range(0, 60, 1)
        .Help("Width painted inside the surface boundary; does not change content layout. Zero hides the accent.");
    Add(spec, FieldId(prefix, "frame_accent_color"), "Colour", group,
        PropertyEditorKind::Color, metrics.frame_accent.color)
        .Help("Accent colour. An inherited/null colour uses the normal frame colour for the current interaction state.");
    Add(spec, FieldId(prefix, "frame_accent_alpha"), "Alpha", group,
        PropertyEditorKind::NumericInt, metrics.frame_accent.alpha).Range(0, 255, 1)
        .Help("Accent opacity from transparent (0) to opaque (255).");
}

inline Value Read(const StyledMetrics& metrics, const String& prefix, const String& id)
{
    const int edge = Edge(prefix, id);
    if(edge) return (metrics.frame_accent.edges & edge) != 0;
    if(id == FieldId(prefix, "frame_accent_thickness")) return metrics.frame_accent.thickness;
    if(id == FieldId(prefix, "frame_accent_color")) return metrics.frame_accent.color;
    if(id == FieldId(prefix, "frame_accent_alpha")) return metrics.frame_accent.alpha;
    return Value();
}

inline bool Apply(StyledMetrics& metrics, const String& prefix,
                  const String& id, const Value& value)
{
    const int edge = Edge(prefix, id);
    if(edge) {
        if((bool)value) metrics.frame_accent.edges |= edge;
        else metrics.frame_accent.edges &= ~edge;
    }
    else if(id == FieldId(prefix, "frame_accent_thickness"))
        metrics.frame_accent.thickness = minmax((int)value, 0, 60);
    else if(id == FieldId(prefix, "frame_accent_color"))
        metrics.frame_accent.color = (Color)value;
    else if(id == FieldId(prefix, "frame_accent_alpha"))
        metrics.frame_accent.alpha = minmax((int)value, 0, 255);
    else return false;
    return true;
}

inline bool Emit(String& out, const String& metrics, const String& prefix,
                 const String& id, const Value& value)
{
    using namespace UiDesignerNormalizedTheme;
    const int edge = Edge(prefix, id);
    if(edge) {
        const char *name = edge == StyledFrameAccent::Top ? "Top" :
                           edge == StyledFrameAccent::Bottom ? "Bottom" :
                           edge == StyledFrameAccent::Left ? "Left" : "Right";
        out << "\t" << metrics << ".frame_accent.edges "
            << ((bool)value ? "|= " : "&= ~") << "StyledFrameAccent::" << name << ";\n";
    }
    else if(id == FieldId(prefix, "frame_accent_thickness"))
        out << "\t" << metrics << ".frame_accent.thickness = " << minmax((int)value, 0, 60) << ";\n";
    else if(id == FieldId(prefix, "frame_accent_color"))
        out << "\t" << metrics << ".frame_accent.color = " << EmitValue(value) << ";\n";
    else if(id == FieldId(prefix, "frame_accent_alpha"))
        out << "\t" << metrics << ".frame_accent.alpha = " << minmax((int)value, 0, 255) << ";\n";
    else return false;
    return true;
}

}
}
#endif
