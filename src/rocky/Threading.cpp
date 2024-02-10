/**
 * rocky c++
 * Copyright 2023 Pelican Mapping
 * MIT License
 */
#include "Threading.h"
#include "IOTypes.h"
#include "Metrics.h"
#include "Utils.h"
#include "Instance.h"

#include <cstdlib>
#include <climits>
#include <mutex>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#   include <Windows.h>
#elif defined(__APPLE__) || defined(__LINUX__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__ANDROID__)
#   include <unistd.h>
#   include <sys/syscall.h>
#   include <pthread.h>
#endif

unsigned rocky::util::getCurrentThreadId()
{
#ifdef _WIN32
    return (unsigned)::GetCurrentThreadId();
#elif __APPLE__
    return ::syscall(SYS_thread_selfid);
#elif __ANDROID__
    return gettid();
#elif __LINUX__
    return (unsigned)::syscall(SYS_gettid);
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    long  tid;
    syscall(SYS_thr_self, &tid);
    return (unsigned)tid;
#else
    /* :XXX: this truncates to 32 bits, but better than nothing */
    return (unsigned)pthread_self();
#endif
}

void
rocky::util::setThreadName(const std::string& name)
{
#if (defined _WIN32 && defined _WIN32_WINNT_WIN10 && defined _WIN32_WINNT && _WIN32_WINNT >= _WIN32_WINNT_WIN10) || (defined __CYGWIN__)
    wchar_t buf[256];
    mbstowcs(buf, name.c_str(), 256);

    // Look up the address of the SetThreadDescription function rather than using it directly.
    typedef ::HRESULT(WINAPI* SetThreadDescription)(::HANDLE hThread, ::PCWSTR lpThreadDescription);
    auto set_thread_description_func = reinterpret_cast<SetThreadDescription>(::GetProcAddress(::GetModuleHandle("Kernel32.dll"), "SetThreadDescription"));
    if (set_thread_description_func)
    {
        set_thread_description_func(::GetCurrentThread(), buf);
    }

#elif defined _GNU_SOURCE && !defined __EMSCRIPTEN__ && !defined __CYGWIN__

    const auto sz = strlen(name.c_str());
    if (sz <= 15)
    {
        pthread_setname_np(pthread_self(), name.c_str());
    }
    else
    {
        char buf[16];
        memcpy(buf, name.c_str(), 15);
        buf[15] = '\0';
        pthread_setname_np(pthread_self(), buf);
    }
#endif
}
