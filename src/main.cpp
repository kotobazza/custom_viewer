#include <iostream>

#include "Application.h"
auto screen = ftxui::ScreenInteractive::Fullscreen();

void exitor()
{
    screen.Exit();
}

int main()
{
    using namespace std::string_literals;
    std::cout << "Hello World!" << std::endl;

    Application a(exitor);

    screen.Loop(a.render());
}
