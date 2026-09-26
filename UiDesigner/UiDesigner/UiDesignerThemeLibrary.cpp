#include "UiDesignerWindow.h"
#include "UiDesignerStyle.h"

namespace Upp {
void UiDesignerWindow::BuildThemeLibrary()
{
    theme_library_panel_.Add(theme_tree_);
    theme_tree_.SetRootVisible(false);
    auto wire = [&](UiButton& button, const char* label, const char* action) {
        theme_library_panel_.Add(button); button.SetText(label);
        String command = action; button.WhenAction = [=] { ThemeLibraryAction(command); };
    };
    wire(theme_new_, "New", "new"); wire(theme_duplicate_, "Duplicate", "duplicate");
    wire(theme_rename_, "Rename", "rename"); wire(theme_delete_, "Delete", "delete");
    wire(theme_use_, "New source copy", "use"); wire(theme_publish_, "Add to My Themes", "publish");
    theme_library_panel_.Add(theme_library_hint_);
    theme_library_hint_.SetText("Save Project keeps all theme drafts.");
    theme_tree_.WhenSelection = [=] {
        if(syncing_theme_tree_) return;
        int q = theme_tree_keys_.Find(theme_tree_.GetCursor().id);
        selected_theme_tree_key_ = q >= 0 ? theme_tree_keys_[q] : String();
        if(selected_theme_tree_key_.StartsWith("project:")) {
            String error;
            if(!session_.SelectProjectTheme(atoi(~selected_theme_tree_key_.Mid(8)), error)) RefreshStatus(error);
        }
        else if(selected_theme_tree_key_.StartsWith("file:") || selected_theme_tree_key_.StartsWith("builtin:")) {
            String error;
            if(!session_.ActivateThemeSource(selected_theme_tree_key_, error)) RefreshStatus(error);
        }
        // Selection alone must not rebuild the model or reset keyboard traversal.
        RefreshThemeLibraryActions();
    };
    RefreshThemeLibrary();
}

void UiDesignerWindow::RefreshThemeLibrary()
{
    if(syncing_theme_tree_) return;
    syncing_theme_tree_ = true;
    auto& model = theme_tree_.Model(); model.Clear(); theme_tree_keys_.Clear();
    auto project = model.AddChild(model.Root(), UiModelItem("Project themes"));
    auto library = model.AddChild(model.Root(), UiModelItem("My Themes"));
    auto defaults = model.AddChild(model.Root(), UiModelItem("Defaults"));
    theme_tree_keys_.Add(project.id, "group:project");
    theme_tree_keys_.Add(library.id, "group:library");
    theme_tree_keys_.Add(defaults.id, "group:defaults");
    UiTreeNodeRef selected;
    if(selected_theme_tree_key_ == "group:project") selected = project;
    if(selected_theme_tree_key_ == "group:library") selected = library;
    if(selected_theme_tree_key_ == "group:defaults") selected = defaults;
    auto add = [&](UiTreeNodeRef parent, const String& label, const String& key) {
        auto node = model.AddChild(parent, UiModelItem(label, key));
        theme_tree_keys_.Add(node.id, key);
        if(key == selected_theme_tree_key_) selected = node;
    };
    if(selected_theme_tree_key_.IsEmpty() || selected_theme_tree_key_.StartsWith("project:"))
        selected_theme_tree_key_ = "project:" + AsString(session_.GetActiveProjectTheme());
    for(int i=0; i<session_.GetProjectThemeCount(); ++i)
        add(project, session_.GetProjectThemeName(i) + (i == session_.GetActiveProjectTheme() ? "  (active)" : ""), "project:" + AsString(i));
    for(const auto& path : theme_library_) add(library, GetFileTitle(path), "file:" + path);
    for(const char* preset : {"Minimal", "Pill", "Linear", "Solid", "Outline", "Compact", "Layered"})
        add(defaults, preset, "builtin:" + String(preset));
    theme_tree_.Expand(project).Expand(library).Expand(defaults);
    if(selected.IsValid()) theme_tree_.SetCursor(selected).SelectNode(selected);
    syncing_theme_tree_ = false;
    RefreshThemeLibraryActions();
}

void UiDesignerWindow::RefreshThemeLibraryActions()
{
    bool draft = selected_theme_tree_key_.StartsWith("project:");
    bool file = selected_theme_tree_key_.StartsWith("file:");
    bool ready = !session_.Theme().HasProposal();
    theme_new_.Enable(ready); theme_duplicate_.Enable(ready && draft);
    theme_rename_.Enable(ready && draft); theme_delete_.Enable(ready && (file || (draft && session_.GetProjectThemeCount()>1)));
    theme_delete_.SetText(file ? "Remove from list" : "Delete");
    theme_use_.Enable(ready && (file || selected_theme_tree_key_.StartsWith("builtin:")));
    theme_publish_.Enable(ready && draft);
    theme_library_hint_.SetText(file || selected_theme_tree_key_.StartsWith("builtin:")
        ? "Editing a project copy; source is unchanged."
        : "Save Project keeps all theme drafts.");
}

void UiDesignerWindow::ThemeLibraryAction(const String& action)
{
    String error, key = selected_theme_tree_key_;
    if(session_.Theme().HasProposal()) { RefreshStatus("Keep or discard the proposal first"); return; }
    if(action == "new" || action == "duplicate") {
        String name = action == "new" ? String("New theme") : session_.GetProjectThemeName(session_.GetActiveProjectTheme()) + " copy";
        if(!EditText(name, action == "new" ? "New project theme" : "Duplicate theme", "Name")) return;
        UiDesignerThemeSnapshot snapshot = action == "new" ? UiDesignerThemeSnapshot() : session_.Theme().Get();
        session_.AddProjectTheme(name, snapshot, error);
        selected_theme_tree_key_.Clear();
    }
    else if(action == "rename" && key.StartsWith("project:")) {
        int index = atoi(~key.Mid(8)); String name = session_.GetProjectThemeName(index);
        if(EditText(name, "Rename project theme", "Name")) session_.RenameProjectTheme(index, name, error);
    }
    else if(action == "delete") {
        if(key.StartsWith("file:")) {
            int q = FindIndex(theme_library_, key.Mid(5));
            if(q >= 0) theme_library_.Remove(q);
            SaveThemeLibraryIndex(); selected_theme_tree_key_.Clear();
            RefreshStatus("Removed from My Themes. The theme file was not deleted.");
        }
        else if(key.StartsWith("project:") && PromptYesNo("Delete this project theme? Its unsaved edits and Undo history will be removed.")) {
            session_.RemoveProjectTheme(atoi(~key.Mid(8)), error); selected_theme_tree_key_.Clear();
        }
    }
    else if(action == "publish") SaveThemeAs();
    else if(action == "use") {
        if(key.StartsWith("file:")) session_.LoadThemeFile(key.Mid(5), error);
        else if(key.StartsWith("builtin:")) {
            UiDesignerThemeDocument fresh;
            if(fresh.Commit("preset", key.Mid(8), "Starting point", error))
                session_.AddProjectTheme(key.Mid(8) + " copy", fresh.Get(), error);
        }
        selected_theme_tree_key_.Clear();
    }
    if(!error.IsEmpty()) RefreshStatus(error);
    RefreshThemeLibrary(); SyncThemeChoices();
}
}
