// abmsgs.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"
#include "Version.h"
#include "Application.h"
#if !defined(WIN_SERVICE)
#include <csignal>
#include "MiniDump.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#   define CRTDBG_MAP_ALLOC
#   include <stdlib.h>
#   include <crtdbg.h>
#endif

namespace {
std::function<void(int)> shutdown_handler;
void signal_handler(int signal)
{
    shutdown_handler(signal);
}
} // namespace

static void ShowLogo()
{
    std::cout << "This is " << SERVER_PRODUCT_NAME << std::endl;
    std::cout << "Version " << SERVER_VERSION_MAJOR << "." << SERVER_VERSION_MINOR <<
        " (" << __DATE__ << " " << __TIME__ << ")";
#ifdef _DEBUG
    std::cout << " DEBUG";
#endif
    std::cout << std::endl;
    std::cout << "(C) 2017-" << SERVER_YEAR << std::endl;
    std::cout << std::endl;

    std::cout << "##########  ######  ######" << std::endl;
    std::cout << "    ##          ##  ##" << std::endl;
    std::cout << "    ##  ######  ##  ##" << std::endl;
    std::cout << "    ##  ##      ##  ##" << std::endl;
    std::cout << "    ##  ##  ##  ##  ##" << std::endl;
    std::cout << "    ##  ##  ##  ##  ##" << std::endl;
    std::cout << "    ##  ##  ##  ##  ##" << std::endl;

    std::cout << std::endl;
}

#ifdef _WIN32
static std::mutex gTermLock;
static std::condition_variable termSignal;
#endif

int main(int argc, char** argv)
{
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#if defined(_MSC_VER) && defined(WRITE_MINIBUMP)
    SetUnhandledExceptionFilter(System::UnhandledHandler);
#endif

    std::signal(SIGINT, signal_handler);              // Ctrl+C
    std::signal(SIGTERM, signal_handler);
#ifdef _WIN32
    std::signal(SIGBREAK, signal_handler);            // X clicked
#endif

    ShowLogo();

    {
        Application app;
        if (!app.InitializeA(argc, argv))
            return EXIT_FAILURE;

        shutdown_handler = [&app](int /*signal*/)
        {
#ifdef _WIN32
            std::unique_lock<std::mutex> lockUnique(gTermLock);
#endif
            app.Stop();
#ifdef _WIN32
            termSignal.wait(lockUnique);
#endif
        };

        app.Run();
    }

#ifdef _WIN32
    termSignal.notify_all();
#endif

    return EXIT_SUCCESS;
}

#else   // !defined(WIN_SERVICE)
// Internal name of the service
#define SERVICE_NAME             L"ABMessageServer"
// Displayed name of the service
#define SERVICE_DISPLAY_NAME     L"AB Message Server"
#define SERVICE_DESCRIPTION      L"Forgotten Wars Message Server"
// Service start options.
#define SERVICE_START_TYPE       SERVICE_AUTO_START
// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     L""
// The name of the account under which the service should run
// LocalService may not start because it does not have access to the directory
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"
// The password to the service account name
#define SERVICE_PASSWORD         NULL

#include "WinService.h"
AB_SERVICE_MAIN(System::WinService<Application>)
#endif // !defined(WIN_SERVICE)
