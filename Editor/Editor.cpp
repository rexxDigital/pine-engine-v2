#include <iostream>
#include <Pine/Pine.hpp>

int main()
{
    if (!Pine::Setup()) {
        return 1;
    }



    Pine::Run();

    Pine::Terminate();

    return 0;
}