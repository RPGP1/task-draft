#include <iostream>
#include <random>

#include "task_includes.hpp"

struct Human{
    bool alive() const
    {
        char result;

        std::cout << "Am I alive? [Y/n]";
        std::cin >> result;

        if (result == 'y' || result == 'Y') {
            return true;
        } else {
            std::cout << "\nsucceeded to the next generation...\n";
            return false;
        }
    }
};

void TRY()
{
    std::cout << "Just do it!" << std::endl;
}

bool SUCCESS()
{
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_int_distribution<> rnd(0, 1);

    return rnd(mt) == 0;
}

void REVIEW()
{
    std::cout << "What made difference?" << std::endl;
}


int main()
{
    using namespace TaskManager;

    Human I{};

    auto life = While([I] { return I.alive(); })(
        [] { TRY(); },
        If[ ([] { return SUCCESS(); }) ](
            [] { std::cout << "Happy Party!" << std::endl; }
        )->Else(
            [] { REVIEW(); }
        )
    );

    life.start();
    while(life.running()){
        life.resume();
    }


    return 0;
}
