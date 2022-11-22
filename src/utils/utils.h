
#include "imgui.h"

#include <string>
#include <vector>

namespace coffeepot
{
    class Playlist;
    struct Action;
    struct Option;
}

namespace ImGui
{
    void InputString(const char* label, std::string& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
    void ComboBox(const char* label, std::string& selectedValue, const std::vector<std::string>& selectableValues);
	bool Checkbox(const char* label, bool& bChecked);
    bool IconButton(const char* label);

    void OptionRow(coffeepot::Option& option);

    enum class BuilderFooterResult
    {
        None,
        Save,
        Cancel,
        Start
    };

    BuilderFooterResult BuilderFooter(const char* label, bool& value);
}

