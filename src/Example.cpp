#include "gekko_engine.h"


int main() {
    using namespace Gekko::Engine;

    Engine en;

    en.CreateCharacterInstance("ExampleCharacter", 0);

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";

    // Simulate updates
    for (int i = 0; i < 400; i++)
    {
        if (i == 5) {
            en.CreateCharacterInstance("ExampleCharacter", 0);
        }
        std::cout << "Frame " << i << ":\n";
        en.Update();
        std::cout << "-----------------\n";
        i++;
    }

    return 0;
}
