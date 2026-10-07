#include "UiDesignerFonts.h"
namespace Upp {
bool UiDesignerImportFont(UiDesignerDocument& document, const String& path,
                         const String& family_id, const String& license_text,
                         String& asset_id, String& error) {
    FileIn file(path);
    if(!file) { error = "Unable to open font file"; return false; }
    if(file.GetSize() > 16 * 1024 * 1024) { error = "Font exceeds 16 MiB per-face limit"; return false; }
    String bytes = file.Get((int)file.GetSize());
    UiFontCatalog candidate;
    String id = "font-" + SHA256String(bytes);
    UiFontStatus status = candidate.Import(id, family_id.IsEmpty() ? "candidate" : family_id,
                                          bytes, path, license_text);
    const auto& assets = candidate.GetAssets();
    if(status != UiFontStatus::Loaded) {
        error = assets.IsEmpty() ? "Invalid font identity" : assets[0].diagnostic; return false;
    }
    const UiFontAsset& a = assets[0];
    String family = family_id.IsEmpty() ? "family-" + SHA256String(a.family).Left(24) : family_id;
    ValueMap metadata;
    metadata.Set("font_asset_id", id); metadata.Set("font_family_id", family);
    metadata.Set("font_sha256", a.content_hash); metadata.Set("font_family", a.family);
    metadata.Set("font_face", a.face); metadata.Set("font_bold", a.bold); metadata.Set("font_italic", a.italic);
    metadata.Set("font_license", license_text); metadata.Set("font_source", GetFileName(path));
    String key = document.AddResource("font", bytes, "font/ttf", GetFileName(path), 0, 0, metadata);
    if(key.IsEmpty()) { error = "Unable to store font resource"; return false; }
    asset_id = id; error.Clear(); return true;
}
void UiDesignerLoadFonts(const UiDesignerDocument& document, UiFontCatalog& catalog) {
    catalog.Clear();
    for(const UiDesignerResource& r : document.GetResources()) if(r.resource_type == "font") {
        String id = AsString(r.metadata["font_asset_id"]), family = AsString(r.metadata["font_family_id"]);
        if(id.IsEmpty()) id = "font-" + SHA256String(r.bytes);
        if(family.IsEmpty()) family = id;
        catalog.Import(id, family, r.bytes, r.original_name, AsString(r.metadata["font_license"]), AsString(r.metadata["font_sha256"]));
    }
    Value manifest = document.GetProperty(document.GetRootId(), "font_manifest");
    if(manifest.Is<ValueArray>()) for(const Value& item : (ValueArray)manifest) if(item.Is<ValueMap>()) {
        ValueMap m = item; String id = AsString(m["asset_id"]), family = AsString(m["family_id"]);
        bool found = false;
        for(const auto& a : catalog.GetAssets()) if(a.id == id) found = true;
        if(!found && !id.IsEmpty() && !family.IsEmpty())
            catalog.DeclareMissing(id, family, AsString(m["source"]), "Packaged font resource is unavailable");
    }
}
UiTypography UiDesignerTypography(const ValueMap& fields) {
    UiTypography t;
    t.body = AsString(fields["body_font"]); t.heading = AsString(fields["heading_font"]);
    t.code = AsString(fields["code_font"]);
    if(fields.Find("fallback_font") >= 0) t.fallback = AsString(fields["fallback_font"]);
    return t;
}
void UiDesignerActivateFonts(const UiDesignerDocument& document, const UiDesignerThemeSnapshot& theme) {
    // Source bytes are never fetched from paths here: save/load owns copied resources.
    static String previous;
    static uint64 revision = 0;
    String identity = document.GetDocumentId();
    for(const UiDesignerResource& r : document.GetResources()) if(r.resource_type == "font")
        identity << r.key << r.content_hash << AsJSON(r.metadata);
    identity << AsJSON(document.GetProperty(document.GetRootId(), "font_manifest"));
    if(identity != previous || revision != UiFonts::GetRevision()) {
        UiFontCatalog catalog; UiDesignerLoadFonts(document, catalog);
        UiFonts::SetCatalog(catalog); previous = identity;
    }
    UiFonts::SetTypography(UiDesignerTypography(theme.typography));
    revision = UiFonts::GetRevision();
}
}
