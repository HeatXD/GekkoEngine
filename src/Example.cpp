#include "gekko_engine.h"

#include <chrono>
#include <thread>

int main() {
    using namespace std::chrono_literals;

    Engine en;

    en.CreateCharacterInstance("ExampleCharacter", 0);

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";
    // Simulate updates
    //int i = 0;
    //while (true) {
    //    if (i == 5) {
    //        en.CreateCharacterInstance("ExampleCharacter", 0);
    //    }
    //    std::cout << "Frame " << i << ":\n";
    //    en.Update();
    //    std::cout << "-----------------\n";
    //    i++;
    //    std::this_thread::sleep_for(16ms);
    //}

    return 0;
}
