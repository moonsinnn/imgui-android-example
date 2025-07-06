#include "../imgui/imgui.h"

// Shared style settings
static float menuTransparency = 0.95f;
static float rounding = 8.0f;
static float borderSize = 1.0f;

static int themeSelection = 0;

static ImVec4 backgroundColor;
static ImVec4 textColor;
static ImVec4 borderColor;
static ImVec4 buttonColor;
static ImVec4 sliderColor;
static ImVec4 sliderBgColor;
static ImVec4 checkboxColor;

enum ThemePreset
{
    THEME_DARK = 0,
    THEME_LIGHT,
    THEME_CLASSIC,
    THEME_CUTE
};

void ApplyStyleVars()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameRounding = rounding;
    style.WindowRounding = rounding;
    style.ChildRounding = rounding;
    style.PopupRounding = rounding;
    style.GrabRounding = rounding;
    style.ScrollbarRounding = rounding;
    style.FrameBorderSize = borderSize;
    style.WindowBorderSize = borderSize;
}

void ResetColorsToDefaults()
{
    ImVec4 frameBg;

    switch (themeSelection)
    {
    case THEME_DARK:
        backgroundColor = ImVec4(0.1f, 0.1f, 0.1f, menuTransparency);
        textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        borderColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        buttonColor = ImVec4(0.4f, 0.7f, 0.3f, 1.0f);
        sliderColor = ImVec4(0.5f, 0.8f, 0.5f, 1.0f);
        sliderBgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        checkboxColor = buttonColor;
        frameBg = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        break;

    case THEME_LIGHT:
        backgroundColor = ImVec4(1.0f, 1.0f, 1.0f, menuTransparency);
        textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        borderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        buttonColor = ImVec4(0.25f, 0.68f, 0.85f, 1.0f);
        sliderColor = ImVec4(0.45f, 0.78f, 0.92f, 1.0f);
        sliderBgColor = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
        checkboxColor = buttonColor;
        frameBg = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        break;

    case THEME_CLASSIC:
        backgroundColor = ImVec4(0.5f, 0.5f, 0.5f, menuTransparency);
        textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        borderColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        buttonColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f);
        sliderColor = buttonColor;
        sliderBgColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        checkboxColor = buttonColor;
        frameBg = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        break;

    case THEME_CUTE:                                                    // 🌸 Cute Pastel Theme
        backgroundColor = ImVec4(0.98f, 0.95f, 1.0f, menuTransparency); // Lavender milk
        textColor = ImVec4(0.2f, 0.1f, 0.4f, 1.0f);                     // Dark lilac
        borderColor = ImVec4(0.8f, 0.7f, 1.0f, 1.0f);                   // Soft purple
        buttonColor = ImVec4(0.9f, 0.7f, 1.0f, 1.0f);                   // Pinkish purple
        sliderColor = ImVec4(0.85f, 0.6f, 1.0f, 1.0f);                  // Cute magenta
        sliderBgColor = ImVec4(0.95f, 0.9f, 1.0f, 1.0f);                // Pastel bg
        checkboxColor = ImVec4(1.0f, 0.8f, 0.9f, 1.0f);                 // Soft pink
        frameBg = ImVec4(0.95f, 0.9f, 1.0f, 1.0f);                      // Light lavender
        break;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, sliderColor);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, sliderColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBg);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkboxColor);
}

void ApplyTheme(int selection)
{
    themeSelection = selection;

    switch (selection)
    {
    case THEME_DARK:
        ImGui::StyleColorsDark();
        break;
    case THEME_LIGHT:
        ImGui::StyleColorsLight();
        break;
    case THEME_CLASSIC:
        ImGui::StyleColorsClassic();
        break;
    case THEME_CUTE:
        ImGui::StyleColorsLight();
        break; // Base style for cute theme
    }

    ApplyStyleVars();
    ResetColorsToDefaults();
}