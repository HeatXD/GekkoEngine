#include "GekkoEngine.h"

int main() {
    Engine en;

    std::cout << "Character Count: " << en.NumRegisteredCharacters() << "\n";
    // Simulate updates
    for (int i = 0; i < 15; ++i) {
        std::cout << "Frame " << i << ":\n";
        std::cout << "-----------------\n";
    }

    return 0;
}
