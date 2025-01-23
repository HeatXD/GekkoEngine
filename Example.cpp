#include "GekkoEngine.h"
#include <iostream>

// State behaviors
void IdleEnter() { std::cout << "Entering Idle state.\n"; }
void IdleUpdate() { std::cout << "Updating Idle state.\n"; }
void IdleExit() { std::cout << "Exiting Idle state.\n"; }

void RunningEnter() { std::cout << "Entering Running state.\n"; }
void RunningUpdate() { std::cout << "Updating Running state.\n"; }
void RunningExit() { std::cout << "Exiting Running state.\n"; }

void JumpingEnter() { std::cout << "Entering Jumping state.\n"; }
void JumpingUpdate() { std::cout << "Updating Jumping state.\n"; }
void JumpingExit() { std::cout << "Exiting Jumping state.\n"; }

int main() {
    // Create the engine with space for 1 character
    Gekko::Engine engine(1);

    // Create a character with space for 5 variables, 3 states, and 5 transitions
    auto character = engine.CreateCharacter(5, 3, 5);

    // Initialize variables
    character->SetVariable(0, 0);  // Speed
    character->SetVariable(1, 0);  // Jump Count

    // Define states
    Gekko::State idle, running, jumping;

    idle.Init();
    idle.on_enter = IdleEnter;
    idle.on_update = IdleUpdate;
    idle.on_exit = IdleExit;

    running.Init();
    running.on_enter = RunningEnter;
    running.on_update = RunningUpdate;
    running.on_exit = RunningExit;

    jumping.Init();
    jumping.on_enter = JumpingEnter;
    jumping.on_update = JumpingUpdate;
    jumping.on_exit = JumpingExit;

    // Define transitions with various triggers
    Gekko::Transition idleToRunning, runningToJumping, jumpingToIdle;

    idleToRunning.Init();
    idleToRunning.from_state_id = 0;  // Idle
    idleToRunning.to_state_id = 1;    // Running
    idleToRunning.trigger = { Gekko::TriggerAction::GT, 0, 5 }; // Speed > 5

    runningToJumping.Init();
    runningToJumping.from_state_id = 1; // Running
    runningToJumping.to_state_id = 2;   // Jumping
    runningToJumping.trigger = { Gekko::TriggerAction::EQ, 1, 1 }; // Jump Count == 1

    jumpingToIdle.Init();
    jumpingToIdle.from_state_id = 2;   // Jumping
    jumpingToIdle.to_state_id = 0;     // Idle
    jumpingToIdle.trigger = { Gekko::TriggerAction::LTE, 0, 0 }; // Speed <= 0

    // Set allowed transitions
    Gekko::Util::SetBit(idle.allowed_transitions, 0); // only allow to run from idle.
    Gekko::Util::SetBit(running.allowed_transitions, 1); // only allow to jump from run.
    Gekko::Util::SetBit(jumping.allowed_transitions, 2); // only allow to idle from run.

    // Add states to the character
    character->AddState(&idle);
    character->AddState(&running);
    character->AddState(&jumping);

    // Add transitions to the character
    character->AddTransition(&idleToRunning);
    character->AddTransition(&runningToJumping);
    character->AddTransition(&jumpingToIdle);

    auto p1 = engine.GetCharacter(0);

    // Simulate a game loop
    for (int frame = 0; frame < 10; ++frame) {
        std::cout << "GFrame: " << frame << ", PFrame: " << p1->frame_counter << "\n";

        if (frame == 1) {
            p1->SetVariable(0, 6); // Speed > 5 triggers Idle -> Running
        }
        else if (frame == 3) {
            p1->SetVariable(1, 1); // Jump Count == 1 triggers Running -> Jumping
        }
        else if (frame == 5) {
            p1->SetVariable(0, 0); // Speed <= 0 triggers Jumping -> Idle
        }

        engine.Update(); // Process the game engine
    }

    return 0;
}
