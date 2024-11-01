#include "Application.h"

extern "C"
{
    int run_lua(const char *text)
    {
        std::cout << text << "\n";
        std::cout << "HELLO MY FRIEND" << "\n";
        return 0;
    }
}

int main()
{
    Application app(1200, 900);
    app.run();
    return 0;
}
