#include "GekkoEngine.h"
#include <iostream>

Gekko::Trigger::Trigger()
{
    action = None;
    variable_idx = 0;
    trigger_value = 0;
}

Gekko::Trigger::Trigger(TriggerAction act, int var_idx, int32_t trig_value)
{
    action = act;
    variable_idx = var_idx;
    trigger_value = trig_value;
}

bool Gekko::Trigger::IsActive(int32_t* vars) const
{
    if (action == TriggerAction::None) {
        return true;
    }

    int32_t var_value = vars[variable_idx];

    switch (action) {
        case TriggerAction::LT: return var_value < trigger_value;
        case TriggerAction::LTE: return var_value <= trigger_value;
        case TriggerAction::GT: return var_value > trigger_value;
        case TriggerAction::GTE: return var_value >= trigger_value;
        case TriggerAction::EQ: return var_value == trigger_value;
        case TriggerAction::NEQ: return var_value != trigger_value;
        default: return false;
    }
}

void Gekko::Transition::Init()
{
    in_use = false;
    trigger = Trigger();
}

bool Gekko::Transition::CanTransition(uint32_t current_id, int32_t* vars) const
{
    return (current_id == from_state_id) && trigger.IsActive(vars);
}

Gekko::Character::Character()
{
    in_use = false;

    chara_mem_size = 0;
    current_state_id = 0;

    max_var_count = 0;
    max_state_count = 0;
    max_transition_count = 0;

    vars = nullptr;
    states = nullptr;
    transitions = nullptr;

    frame_counter = 0;
}

void Gekko::Character::Init(int max_vars, int max_states, int max_transitions)
{
    current_state_id = UINT32_MAX;

    max_var_count = max_vars;
    max_state_count = max_states;
    max_transition_count = max_transitions;

    chara_mem_size = CalcCharaSize();

    chara_mem = std::make_unique<uint8_t[]>(chara_mem_size);
    std::memset(chara_mem.get(), 0, chara_mem_size);

    frame_counter = 0;

    // Set pointers to allocated memory regions
    SetCharaOffsets();

    // Init states
    for (int i = 0; i < max_state_count; i++) {
        states[i].Init();
    }

    // Init Transitions
    for (int i = 0; i < max_transition_count; i++) {
        transitions[i].Init();
    }
}

void Gekko::Character::SetVariable(int index, int32_t value)
{
    if (index >= 0 && index < max_var_count) {
        vars[index] = value;
    }
}

void Gekko::Character::AddState(State* state)
{
    if (state == nullptr) {
        return;
    }

    for (int i = 0; i < max_state_count; i++) {
        if (!states[i].in_use) {
            states[i] = *state;
            states[i].in_use = true;
            break;
        }
    }
}

void Gekko::Character::AddTransition(Transition* transition)
{
    if (transition == nullptr) {
        return;
    }

    for (int i = 0; i < max_transition_count; i++) {
        if (!transitions[i].in_use) {
            transitions[i] = *transition;
            transitions[i].in_use = true;
            break;
        }
    }
}

void Gekko::Character::Update()
{
    // Exit early if no valid state or transitions
    if (current_state_id == UINT32_MAX) {
        // Handle initial state transition
        current_state_id = 0;
        states[current_state_id].on_enter();
    }

    // 32 for now since thats the max transitions per singular state
    // later look if it needs expanding
    int num_transitions = std::min(32, max_transition_count);
    for (int i = 0; i < num_transitions; ++i) {
        // maybe look into transition priority? // TODO
        if (Util::IsBitSet(states[current_state_id].allowed_transitions, i) && transitions[i].CanTransition(current_state_id, vars)) {

            // exit the current state
            states[current_state_id].on_exit();

            // transition to the new state
            current_state_id = transitions[i].to_state_id;

            // reset frame counter 
            frame_counter = 0;

            // enter the new state
            states[current_state_id].on_enter();

            // only one transition per frame
            break;
        }
    }
    // update frame counter
    frame_counter++;

    // update character state
    states[current_state_id].on_update();
}

int32_t Gekko::Character::CalcCharaSize()
{
    return
        (sizeof(int32_t) * max_var_count) +
        (sizeof(State) * max_state_count) +
        (sizeof(Transition) * max_transition_count);
}

void Gekko::Character::SetCharaOffsets()
{
    vars = (int32_t*)chara_mem.get();
    states = (State*)((uint8_t*)chara_mem.get() + (max_var_count * sizeof(int32_t)));
    transitions = (Transition*)((uint8_t*)chara_mem.get() +
        (max_var_count * sizeof(int32_t)) + (max_state_count * sizeof(State)));
}

Gekko::Engine::Engine(int max_characters)
{
    max_character_count = max_characters;
    characters = std::make_unique<Character[]>(max_characters);
}

Gekko::Character* Gekko::Engine::GetCharacter(int index)
{
    if (index >= max_character_count || index < 0 || !characters[index].in_use) {
        return nullptr;
    }

    return &characters[index];
}

Gekko::Character* Gekko::Engine::CreateCharacter(int max_vars, int max_states, int max_transitions)
{
    for (int i = 0; i < max_character_count; i++) {
        if (!characters[i].in_use) {
            characters[i].Init(max_vars, max_states, max_transitions);
            characters[i].in_use = true;
            return &characters[i];
        }
    }
    return nullptr;
}

void Gekko::Engine::Update()
{
    for (int i = 0; i < max_character_count; ++i) {
        // dont update empty slots.
        if (characters[i].in_use) {
            characters[i].Update();
        }
    }
}

void Gekko::Engine::LoadEngineState(EngineState* state)
{
    // do later
}

Gekko::EngineState* Gekko::Engine::SaveEngineState(int* state_size)
{
    // do later
    return nullptr;
}

void Gekko::Util::SetBit(uint32_t& bitfield, int position)
{
    bitfield |= (1 << position);
}

bool Gekko::Util::IsBitSet(const uint32_t& bitfield, int position)
{
    return bitfield & (1 << position);
}

void Gekko::State::Init()
{
    in_use = false;

    allowed_transitions = 0;

    on_enter = nullptr;
    on_update = nullptr;
    on_exit = nullptr;
}
