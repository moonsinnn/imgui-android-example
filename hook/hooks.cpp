#include "hooks.h"
#include "../imguisetup.h"
#include "../utils/init.h"
#include "../utils/log.h"
#include "../utils/time.h"
#include "input.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <chrono>
#include <dlfcn.h>
#include <dobby.h>
#include <xdl.h>

// Pointer asli
static EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = nullptr;
static EGLSurface (*orig_eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, ANativeWindow *window,
                                                 const EGLint *attrib_list) = nullptr;

// Global state
static ANativeWindow *g_window = nullptr;
static EGLDisplay g_display = nullptr;
static EGLSurface g_surface = nullptr;

void TryInitImGuiSafe()
{
    if (IsImGuiInitialized())
        return;

    // Pastikan semua resource ada
    if (!g_window || !g_display || !g_surface)
    {
        LOGW("TryInitImGuiSafe: Missing window/display/surface");
        return;
    }

    EGLContext ctx = eglGetCurrentContext();
    if (ctx == EGL_NO_CONTEXT)
    {
        LOGW("TryInitImGuiSafe: No valid EGL context yet");
        return;
    }

    GLint vp[4] = {0};
    glGetIntegerv(GL_VIEWPORT, vp);
    if (vp[2] < 600 || vp[3] < 400)
    {
        LOGW("TryInitImGuiSafe: Viewport too small: %dx%d", vp[2], vp[3]);
        return;
    }

    InitImGui(g_window, g_display, g_surface);
}

// ✅ eglSwapBuffers
EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    if (!orig_eglSwapBuffers)
    {
        LOGE("orig_eglSwapBuffers is NULL");
        return EGL_FALSE;
    }

    g_display = dpy;
    g_surface = surface;
    // ✅ Panggil init jika belum
    TryInitImGuiSafe();
    // 🎮 Render UI
    if (IsImGuiInitialized())
    {
        RenderImGui();
    }

    return orig_eglSwapBuffers(dpy, surface);
}

// ✅ eglCreateWindowSurface
EGLSurface hooked_eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, ANativeWindow *window,
                                         const EGLint *attrib_list)
{
    EGLSurface surface = orig_eglCreateWindowSurface(dpy, config, window, attrib_list);

    LOGI("ANativeWindow captured: %p", (void *)window);
    LOGI("EGLSurface created: %p", surface);

    int w = ANativeWindow_getWidth(window);
    int h = ANativeWindow_getHeight(window);
    LOGI("Screen resolution: %dx%d", w, h);

    g_window = window; // Cache for ImGui_ImplAndroid
    g_display = dpy;
    g_surface = surface;

    return surface;
}

// Helper untuk resolve symbol
void *ResolveSymbol(const char *libName, const char *symbolName)
{
    void *handle = dlopen(libName, RTLD_LAZY);
    if (handle)
    {
        void *sym = dlsym(handle, symbolName);
        if (sym)
        {
            LOGI("Resolved %s from %s via dlsym: %p", symbolName, libName, sym);
            return sym;
        }
        else
        {
            LOGW("dlsym failed for %s in %s, trying xDL...", symbolName, libName);
        }
    }
    else
    {
        LOGW("dlopen failed for %s, trying xDL...", libName);
    }

    // Fallback ke xDL
    void *xdlHandle = xdl_open(libName, XDL_DEFAULT);
    if (!xdlHandle)
    {
        LOGE("xdl_open failed for %s", libName);
        return nullptr;
    }

    void *sym = xdl_sym(xdlHandle, symbolName, nullptr);
    if (sym)
    {
        LOGI("Resolved %s from %s via xDL: %p", symbolName, libName, sym);
    }
    else
    {
        LOGE("Failed to resolve %s from %s even with xDL", symbolName, libName);
    }

    return sym;
}

void InitHooks()
{
    LOGI("Initializing hooks...");

    void *swap = ResolveSymbol("libEGL.so", "eglSwapBuffers");
    if (swap &&
        DobbyHook(swap, (dobby_dummy_func_t)hooked_eglSwapBuffers, (dobby_dummy_func_t *)&orig_eglSwapBuffers) == 0)
    {
        LOGI("Successfully hooked eglSwapBuffers");
    }
    else
    {
        LOGE("Failed to hook eglSwapBuffers");
    }

    void *createWin = ResolveSymbol("libEGL.so", "eglCreateWindowSurface");

    if (createWin && DobbyHook(createWin, (dobby_dummy_func_t)hooked_eglCreateWindowSurface,
                               (dobby_dummy_func_t *)&orig_eglCreateWindowSurface) == 0)
    {
        LOGI("Successfully hooked eglCreateWindowSurface");
    }
    else
    {
        LOGE("Failed to hook eglCreateWindowSurface");
    }

    InitInputHooks(); // Optional: hook input
    LOGI("All hooks initialized");
}
