#include "hook/hooks.h"
#include "utils/log.h"
#include "utils/utils.h" // IsLibraryLoaded
#include <dobby.h>
#include <pthread.h>
#include <unistd.h>
//#include <BNM/Loading.hpp>
#include <KittyInclude.hpp>
#define kNO_KEYSTONE
// Thread function
void *pussy_thread(void *)
{
    LOGI("Fuby thread started");

    // 🔁 Tunggu sampai libEGL.so dimuat oleh game
    const char *targetLib = "libEGL.so";
    int maxWaitMs = 10000; // 10 detik
    int waited = 0;

    while (!IsLibraryLoaded(targetLib))
    {
        if (waited >= maxWaitMs)
        {
            LOGE("Timeout: %s belum dimuat setelah %dms", targetLib, maxWaitMs);
            return nullptr;
        }

        LOGI("Menunggu %s...", targetLib);
        usleep(500 * 1000); // 500ms
        waited += 500;
    }

    // ✅ Setelah lib tersedia, pasang hook
    InitHooks();
    LOGI("All hooks initialized..");

    return nullptr;
}

// Entry point dari .so
__attribute__((constructor)) void lib_main()
{
    pthread_t thread;
    int result = pthread_create(&thread, nullptr, pussy_thread, nullptr);

    if (result == 0)
    {
        pthread_detach(thread); // Tidak perlu join
        LOGI("Thread created and detached successfully");
    }
    else
    {
        LOGE("Failed to create mod thread: %d", result);
    }
}