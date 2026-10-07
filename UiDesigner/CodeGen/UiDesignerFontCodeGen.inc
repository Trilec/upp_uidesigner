// Included by UiDesignerCodeGen.cpp; uses its CppString and namespace helpers.
static void CollectFontSelections(const Value& value, Index<String>& families) {
    if(value.Is<String>()) {
        String s = value;
        if(s.StartsWith("project:")) families.FindAdd(s.Mid(8));
    }
    else if(value.Is<ValueMap>()) {
        ValueMap m = value;
        for(int i = 0; i < m.GetCount(); ++i) {
            const Value& item = m.GetValue(i);
            if(ToLower(AsString(m.GetKey(i))).Find("font") >= 0 || item.Is<ValueMap>() || item.Is<ValueArray>())
                CollectFontSelections(item, families);
        }
    }
    else if(value.Is<ValueArray>()) for(const Value& v : (ValueArray)value) CollectFontSelections(v, families);
}
static String ResolveGeneratedFontSelections(const String& source) {
    // Rewrite actual final output, including all existing adapter/nested emitters.
    // Each emitted FaceName statement has one receiver and one balanced argument.
    String result;
    for(const String& line : Split(source, '\n', false)) {
        int at = line.Find(".FaceName("), end = line.ReverseFind(");");
        if(at >= 0 && end > at) {
            int start = 0; while(start < line.GetCount() && IsSpace(line[start])) ++start;
            String receiver = line.Mid(start, at - start);
            bool member = !receiver.IsEmpty() && (IsAlpha(receiver[0]) || receiver[0] == '_');
            for(char c : receiver) member = member && (IsAlNum(c) || c == '_' || c == '.');
            if(!member) { result << line << "\n"; continue; } // Never rewrite quoted user text.
            String argument = line.Mid(at + 10, end - at - 10);
            result << line.Left(start) << "UiFonts::ApplySelection(" << receiver << ", " << argument << ");\n";
        }
        else result << line << "\n";
    }
    return result;
}
static bool PackageProjectFonts(const UiDesignerDocument& document,
                                const UiDesignerCodeGenerationOptions& options,
                                UiDesignerGeneratedProject& result, String& header, String& source) {
    Index<String> families;
    CollectFontSelections(options.project_typography, families);
    for(const UiDesignerNode& node : document.GetNodes()) {
        CollectFontSelections(node.properties, families);
        CollectFontSelections(node.theme_overrides, families);
        CollectFontSelections(node.data, families);
    }
    if(families.IsEmpty() && options.project_typography.IsEmpty()) return false;
    auto Add = [&](const String& path, const String& content) {
        UiDesignerGeneratedFile& file = result.files.Add();
        file.relative_path = path; file.content = content; file.generator_owned = true;
    };
    String brc, initialization;
    const String brc_name = options.class_name + ".fonts.brc";
    ValueArray manifest;
    Index<String> present, packaged;
    int serial = 0;
    initialization << "\tUiFontCatalog fonts;\n";
    for(const UiDesignerResource& r : document.GetResources()) if(r.resource_type == "font") {
        String family = AsString(r.metadata["font_family_id"]);
        if(families.Find(family) < 0) continue;
        String hash = SHA256String(r.bytes), id = AsString(r.metadata["font_asset_id"]);
        if(id.IsEmpty()) id = "font-" + hash;
        String expected = AsString(r.metadata["font_sha256"]);
        if(!expected.IsEmpty() && expected != hash) {
            result.diagnostics.Add("Font resource content identity mismatch: " + id); return false;
        }
        String path = "fonts/" + hash + ".ttf", symbol = SanitizeIdentifier(options.namespace_name + "_" + options.class_name) + "_font_" + AsString(serial++);
        if(packaged.Find(path) < 0) { Add(path, r.bytes); packaged.Add(path); }
        String license = AsString(r.metadata["font_license"]), license_path = "fonts/" + hash + ".license.txt";
        if(!license.IsEmpty() && packaged.Find(license_path) < 0) { Add(license_path, license); packaged.Add(license_path); }
        brc << "BINARY(" << symbol << ", " << CppString(path) << ")\n";
        initialization << "\tfonts.Import(" << CppString(id) << ", " << CppString(family)
            << ", String((const char*)" << symbol << ", " << symbol << "_length), "
            << CppString(r.original_name) << ", " << CppString(license.IsEmpty() ? "" : license_path)
            << ", " << CppString(hash) << ");\n";
        present.FindAdd(family);
        ValueMap entry;
        entry.Set("asset_id", id); entry.Set("family_id", family); entry.Set("sha256", hash);
        entry.Set("file", path); entry.Set("source", r.original_name); entry.Set("license_file", license.IsEmpty() ? "" : license_path);
        entry.Set("family", r.metadata["font_family"]); entry.Set("face", r.metadata["font_face"]);
        entry.Set("bold", r.metadata["font_bold"]); entry.Set("italic", r.metadata["font_italic"]);
        manifest.Add(entry);
    }
    for(const String& family : families) if(present.Find(family) < 0)
        initialization << "\tfonts.DeclareMissing(" << CppString(family + "-missing") << ", "
            << CppString(family) << ", \"export\", \"Requested font resource is unavailable\");\n";
    initialization << "\tUiFonts::SetCatalog(fonts);\n\tUiTypography typography;\n";
    for(const char *field : { "body", "heading", "code", "fallback" }) {
        String key = String(field) + "_font";
        if(options.project_typography.Find(key) >= 0)
            initialization << "\ttypography." << field << " = " << CppString(AsString(options.project_typography[key])) << ";\n";
    }
    initialization << "\tUiTheme::SetTypography(typography);\n";
    // First data member initializes before all control members, including in
    // component exports. Load never depends on a user-owned main.cpp or cwd.
    header.Replace("protected:\n", "protected:\n\tstruct FontBootstrap { FontBootstrap(); };\n\tFontBootstrap font_bootstrap_;\n");
    String constructor = options.class_name + "Generated::FontBootstrap::FontBootstrap()\n{\n" + initialization + "}\n\n";
    source.Replace("void " + options.class_name + "Generated::BuildGeneratedUi()", constructor + "void " + options.class_name + "Generated::BuildGeneratedUi()");
    if(!brc.IsEmpty()) source.Replace("#include \"" + options.class_name + ".generated.h\"", "#include \"" + options.class_name + ".generated.h\"\n#include \"" + brc_name + "\"");
    Add(brc_name, brc);
    ValueMap m; m.Set("schema", 1); m.Set("assets", manifest); m.Set("typography", options.project_typography);
    Add("fonts/manifest.json", AsJSON(m, true));
    return true;
}
