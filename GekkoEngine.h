#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

struct Character;

struct Transition {
    int32_t priority;
    uint16_t target_state_idx;

    virtual bool IsValid(Character* ctx) { return false; };
};

struct State {
    std::vector<Transition> transitions;

    bool interrupt_combat_state = false;
    bool interrupt_movement_state = false;

    virtual void OnEnter() { std::cout << "Entering base state\n"; }
    virtual void OnUpdate() { std::cout << "Updating base state\n"; }
    virtual void OnExit() { std::cout << "Exiting base state\n"; }

    void AddTransition(Transition* transition);
};

struct Character {
    std::unique_ptr<int32_t[]> vars;
    std::unique_ptr<State[]> combat_states;
    std::unique_ptr<State[]> movement_states;

    uint16_t max_vars;
    uint16_t max_combat_states;
    uint16_t max_movement_states;

    uint16_t combat_state_idx;
    uint16_t movement_state_idx;

    uint32_t combat_state_frame;
    uint32_t movement_state_frame;

    void Init(int num_vars, int num_combat_states, int num_movement_states);
    void Update();
};
