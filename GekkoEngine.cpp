#include "GekkoEngine.h"

void Character::Init(int num_vars, int num_combat_states, int num_movement_states)
{
    max_vars = num_vars;
    max_combat_states = num_combat_states;
    max_movement_states = num_movement_states;

    vars = std::make_unique<int32_t[]>(max_vars);
    combat_states = std::make_unique<State[]>(max_combat_states);
    movement_states = std::make_unique<State[]>(max_movement_states);

    combat_state_idx = UINT16_MAX;
    movement_state_idx = UINT16_MAX;

    combat_state_frame = 0;
    movement_state_frame = 0;
}

void Character::Update()
{
    if (combat_state_idx == UINT16_MAX) {
        // set initial movement and combat state
        combat_state_idx = 0;
        movement_state_idx = 0;
        // enter initial movement and combat state
        combat_states[combat_state_idx].OnEnter();
        movement_states[movement_state_idx].OnEnter();
    }



    // update frames
    movement_state_frame++;
    combat_state_frame++;

    // update current states
    movement_states[movement_state_idx].OnUpdate();
    combat_states[combat_state_idx].OnUpdate();
}

void State::AddTransition(Transition* transition)
{
}
