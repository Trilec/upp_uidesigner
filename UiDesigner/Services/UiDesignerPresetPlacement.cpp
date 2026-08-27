#include "UiDesignerSession.h"

namespace Upp {

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
