#include "GekkoEngine.h"

struct CombatTransition : Transition {
    CombatTransition(int32_t p, uint16_t t) {
        priority = p;
        target_state_idx = t;
    }

    bool IsValid(Character* ctx) override {
        return ctx->combat_state_frame > 10;
    }
};

struct MovementTransition : Transition {
    MovementTransition(int32_t p, uint16_t t) {
        priority = p;
        target_state_idx = t;
    }

    bool IsValid(Character* ctx) override {
        return ctx->movement_state_frame > 5;
    }
};

// Custom combat state
struct CombatState : State {
    void OnEnter() override {
        std::cout << "Entering Combat State\n";
    }

    void OnUpdate() override {
        std::cout << "Updating Combat State\n";
    }

    void OnExit() override {
        std::cout << "Exiting Combat State\n";
    }
};

// Custom movement state
struct MovementState : State {
    void OnEnter() override {
        std::cout << "Entering Movement State\n";
    }

    void OnUpdate() override {
        std::cout << "Updating Movement State\n";
    }

    void OnExit() override {
        std::cout << "Exiting Movement State\n";
    }
};

int main() {
    // Create a character
    Character character;
    character.Init(10, 2, 2); // 10 vars, 2 combat states, 2 movement states

    // Define combat states
    character.combat_states[0] = CombatState();
    character.combat_states[1] = CombatState();

    // Define movement states
    character.movement_states[0] = MovementState();
    character.movement_states[1] = MovementState();

    // Add transitions to combat states
    character.combat_states[0].AddTransition(new CombatTransition{ 1, 1 }); // Transition to state 1
    character.combat_states[1].AddTransition(new CombatTransition{ 2, 0 }); // Transition back to state 0

    // Add transitions to movement states
    character.movement_states[0].AddTransition(new MovementTransition{ 1, 1 }); // Transition to state 1
    character.movement_states[1].AddTransition(new MovementTransition{ 2, 0 }); // Transition back to state 0

    // Simulate updates
    for (int i = 0; i < 15; ++i) {
        std::cout << "Frame " << i << ":\n";
        character.Update();
        std::cout << "-----------------\n";
    }

    return 0;
}
