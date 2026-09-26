#include "UiDesignerSession.h"
#include "UiDesignerListDataAdapter.h"

namespace Upp {

bool UiDesignerSession::BuildComposition(UiDesignerNodeId parent,
    const Vector<UiDesignerCompositionItem>& items, UiDesignerDocument& prepared,
    Vector<UiDesignerNodeId>& created, String& error) const
{
    if(items.IsEmpty() || items.GetCount() > 64 || !document_.Find(parent)) {
        error = "Composition requires an existing parent and 1..64 items"; return false;
    }
    if(!UiDesignerDeserialize(UiDesignerSerialize(document_, false), prepared, error)) return false;
    // Isolated construction uses the normal allocator and semantic drop rules.
    // No command/event touches the live session until one ReplaceDocument commit.
    UiDesignerCommandService construction(prepared);
    UiDesignerDropService drops(prepared, catalog_, construction);
    VectorMap<String, UiDesignerNodeId> refs;
    for(const auto& item : items) {
        if(item.reference.IsEmpty() || refs.Find(item.reference) >= 0) { error = "Duplicate/empty symbolic reference"; return false; }
        UiDesignerNodeId target = parent;
        if(!item.parent_reference.IsEmpty()) {
            int q = refs.Find(item.parent_reference);
            if(q < 0) { error = "Parent reference must precede its children"; return false; }
            target = refs[q];
        }
        const auto* target_node=prepared.Find(target);
        if(item.has_list_items && (item.type != "UiList" || item.list_items.GetCount() > 128)) {
            error = "list_items supports UiList only, with at most 128 text rows"; return false;
        }
        if(item.grid_row >= 0 || item.grid_column >= 0) {
            if(!target_node || target_node->type!="UiGridLayout" || item.grid_row<0 || item.grid_column<0 ||
               item.grid_row >= (int)target_node->GetProperty("rows",1) ||
               item.grid_column >= (int)target_node->GetProperty("columns",1)) {
                error="Composition grid placement requires a Grid parent and an in-range row/column pair"; return false;
            }
        }
        auto plan = drops.PlanAdd(item.type, target, Point(), false, -1, item.grid_row, item.grid_column);
        UiDesignerNodeId id = 0;
        if(!plan.valid || !drops.Execute(plan, &id, &error)) {
            if(error.IsEmpty()) error = plan.reason; return false;
        }
        refs.Add(item.reference, id); created.Add(id);
        if(item.has_list_items) {
            ValueMap data; data.Set("items", ValueArray());
            for(const String& text : item.list_items) {
                if(text.GetCount() > 4096) { error = "List row text exceeds 4096 characters"; return false; }
                ValueMap row; row.Set("text", text);
                if(!UiDesignerListDataAdapter::AppendItem(data, row)) {
                    error = "Invalid list row"; return false;
                }
            }
            if(!prepared.SetData(id, "root", data, UiDesignerImpactControlState | UiDesignerImpactCode)) {
                error = "Cannot set authored list data"; return false;
            }
        }
        for(int i = 0; i < item.properties.GetCount(); i++) {
            String key = AsString(item.properties.GetKey(i));
            const auto* spec = catalog_.Find(item.type);
            if(!spec || !spec->FindProperty(key) || key == "name" || key.StartsWith("document_") ||
               !prepared.SetProperty(id, key, item.properties.GetValue(i), UiDesignerImpactStructure)) {
                error = "Invalid composition field " + key; return false;
            }
        }
    }
    return catalog_.ValidateDocument(prepared, error);
}

static String UniquePlacedPresetName(const UiDesignerDocument& document,
                                     const String& base)
{
    String candidate = base;
    int suffix = 2;
    for(;;) {
        bool exists = false;
        for(const UiDesignerNode& node : document.GetNodes())
            if(node.name == candidate) {
                exists = true;
                break;
            }
        if(!exists)
            return candidate;
        candidate = base + "_" + AsString(suffix++);
    }
}

static int SnapPresetCoordinate(int value)
{
    return max(0, ((value + 4) / 8) * 8);
}

bool UiDesignerSession::InsertPresetAt(
    const String& preset_id, UiDesignerNodeId target,
    Point canvas_position, bool has_canvas_position, int index,
    int grid_row, int grid_column, UiDesignerNodeId *created, String& error)
{
    if(!catalog_.FindPreset(preset_id)) {
        error = "Unknown preset " + preset_id;
        return false;
    }
    if(!target)
        target = ResolveInsertParent();

    UiDesignerDocument fragment;
    UiDesignerNodeId fragment_root = 0;
    if(!UiDesignerPresetLibrary::Build(preset_id, catalog_, fragment,
                                       fragment_root, error))
        return false;
    const UiDesignerNode *fragment_node = fragment.Find(fragment_root);
    if(!fragment_node) {
        error = "Preset construction failed";
        return false;
    }

    UiDesignerDropPlan placement = PlanAddControl(
        fragment_node->type, target, canvas_position, has_canvas_position,
        index, grid_row, grid_column);
    if(!placement.valid) {
        error = placement.reason.IsEmpty() ? "Preset drop is invalid"
                                           : placement.reason;
        return false;
    }

    UiDesignerDocument updated;
    if(!UiDesignerDeserialize(UiDesignerSerialize(document_, false), updated, error))
        return false;

    VectorMap<UiDesignerNodeId, UiDesignerNodeId> id_map;
    Function<UiDesignerNodeId(UiDesignerNodeId, UiDesignerNodeId, int)> clone_node;
    clone_node = [&](UiDesignerNodeId source_id, UiDesignerNodeId parent,
                     int insert_index) -> UiDesignerNodeId {
        const UiDesignerNode *source = fragment.Find(source_id);
        if(!source)
            return 0;
        const String name = UniquePlacedPresetName(updated, source->name);
        UiDesignerNodeId destination = updated.AddNode(
            source->type, name, parent, source->flags, insert_index);
        UiDesignerNode *copy = updated.Find(destination);
        if(!copy)
            return 0;
        copy->properties = source->properties;
        copy->data = source->data;
        copy->theme_overrides = source->theme_overrides;
        copy->theme_override_saved = source->theme_override_saved;
        id_map.Add(source_id, destination);
        for(int i = 0; i < source->children.GetCount(); ++i)
            if(!clone_node(source->children[i], destination, i))
                return 0;
        return destination;
    };

    UiDesignerNodeId inserted = clone_node(
        fragment_root, placement.parent, placement.index);
    if(!inserted) {
        error = "Unable to clone preset subtree";
        return false;
    }

    UiDesignerNode *inserted_node = updated.Find(inserted);
    const UiDesignerNode *parent = updated.Find(placement.parent);
    if(inserted_node) {
        static const char *placement_fields[] = {
            "grid_row", "grid_column", "x", "y"
        };
        for(const char *field : placement_fields) {
            const int q = placement.add_defaults.Find(field);
            if(q >= 0)
                inserted_node->properties.Set(
                    field, placement.add_defaults.GetValue(q));
        }
        // Containers normally snap to the origin when nested by the generic
        // freeform planner. A preset dropped into an Absolute Layout is a
        // deliberate positioned composition, so preserve the actual drop point.
        if(parent && parent->type == "UiAbsoluteLayout" &&
           has_canvas_position) {
            inserted_node->properties.Set("x",
                SnapPresetCoordinate(canvas_position.x));
            inserted_node->properties.Set("y",
                SnapPresetCoordinate(canvas_position.y));
        }
    }

    for(int i = 0; i < id_map.GetCount(); ++i) {
        const UiDesignerNode *source = fragment.Find(id_map.GetKey(i));
        UiDesignerNode *destination = updated.Find(id_map[i]);
        if(!source || !destination)
            continue;
        destination->actions = clone(source->actions);
        for(UiDesignerActionBinding& action : destination->actions)
            if(action.target) {
                const int q = id_map.Find(action.target);
                action.target = q >= 0 ? id_map[q] : 0;
            }
    }

    if(!catalog_.ValidateDocument(updated, error))
        return false;
    if(!commands_.ReplaceDocument(updated, "Insert preset " + preset_id)) {
        error = commands_.GetLastError();
        return false;
    }
    Select(inserted);
    if(created)
        *created = inserted;
    WhenStatus("Inserted preset " + preset_id);
    error.Clear();
    return true;
}

}
