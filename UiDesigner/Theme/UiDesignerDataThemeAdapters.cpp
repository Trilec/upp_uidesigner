#include "UiDesignerStyledThemeCommon.h"
#include <Ui/UiTable.h>
#include <Ui/UiBreadcrumbs.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;
using namespace UiDesignerStyledTheme;

// These controls have additional semantic colours alongside their styled surface.
// One descriptor drives discovery, application, resolution and generated C++.
template <class S>
struct ColorField : Moveable<ColorField<S>> {
    const char* id;
    Color S::* member;
    ColorField(const char* i, Color S::* m) : id(i), member(m) {}
};
template <class S>
struct IntField : Moveable<IntField<S>> {
    const char* id;
    int S::* member;
    IntField(const char* i, int S::* m) : id(i), member(m) {}
};
struct TableTraits {
    using Control = UiTable;
    using Style = UiTable::Style;
    static Font& Heading(Style& s) { return s.header_font; }
    static const char* HeadingMember() { return "header_font"; }
    static const Vector<IntField<Style>>& Numbers() {
        static Vector<IntField<Style>> fields;
        if(fields.IsEmpty()) {
            fields.Add(IntField<Style>{"row_height", &Style::row_height});
            fields.Add(IntField<Style>{"header_height", &Style::header_height});
        }
        return fields;
    }
    static const char* Id() { return "table"; }
    static const char* Type() { return "UiTable"; }
    static UiDesignerRuntimeKind Kind() { return UiDesignerRuntimeKind::UiTable; }
    static Style Base(UiRole role = UiRole::Standard) { return UiTheme::ResolveTable(role); }
    static String BaseCode(const Value& role) { return "UiTheme::ResolveTable(" + RoleExpr(role) + ")"; }
    static const Vector<ColorField<Style>>& Colors() {
        static Vector<ColorField<Style>> fields;
        if(fields.IsEmpty()) {
            fields.Add(ColorField<Style>{"table_bg", &Style::table_bg});
            fields.Add(ColorField<Style>{"header_bg", &Style::header_bg});
            fields.Add(ColorField<Style>{"header_hot_bg", &Style::header_hot_bg});
            fields.Add(ColorField<Style>{"header_ink", &Style::header_ink});
            fields.Add(ColorField<Style>{"row_header_bg", &Style::row_header_bg});
            fields.Add(ColorField<Style>{"cell_ink", &Style::cell_ink});
            fields.Add(ColorField<Style>{"muted_ink", &Style::muted_ink});
            fields.Add(ColorField<Style>{"grid_color", &Style::grid_color});
            fields.Add(ColorField<Style>{"alternate_row_bg", &Style::alternate_row_bg});
            fields.Add(ColorField<Style>{"hover_bg", &Style::hover_bg});
            fields.Add(ColorField<Style>{"selection_bg", &Style::selection_bg});
            fields.Add(ColorField<Style>{"selection_border", &Style::selection_border});
            fields.Add(ColorField<Style>{"active_bg", &Style::active_bg});
            fields.Add(ColorField<Style>{"active_border", &Style::active_border});
            fields.Add(ColorField<Style>{"read_only_bg", &Style::read_only_bg});
            fields.Add(ColorField<Style>{"warning_bg", &Style::warning_bg});
            fields.Add(ColorField<Style>{"error_bg", &Style::error_bg});
            fields.Add(ColorField<Style>{"resize_guide", &Style::resize_guide});
        }
        return fields;
    }
};
struct BreadcrumbTraits {
    using Control = UiBreadcrumbs;
    using Style = UiBreadcrumbs::Style;
    static Font& Heading(Style& s) { return s.current_font; }
    static const char* HeadingMember() { return "current_font"; }
    static const Vector<IntField<Style>>& Numbers() { static Vector<IntField<Style>> fields; return fields; }
    static const char* Id() { return "breadcrumbs"; }
    static const char* Type() { return "UiBreadcrumbs"; }
    static UiDesignerRuntimeKind Kind() { return UiDesignerRuntimeKind::UiBreadcrumbs; }
    static Style Base(UiRole role = UiRole::Standard) { return UiBreadcrumbs::ResolveThemeStyle(role); }
    static String BaseCode(const Value& role) { return "UiBreadcrumbs::ResolveThemeStyle(" + RoleExpr(role) + ")"; }
    static const Vector<ColorField<Style>>& Colors() {
        static Vector<ColorField<Style>> fields;
        if(fields.IsEmpty()) {
            fields.Add(ColorField<Style>{"text_ink", &Style::text_ink});
            fields.Add(ColorField<Style>{"current_ink", &Style::current_ink});
            fields.Add(ColorField<Style>{"divider_ink", &Style::divider_ink});
            fields.Add(ColorField<Style>{"current_underline", &Style::current_underline});
        }
        return fields;
    }
};

