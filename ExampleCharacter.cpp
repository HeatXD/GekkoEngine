#include "GekkoEngine.h"

std::unique_ptr<CharacterBehaviour> ExampleCharacter;

void BuildBehaviour_ExampleCharacter() {
	ExampleCharacter = std::make_unique<CharacterBehaviour>();
	ExampleCharacter->Init(1, 1, 4);
    // Walk State
    auto& walkState = ExampleCharacter->movement_states[0];
    walkState.OnEnter = [](Character* ctx) {
        std::cout << "Entering Walk State\n";
        ctx->vars[0] = 10; // Set timer for Walk
        };
    walkState.OnUpdate = [](Character* ctx) {
        std::cout << "Walking...\n";
        ctx->vars[0]--; // Decrement the timer
        };
    walkState.OnExit = [](Character* ctx) {
        std::cout << "Exiting Walk State\n";
        };

    // Run State
    auto& runState = ExampleCharacter->movement_states[1];
    runState.OnEnter = [](Character* ctx) {
        std::cout << "Entering Run State\n";
        ctx->vars[0] = 20; // Set timer for Run
        };
    runState.OnUpdate = [](Character* ctx) {
        std::cout << "Running...\n";
        ctx->vars[0]--; // Decrement the timer
        };
    runState.OnExit = [](Character* ctx) {
        std::cout << "Exiting Run State\n";
        };

    // Jump State
    auto& jumpState = ExampleCharacter->movement_states[2];
    jumpState.OnEnter = [](Character* ctx) {
        std::cout << "Entering Jump State\n";
        ctx->vars[0] = 10; // Set timer for Jump
        };
    jumpState.OnUpdate = [](Character* ctx) {
        std::cout << "Jumping...\n";
        ctx->vars[0]--; // Decrement the timer
        };
    jumpState.OnExit = [](Character* ctx) {
        std::cout << "Exiting Jump State\n";
        };

    // Fall State
    auto& fallState = ExampleCharacter->movement_states[3];
    fallState.OnEnter = [](Character* ctx) {
        std::cout << "Entering Fall State\n";
        ctx->vars[0] = 10; // Set timer for Fall
        };
    fallState.OnUpdate = [](Character* ctx) {
        std::cout << "Falling...\n";
        ctx->vars[0]--; // Decrement the timer
        };
    fallState.OnExit = [](Character* ctx) {
        std::cout << "Exiting Fall State\n";
        };

    // Transitions
    walkState.AddTransition(new Transition{
        1, 1, [](Character* ctx) { return ctx->vars[0] <= 0; } // Walk → Run after timer expires
        });
    runState.AddTransition(new Transition{
        1, 2, [](Character* ctx) { return ctx->vars[0] <= 0; } // Run → Jump after timer expires
        });
    jumpState.AddTransition(new Transition{
        1, 3, [](Character* ctx) { return ctx->vars[0] <= 0; } // Jump → Fall after timer expires
        });
    fallState.AddTransition(new Transition{
        1, 0, [](Character* ctx) { return ctx->vars[0] <= 0; } // Fall → Walk after timer expires
        });

}

struct CharacterHook_ExampleCharacter {
	CharacterHook_ExampleCharacter() {
		BuildBehaviour_ExampleCharacter();
		Engine::RegisterCharacterBehaviour("ExampleCharacter", ExampleCharacter.get());
	}
};

static CharacterHook_ExampleCharacter characterHook_ExampleCharacter;

std::unique_ptr<CharacterBehaviour> ExampleCharacter2;

void BuildBehaviour_ExampleCharacter2() {
	ExampleCharacter2 = std::make_unique<CharacterBehaviour>();
	ExampleCharacter2->Init(1, 3, 3);
}

struct CharacterHook_ExampleCharacter2 {
	CharacterHook_ExampleCharacter2() {
		BuildBehaviour_ExampleCharacter2();
		Engine::RegisterCharacterBehaviour("ExampleCharacter2", ExampleCharacter2.get());
	}
};

static CharacterHook_ExampleCharacter2 characterHook_ExampleCharacter2;
