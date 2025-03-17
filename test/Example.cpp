#include "gekko_engine.h"


int main() {
    using namespace Gekko::Engine;

    Engine en;

    en.CreateCharacterInstance("ExampleCharacter", 0);

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";

    // Simulate updates
    for (int i = 1; i <= 400; i++)
    {
        std::cout << "Frame: " << i << "\n";
        if (i == 5) {
            en.CreateCharacterInstance("ExampleCharacter", 0);
        }
        en.Update();
        std::cout << "-----------------\n";
    }

    return 0;
}