template <class Traits>
class DataThemeAdapter final : public UiDesignerThemeAdapter {
    using S = typename Traits::Style;
    static bool Apply(S& s, const String& id, const Value& v) {
        if(ApplyPaletteMetrics(s.palette, s.metrics, "", id, v)) return true;
        for(const auto& f : Traits::Numbers()) if(id == f.id) { s.*(f.member) = (int)v; return true; }
        for(const auto& f : Traits::Colors()) if(id == f.id) { s.*(f.member) = (Color)v; return true; }
        if(id == "title_font_face") Traits::Heading(s).FaceName(AsString(v));
        else if(id == "title_font_height") Traits::Heading(s).Height((int)v);
        else if(id == "title_font_bold") Traits::Heading(s).Bold((bool)v);
        else if(id == "font_face") s.font.FaceName(AsString(v));
        else if(id == "font_height") s.font.Height((int)v);
        else if(id == "font_bold") s.font.Bold((bool)v);
        else return false;
        return true;
    }
    static S Resolve(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                     const UiDesignerTransientOverlay* overlay) {
        S s = Traits::Base(Role(node.GetProperty("role", "Standard")));
        for(const auto& f : spec.theme_overrides)
            if(HasValue(node, overlay, f.id))
                Apply(s, f.adapter_field_id, ResolveValue(node, overlay, f.id, node.theme_overrides.Find(f.id) >= 0 ? node.theme_overrides[f.id] : f.default_value));
        return s;
    }
public:
    const char* Id() const override { return Traits::Id(); }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == Traits::Kind(); }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override {
        S s = Traits::Base();
        AddPaletteMetrics(spec, "", "Surface", s.palette, s.metrics);
        for(const auto& f : Traits::Numbers()) AddNumeric(spec, f.id, f.id, "Layout", s.*(f.member), 6, 256, true);
        for(const auto& f : Traits::Colors())
            Add(spec, f.id, f.id, "Content colours", PropertyEditorKind::Color, s.*(f.member));
        Add(spec, "font_face", "Font", "Text", PropertyEditorKind::Text, s.font.GetFaceName(), true).Editor("property.font");
        Add(spec, "title_font_face", "Font", "Heading", PropertyEditorKind::Text, Traits::Heading(s).GetFaceName(), true).Editor("property.font");
        AddNumeric(spec, "title_font_height", "Size", "Heading", Traits::Heading(s).GetHeight(), 6, 96, true);
        Add(spec, "title_font_bold", "Bold", "Heading", PropertyEditorKind::Boolean, Traits::Heading(s).IsBold(), true);
        AddNumeric(spec, "font_height", "Size", "Text", s.font.GetHeight(), 6, 96, true);
        Add(spec, "font_bold", "Bold", "Text", PropertyEditorKind::Boolean, s.font.IsBold(), true);
    }
    bool HasField(const String& id) const override {
        if(id == "title_font_face" || id == "title_font_height" || id == "title_font_bold") return true;
        if(IsPaletteMetricsField("", id) || id == "font_face" || id == "font_height" || id == "font_bold") return true;
        for(const auto& f : Traits::Numbers()) if(id == f.id) return true;
        for(const auto& f : Traits::Colors()) if(id == f.id) return true;
        return false;
    }
    bool FieldAffectsLayout(const String& id) const override {
        for(const auto& f : Traits::Numbers()) if(id == f.id) return true;
        return PaletteMetricsAffectsLayout("", id) || id.StartsWith("font_") || id.StartsWith("title_font_");
    }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override {
        S s = Resolve(node, spec, overlay);
        if(IsPaletteMetricsField("", id)) return PaletteMetricsValue(s.palette, s.metrics, "", id);
        for(const auto& f : Traits::Numbers()) if(id == f.id) return s.*(f.member);
        for(const auto& f : Traits::Colors()) if(id == f.id) return s.*(f.member);
        if(id == "title_font_face") return Traits::Heading(s).GetFaceName();
        if(id == "title_font_height") return Traits::Heading(s).GetHeight();
        if(id == "title_font_bold") return Traits::Heading(s).IsBold();
        if(id == "font_face") return s.font.GetFaceName();
        if(id == "font_height") return s.font.GetHeight();
        if(id == "font_bold") return s.font.IsBold();
        return Value();
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override {
        auto* c = dynamic_cast<typename Traits::Control*>(&ctrl);
        if(!c) return;
        c->SetCustomStyle(Resolve(node, spec, overlay));
    }
    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override {
        // Standard is also explicit: Breadcrumbs otherwise defaults to Accent.
        String var = member + "_style";
        out << "\t" << Traits::Type() << "::Style " << var << " = " << Traits::BaseCode(node.GetProperty("role", "Standard")) << ";\n";
        for(const auto& f : spec.theme_overrides) {
            int q = node.theme_overrides.Find(f.id); if(q < 0) continue;
            Value v = node.theme_overrides.GetValue(q); String id = f.adapter_field_id;
            if(EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "", id, v)) continue;
            bool number = false;
            for(const auto& field : Traits::Numbers()) if(id == field.id) {
                out << "\t" << var << "." << id << " = " << (int)v << ";\n";
                number = true;
            }
            if(number) continue;
            if(id == "title_font_face") out << "\t" << var << "." << Traits::HeadingMember() << ".FaceName(" << EmitValue(v) << ");\n";
            else if(id == "title_font_height") out << "\t" << var << "." << Traits::HeadingMember() << ".Height(" << (int)v << ");\n";
            else if(id == "title_font_bold") out << "\t" << var << "." << Traits::HeadingMember() << ".Bold(" << AsString((bool)v) << ");\n";
            else if(id == "font_face") out << "\t" << var << ".font.FaceName(" << EmitValue(v) << ");\n";
            else if(id == "font_height") out << "\t" << var << ".font.Height(" << (int)v << ");\n";
            else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << EmitValue(v) << ");\n";
            else for(const auto& field : Traits::Colors()) if(id == field.id)
                out << "\t" << var << "." << id << " = " << EmitValue(v) << ";\n";
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};
}
const UiDesignerThemeAdapter& UiDesignerTableThemeAdapterInstance() {
    static DataThemeAdapter<TableTraits> adapter; return adapter;
}
const UiDesignerThemeAdapter& UiDesignerBreadcrumbThemeAdapterInstance() {
    static DataThemeAdapter<BreadcrumbTraits> adapter; return adapter;
}
}
