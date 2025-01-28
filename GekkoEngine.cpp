#include "GekkoEngine.h"

#include <algorithm>

void Character::Init(const CharacterBehaviour* bhvr)
{
    base = bhvr;

    vars = std::make_unique<int32_t[]>(base->max_vars);

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
        base->combat_states[combat_state_idx].OnEnter(this);
        base->movement_states[movement_state_idx].OnEnter(this);
    }

    // handle transition logic 
    HandleStateTransition(movement_state_idx, movement_state_frame, base->movement_states.get());
    HandleStateTransition(combat_state_idx, combat_state_frame, base->combat_states.get());

    // handle state interrupt logic
    // todo

    // update frames
    movement_state_frame++;
    combat_state_frame++;

    // update current states
    base->movement_states[movement_state_idx].OnUpdate(this);
    base->combat_states[combat_state_idx].OnUpdate(this);
}

void Character::HandleStateTransition(uint16_t& state_idx, uint32_t& state_frame, const State* states)
{
    for (const auto& transition : states[state_idx].transitions)
    {
        if (transition.IsValid(this)) {
            states[state_idx].OnExit(this);
            state_idx = transition.target_state_idx;
            states[state_idx].OnEnter(this);
            state_frame = 0;
            break;
        }
    }
}

void State::AddTransition(Transition* transition)
{
    transitions.push_back(*transition);
    std::sort(transitions.begin(), transitions.end(), [](Transition a, Transition b) {
        return a.priority > b.priority;
    });
}

void CharacterBehaviour::Init(int num_vars, int num_combat_states, int num_movement_states)
{
    max_vars = num_vars;
    max_combat_states = num_combat_states;
    max_movement_states = num_movement_states;

    combat_states = std::make_unique<State[]>(max_combat_states);
    movement_states = std::make_unique<State[]>(max_movement_states);
}

CharacterBehaviour::CharacterBehaviour()
{
    max_vars = 0;
    max_combat_states = 0;
    max_movement_states = 0;
}

std::unordered_map<std::string, const CharacterBehaviour*>& Engine::GetCharacterRegister()
{
    static std::unordered_map<std::string, const CharacterBehaviour*> map;
    return map;
}

void Engine::RegisterCharacterBehaviour(std::string name, const CharacterBehaviour* behaviour)
{
    auto& map = GetCharacterRegister();
    map[name] = behaviour;
}

int Engine::NumRegisteredCharacters()
{
    return GetCharacterRegister().size();
}
