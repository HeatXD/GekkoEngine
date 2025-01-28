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

    int32_t move_trans_count = movement_states[movement_state_idx].transitions.size();
    for (int32_t i = 0; i < move_trans_count; i++)
    {
        auto& transition = movement_states[movement_state_idx].transitions[i];
        if (transition.IsValid(this)) {
            movement_states[movement_state_idx].OnExit();
            movement_state_idx = transition.target_state_idx;
            movement_states[movement_state_idx].OnEnter();
            movement_state_frame = 0;
            break;
        }
    }

    int32_t combat_trans_count = combat_states[combat_state_idx].transitions.size();
    for (int32_t i = 0; i < combat_trans_count; i++)
    {
        auto& transition = combat_states[combat_state_idx].transitions[i];
        if (transition.IsValid(this)) {
            combat_states[combat_state_idx].OnExit();
            combat_state_idx = transition.target_state_idx;
            combat_states[combat_state_idx].OnEnter();
            combat_state_frame = 0;
            break;
        }
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
    transitions.push_back(*transition);
    std::sort(transitions.begin(), transitions.end(), [](Transition a, Transition b) {
        return a.priority > b.priority;
    });
}
