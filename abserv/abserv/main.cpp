// abserv.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"
#include "Application.h"

int main(int argc, char** argv)
{
    Application app;
    if (!app.Initialize(argc, argv))
        return EXIT_FAILURE;

    app.Run();
    return EXIT_SUCCESS;
}
