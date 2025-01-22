#include "GekkoEngine.h"

#include <memory>

bool Gekko::Trigger::IsActive(const int32_t vars[]) const
{
	int32_t var_value = vars[variable_idx];

	switch (action) {
		case TriggerAction::LT: return var_value < value;
		case TriggerAction::LTE: return var_value <= value;
		case TriggerAction::GT: return var_value > value;
		case TriggerAction::GTE: return var_value >= value;
		case TriggerAction::EQ: return var_value == value;
		case TriggerAction::NEQ: return var_value != value;
		default: return false;
	}
}

bool Gekko::Transition::CanTransition(uint32_t current_id, const int32_t vars[]) const
{
	return (current_id == from_state_id) && trigger.IsActive(vars);
}

Gekko::State::State()
{
	allowed_transitions = 0;

	OnEnter = nullptr;
	Update = nullptr;
	OnExit = nullptr;
}

void Gekko::State::ToggleTransitionBit(int index)
{
}

bool Gekko::State::IsTransitionBitSet(int index)
{
	return false;
}

Gekko::Character::Character(int max_vars, int max_states, int max_transitions)
{
	num_vars = max_vars;
	num_states = max_states;
	num_transitions = max_transitions;

	mem_size = 
		(sizeof(int32_t) * num_vars) + 
		(sizeof(State) * num_states) + 
		(sizeof(Transition) * num_transitions);

	char_mem = (uint8_t*)std::malloc(mem_size);

	if (!char_mem) {
		throw std::exception("Failed to allocate character memory");
	}

	std::memset(char_mem, 0, mem_size);

	// set ptr offsets
	vars = (int32_t*)char_mem;
	states = (State*)(char_mem + (num_vars * sizeof(int32_t)));
	transitions = (Transition*)(states + num_states);
}

void Gekko::Character::SetVariable(int index, int32_t value)
{
}

void Gekko::Character::AddState(State* state)
{
}

void Gekko::Character::AddTransition(Transition* transition)
{
}

void Gekko::Character::Update()
{
}

Gekko::Engine::Engine(int max_characters)
{

}

void Gekko::Engine::AddCharacter(Character* character)
{
}

void Gekko::Engine::Update()
{
}

void Gekko::Engine::LoadEngineState(EngineState* state)
{
}

Gekko::EngineState* Gekko::Engine::SaveEngineState()
{
	return nullptr;
}
