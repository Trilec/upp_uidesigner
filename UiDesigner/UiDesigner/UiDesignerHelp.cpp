#include "UiDesignerWidgets.h"

namespace Upp {
namespace {

struct HelpTopic {
    const char* title;
    const char* sections[8]; // Four short heading/body pairs per topic.
};

const HelpTopic help_topics[] = {
    {"Start here", {
        "Design, review, build",
        "Create an editable interface in Designer. Set its shared appearance in Theme Studio. The Assistant can prepare changes in either workspace; you decide whether to apply them.",
        "A useful first design",
        "Add a Grid Layout with one column and three rows. Put a Label at the top, an expanding Panel in the middle, and a horizontal Box Layout at the bottom. Add an expanding Spacer before OK and Cancel buttons to push them right.",
        "Save your work",
        "Save Project stores the editable design and its project themes. Export creates files for another application. Saving a project and building an application are separate actions. Select and copy text from this guide; Escape or Close returns to your work.",
        "Find your way around",
        "The left sidebar contains Layouts, Containers, UI Controls, Presets and U++ Controls. The right sidebar holds Hierarchy, Inspector and other editing tools. Hover over an icon for its name. Designer / Theme Studio switches workspaces; Assistant opens below. Choose another Help topic above to learn that workflow."
    }},
    {"Designer & layouts", {
        "Build from the outside in",
        "Drag controls from the left catalogue onto the canvas or into a container. Use Grid for stable rows and columns; use Box for horizontal or vertical sequences, with wrapping when needed. A TitleCard is for a richer heading; use a Label for a simple one. A TitleCard accepts one content child: use a layout there for several controls.",
        "Choose where extra space goes",
        "Fit measures the content. Expand takes available space. Fixed uses an explicit size. Typically the main content expands while headings and action rows fit. A Spacer can separate groups or push actions to the right. Avoid fixed coordinates when a layout expresses the structure.",
        "Select and refine",
        "Use Hierarchy to select a container covered by its children. Inspector edits the selected control; Data edits supported list/tree content; Behavior configures supported actions. Use Undo/Redo to recover changes. Code shows the generated C++ for review.",
        "Example: a resizing dialog",
        "Set the outer Grid to expand in both directions. Set the heading and bottom button row to Fit height; set the centre Panel to Expand height. In the horizontal button Box, make the Spacer expand and each button fit. Resize to check that the body gains space while the buttons remain together."
    }},
    {"Theme Studio", {
        "Choose a theme to work on",
        "Open Themes in the right sidebar. Project themes are editable drafts saved with the project. Defaults are starting points. My Themes lists saved theme files. New, Duplicate and Rename help organise drafts; deleting a draft is different from removing a saved file from the list.",
        "Roles and appearance",
        "Standard is the base appearance; Subtle is quieter; Accent highlights a control; Alert calls attention to it. Light and Dark are two views of the same theme. Select a sample and use Inspector to adjust its role recipe. Per-control overrides in Designer can still replace inherited styling.",
        "Keep, save and reuse",
        "An Assistant theme proposal previews changes before you Keep it. Refine it in chat or discard it. Keep accepts the proposal; it does not save a file. Save Project preserves project themes. Save Theme / Save Theme As writes a reusable theme file; Add to My Themes uses that save flow. Theme Studio has its own Undo/Redo.",
        "Example: refine a style",
        "Duplicate a project theme and give it a useful name. Ask for a yellow brutalist theme with square corners, bold headings and hard shadows. Review the samples in Light and Dark, then ask for a narrower change, such as lighter Accordion header text in Light Accent. Keep the result when satisfied, then save it. Changing a role recipe affects controls inheriting that recipe."
    }},
    {"Assistant", {
        "Describe the result",
        "Open Assistant at the bottom. Choose a provider/model through Profile. Credentials belong in the configured environment variable, never in a project or chat. Ask for an interface, identify the main expanding area and name controls you specifically want. Example: Create an account dialog with a Label heading, a Name entry, and right-aligned OK and Cancel buttons. Do not use presets.",
        "Review before Apply",
        "A prose reply alone creates nothing. A valid proposal exposes Apply or Review proposals. Activity shows tool progress and failures. No proposal to apply means preparation failed; read the explanation and use Retry with fix or clarify the request. Execution is bounded to prevent runaway calls. A later failure does not invalidate an already prepared proposal.",
        "Iterate and know the limits",
        "After Apply, ask to change an existing label or refine the layout; Undo can reverse the applied group. History lets you revisit proposals. Clear all clears the discussion and dismisses pending proposals, not applied document edits. Pasted HTML can guide a design, but image attachments and rendered HTML are not supported here yet. Generated buttons are visual controls unless their behavior is explicitly wired.",
        "Make requests easy to verify",
        "Describe what must appear, where it belongs and which area should expand. Select an existing control and ask: Change this heading to Account settings, keeping the layout. The Assistant can initialise a new List with text rows; more complex collections use Data. Editing the document while a proposal is pending may make it stale; request a fresh proposal against the current design."
    }},
    {"Code, export & build", {
        "Review and export",
        "Code opens the generated source; the larger viewer has Close and Escape. Export offers a complete C++ package, a component, or editable JSON. Complete packages include the .upp manifest, generated source, a user subclass and an entry point. Keep custom application logic in user-owned files, which normal regeneration preserves.",
        "Build and run",
        "In Designer's Code panel choose Build. Set the UMK executable, source nests (including Ui and its dependencies plus U++ uppsrc), build method, package folder/name, window class and output executable. The package folder name must match the package name. Local build settings are remembered separately from your project.",
        "Check the real result",
        "Export + build writes the package and compiles it. Read errors in the compiler log; Stop build cancels compilation. A successful build enables Run application for manual testing. Compilation proves the source builds, not that every button performs an application action. Test resizing, controls and any wired behavior in the running application.",
        "Where the files go",
        "The Designer application is bin/UiDesigner.exe. Its local preferences are separate from the project. Your generated application goes to the folder you choose in Build. The .build.log beside its executable records the compiler output. To share editable work, share the saved project; to continue application development, use the complete exported C++ package."
    }},
    {"When something looks wrong", {
        "Layout or selection",
        "Select the parent in Hierarchy and inspect its direction, Grid placement and sizing. Check that the main region expands and small rows fit. Undo the last change to isolate it. An empty Fit container may become very small; a parent with restrictive fixed dimensions can limit its children.",
        "Unexpected colours",
        "Check the active theme, Light/Dark view and selected role. Then check whether a local override is masking the shared theme recipe. Reset the relevant override to inherit again. Review both appearances before saving a theme.",
        "Assistant or build failure",
        "For an Assistant failure, expand Activity and use Retry with fix. Check Profile and the credential environment variable if the provider cannot connect. For compiler failures, check UMK, build method and source nests, then read the first compiler error. Include the request, error text and a screenshot when reporting a problem; never include an API key.",
        "Useful information for a report",
        "Note the application version, active workspace and theme, and the smallest sequence of steps that reproduces the issue. Say whether the problem occurs in a blank project. Save a copy before experimenting. The visible result matters: a successfully prepared proposal or compiled program can still need layout or styling refinement."
    }}
};

class UiDesignerHelpWindow : public TopWindow {
    UiLabel label_;
    UiDropdown topics_;
    RichTextView document_;
    UiButton close_;
    void ShowTopic() {
        int index = (int)topics_.GetData();
        if(index < 0 || index >= int(sizeof(help_topics) / sizeof(help_topics[0]))) index = 0;
        const auto& topic = help_topics[index];
        Color ink = UiTheme::ResolveLabel(UiRole::Standard).palette.ink[ST_NORMAL];
        Color accent = UiTheme::ResolveLabel(UiRole::Accent).palette.ink[ST_NORMAL];
        auto colour = [](Color c) { return Format("@(%d.%d.%d)", c.GetR(), c.GetG(), c.GetB()); };
        String qtf = "[A+100" + colour(ink) + " [*+160" + colour(accent) + " " + DeQtf(topic.title) + "]&";
        for(int i = 0; i < 8; i += 2)
            qtf << "&[* " << DeQtf(topic.sections[i]) << "]&" << DeQtf(topic.sections[i + 1]) << "&";
        qtf << "]";
        document_.SetQTF(qtf);
        document_.SetSb(0);
    }
public:
    void Paint(Draw& draw) override {
        Color paper = UiTheme::ResolvePanel(UiPanelRole::Surface).palette.face[ST_NORMAL].color;
        draw.DrawRect(GetSize(), IsNull(paper) ? SColorFace() : paper);
    }
    UiDesignerHelpWindow() {
        Title("Ui Designer - Quick guide").Sizeable().Zoomable();
        SetRect(0, 0, DPI(760), DPI(600));
        SetMinSize(Size(DPI(500), DPI(360)));
        label_.SetText("Help topic");
        Add(label_.LeftPos(16, 100).TopPos(16, 30));
        Add(topics_.HSizePos(120, 16).TopPos(16, 30));
        topics_.UseInternalModel().Clear();
        for(int i = 0; i < int(sizeof(help_topics) / sizeof(help_topics[0])); ++i)
            topics_.Add(help_topics[i].title, i);
        topics_.Select(0);
        topics_.WhenSelectData = [=](const Value&) { ShowTopic(); };
        Add(document_.HSizePos(16, 16).VSizePos(60, 56));
        Color paper = UiTheme::ResolvePanel(UiPanelRole::Surface).palette.face[ST_NORMAL].color;
        document_.Background(IsNull(paper) ? SColorPaper() : paper).Margins(DPI(12))
                 .SetZoom(GetRichTextStdScreenZoom());
        close_.SetText("Close (Esc)");
        close_.WhenAction = [=] { Break(IDCANCEL); };
        Add(close_.RightPos(16, 120).BottomPos(12, 32));
        ShowTopic();
    }
    bool Key(dword key, int count) override {
        if(key == K_ESCAPE) { Break(IDCANCEL); return true; }
        return TopWindow::Key(key, count);
    }
};
}

void ShowUiDesignerHelp() { UiDesignerHelpWindow dialog; dialog.Run(); }
}
