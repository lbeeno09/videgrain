#include "Core/App.h"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        App app(1280, 720, L"Videgrain");
        app.Run();
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR:\t" << e.what() << '\n';

        return -1;
    }

    return 0;
}
