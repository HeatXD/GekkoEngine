#include "GekkoEngine.h"

int main() {
    Engine en;

    en.CreateCharacterInstance("ExampleCharacter", 0);
    en.CreateCharacterInstance("ExampleCharacter2", 0);

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";
    // Simulate updates
    for (int i = 0; i < 15; ++i) {
        std::cout << "Frame " << i << ":\n";
        en.Update();
        std::cout << "-----------------\n";
    }

    return 0;
}
