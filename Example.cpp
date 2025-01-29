#include "GekkoEngine.h"

int main() {
    Engine en;

    en.CreateCharacterInstance("ExampleCharacter", 0);

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";
    // Simulate updates
    for (int i = 0; i < 100; ++i) {
        if (i == 5) {
            en.CreateCharacterInstance("ExampleCharacter", 0);
        }
        std::cout << "Frame " << i << ":\n";
        en.Update();
        std::cout << "-----------------\n";
    }

    return 0;
}
