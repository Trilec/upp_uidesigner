#include <Core/Core.h>
#include <UiDesigner/Services/UiDesignerListDataAdapter.h>

using namespace Upp;

struct ListDataAdapterTester {
    int checks = 0;
    int failures = 0;

    void Check(bool condition, const String& label)
    {
        checks++;
        if(condition)
            Cout() << "PASS  " << label << '\n';
        else {
            failures++;
            Cout() << "FAIL  " << label << '\n';
        }
    }
};

static ValueMap Item(const char *text)
{
    ValueMap item;
    item.Set("text", text);
    item.Set("enabled", true);
    return item;
}

static ValueMap Root()
{
    ValueArray items;
    items.Add(Item("A"));
    items.Add(Item("B"));
    items.Add(Item("C"));
    ValueMap root;
    root.Set("items", items);
    return root;
}

static String TextAt(const ValueMap& root, int index)
{
    const ValueMap item = UiDesignerListDataAdapter::Item(root, index);
    return UiDesignerMapValue(item, "text", String());
}

static bool IsOrder(const ValueMap& root, const char *a,
                    const char *b, const char *c)
{
    return UiDesignerListDataAdapter::Items(root).GetCount() == 3 &&
           TextAt(root, 0) == a && TextAt(root, 1) == b && TextAt(root, 2) == c;
}

CONSOLE_APP_MAIN
{
    ListDataAdapterTester t;

    ValueMap down = Root();
    t.Check(UiDesignerListDataAdapter::MoveItem(down, 0, 1),
            "move down succeeds");
    t.Check(IsOrder(down, "B", "A", "C"),
            "move down preserves every item and swaps the requested row");

    ValueMap up = Root();
    t.Check(UiDesignerListDataAdapter::MoveItem(up, 1, -1),
            "move up succeeds");
    t.Check(IsOrder(up, "B", "A", "C"),
            "move up preserves every item and swaps the requested row");

    ValueMap two_down = Root();
    t.Check(UiDesignerListDataAdapter::MoveItem(two_down, 0, 2),
            "multi-position move down succeeds");
    t.Check(IsOrder(two_down, "B", "C", "A"),
            "multi-position move down preserves order and item count");

    ValueMap invalid = Root();
    t.Check(!UiDesignerListDataAdapter::MoveItem(invalid, 0, -1),
            "out-of-range move is rejected");
    t.Check(IsOrder(invalid, "A", "B", "C"),
            "rejected move leaves authored data unchanged");

    Cout() << "LIST_DATA_ADAPTER_SUMMARY checks=" << t.checks
           << " failed=" << t.failures << '\n';
    SetExitCode(t.failures ? 1 : 0);
}
