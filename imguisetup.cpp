#include "imguisetup.h"
#include "fonts/maple.h"
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"
#include "menu.h"
#include "utils/init.h"
#include "utils/log.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>

void InitImGui(ANativeWindow *window, EGLDisplay display, EGLSurface surface)
{
    if (g_ImGuiInitialized.load())
    {
        LOGW("InitImGui called but already initialized");
        return;
    }

    if (!window || !display || !surface)
    {
        LOGE("InitImGui failed: window/display/surface is null");
        return;
    }

    EGLContext context = eglGetCurrentContext();
    if (context == EGL_NO_CONTEXT)
    {
        LOGW("InitImGui: No current EGL context, aborting init");
        return;
    }

    // Just to be sure: make sure current context is active
    if (eglMakeCurrent(display, surface, surface, context) != EGL_TRUE)
    {
        LOGE("InitImGui failed: eglMakeCurrent failed, error: 0x%04x", eglGetError());
        return;
    }

    GLint viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    int fb_width = viewport[2];
    int fb_height = viewport[3];

    if (fb_width < 600 || fb_height < 400)
    {
        LOGW("InitImGui skipped: viewport too small (%dx%d)", fb_width, fb_height);
        return;
    }

    LOGI("GL framebuffer: %dx%d", fb_width, fb_height);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)fb_width, (float)fb_height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Custom style
    ImGuiStyle &style = ImGui::GetStyle();
    style.ChildRounding = 12.0f;
    style.FrameRounding = 12.0f;
    style.PopupRounding = 12.0f;
    style.ScrollbarRounding = 18.0f;
    style.GrabRounding = 12.0f;
    style.TabRounding = 6.0f;
    style.WindowRounding = 12.0f;
    style.GrabMinSize = 20.0f;

    // ImGui::StyleColorsLight();

    // Load custom font
    if (MapleMonoNormalNL_NF_ExtraBold_ttf_len > 0)
    {
        io.Fonts->AddFontFromMemoryTTF((void *)MapleMonoNormalNL_NF_ExtraBold_ttf,
                                       MapleMonoNormalNL_NF_ExtraBold_ttf_len, 38.0f);
        LOGI("Custom font loaded");
    }
    else
    {
        LOGW("Custom font not found or empty");
    }

    // Backend init
    ImGui_ImplAndroid_Init(window);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    g_ImGuiInitialized.store(true);
    LOGI("ImGui initialized successfully");
}

void RenderImGui()
{
    if (!g_ImGuiInitialized.load())
    {
        LOGW("RenderImGui skipped: ImGui not initialized");
        return;
    }

    // Frame lifecycle
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();

    // Update framebuffer size dan scale secara real-time
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int fb_width = viewport[2];
    int fb_height = viewport[3];
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)fb_width, (float)fb_height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ImGui::NewFrame();

    DrawMenu();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ShutdownImGui()
{
    if (!g_ImGuiInitialized.load())
    {
        LOGW("ShutdownImGui called but ImGui not initialized");
        return;
    }

    LOGI("Shutting down ImGui...");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();

    g_ImGuiInitialized.store(false);

    // ⏱ Simpan waktu saat shutdown
    g_LastShutdownTime = std::chrono::steady_clock::now();

    LOGI("ImGui shutdown complete");
}

bool IsImGuiInitialized()
{
    return g_ImGuiInitialized.load();
}