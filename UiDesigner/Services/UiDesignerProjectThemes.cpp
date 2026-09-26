#include "UiDesignerSession.h"

namespace Upp {
void UiDesignerSession::ResetProjectThemes()
{
    project_themes_.Clear();
    project_themes_.Add().name = "Current theme";
    active_project_theme_ = 0;
    project_themes_dirty_ = false;
    UiDesignerThemeSnapshot starting = theme_.Get();
    theme_.Replace(starting, true);
    WhenProjectThemesChanged();
}

bool UiDesignerSession::SelectProjectTheme(int index, String& error)
{
    error.Clear();
    if(index < 0 || index >= project_themes_.GetCount()) { error = "Unknown project theme"; return false; }
    if(index == active_project_theme_) return true;
    if(theme_.HasProposal()) { error = "Keep or discard the proposal before switching themes"; return false; }
    auto& previous = project_themes_[active_project_theme_];
    theme_.SwapDraftState(previous.document);
    previous.path = theme_path_; previous.checkpoint = theme_file_checkpoint_;
    active_project_theme_ = index;
    auto& next = project_themes_[index];
    theme_.SwapDraftState(next.document);
    theme_path_ = next.path; theme_file_checkpoint_ = next.checkpoint;
    project_themes_dirty_ = true;
    theme_.WhenChanged(); theme_.WhenHistoryChanged();
    WhenProjectThemesChanged();
    return true;
}

bool UiDesignerSession::AddProjectTheme(const String& name, const UiDesignerThemeSnapshot& value, String& error)
{
    if(theme_.HasProposal()) { error = "Keep or discard the proposal before creating a theme"; return false; }
    if(project_themes_.GetCount() >= 100) { error = "Project theme limit is 100"; return false; }
    String label = TrimBoth(name);
    if(label.IsEmpty() || label.GetCount() > 120) { error = "Use a theme name of 1 to 120 characters"; return false; }
    auto& entry = project_themes_.Add(); entry.name = label;
    entry.document.Replace(value, true);
    return SelectProjectTheme(project_themes_.GetCount()-1, error);
}

bool UiDesignerSession::RenameProjectTheme(int index, const String& name, String& error)
{
    String label = TrimBoth(name);
    if(index < 0 || index >= project_themes_.GetCount() || label.IsEmpty() || label.GetCount() > 120) {
        error = "Choose a project theme and a name of 1 to 120 characters"; return false;
    }
    project_themes_[index].name = label; project_themes_dirty_ = true;
    error.Clear(); WhenProjectThemesChanged(); return true;
}

bool UiDesignerSession::RemoveProjectTheme(int index, String& error)
{
    if(theme_.HasProposal()) { error = "Keep or discard the proposal before deleting a theme"; return false; }
    if(index < 0 || index >= project_themes_.GetCount() || project_themes_.GetCount() < 2) {
        error = "Keep at least one project theme"; return false;
    }
    if(index == active_project_theme_ && !SelectProjectTheme(index == 0 ? 1 : 0, error)) return false;
    project_themes_.Remove(index);
    if(active_project_theme_ > index) --active_project_theme_;
    project_themes_dirty_ = true; error.Clear(); WhenProjectThemesChanged(); return true;
}

bool UiDesignerSession::IsProjectThemeWorkspaceDirty() const
{
    if(project_themes_dirty_ || theme_.IsDirty()) return true;
    for(int i=0; i<project_themes_.GetCount(); ++i)
        if(i != active_project_theme_ && project_themes_[i].document.IsDirty()) return true;
    return false;
}

ValueMap UiDesignerSession::SerializeProjectThemes() const
{
    ValueArray entries;
    for(int i=0; i<project_themes_.GetCount(); ++i) {
        ValueMap entry; entry.Set("name", project_themes_[i].name);
        entry.Set("theme", i == active_project_theme_ ? theme_.Get().ToValue() : project_themes_[i].document.Get().ToValue());
        entries.Add(entry);
    }
    ValueMap result; result.Set("schema", 1); result.Set("active", active_project_theme_); result.Set("entries", entries);
    return result;
}

bool UiDesignerSession::ParseProjectThemes(const Value& value, Array<ProjectTheme>& entries,
                                          int& active, String& error) const
{
    if(!value.Is<ValueMap>() || value["schema"] != 1 || !value["entries"].Is<ValueArray>() || !IsNumber(value["active"])) {
        error = "Invalid project theme workspace"; return false;
    }
    ValueArray list = value["entries"];
    double active_value = value["active"];
    if(active_value < 0 || active_value >= list.GetCount() || active_value != (int)active_value) {
        error = "Invalid active project theme"; return false;
    }
    active = (int)active_value;
    if(list.IsEmpty() || list.GetCount()>100 || active<0 || active>=list.GetCount()) {
        error = "Invalid project theme count or active selection"; return false;
    }
    for(const auto& item : list) {
        if(!item.Is<ValueMap>() || !item["name"].Is<String>() || TrimBoth((String)item["name"]).IsEmpty() || ((String)item["name"]).GetCount()>120) {
            error = "Invalid project theme name"; return false;
        }
        UiDesignerThemeSnapshot snapshot;
        if(!snapshot.FromValue(item["theme"], error)) return false;
        auto& entry = entries.Add(); entry.name = item["name"]; entry.document.Replace(snapshot, true);
    }
    return true;
}
}
