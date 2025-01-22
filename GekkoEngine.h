#pragma once

#include <cstdint>

namespace Gekko {

    enum TriggerAction {
        LT,   // Less Than
        LTE,  // Less Than or Equal To
        GT,   // Greater Than
        GTE,  // Greater Than or Equal To
        EQ,   // Equal To
        NEQ   // Not Equal To
    };

    struct Trigger {
        TriggerAction action;
        int32_t variable_idx;
        int32_t value;

        bool IsActive(const int32_t vars[]) const;
    };

    struct Transition {
        uint32_t from_state_id;
        uint32_t to_state_id;
        Trigger trigger;

        bool CanTransition(uint32_t current_id, const int32_t vars[]) const;
    };

    struct State {
        // for now a 32 bits as a bitfield should be enough 
        // once a character has more then 32 unique transitions look at this! 
        uint32_t allowed_transitions;

        void (*OnEnter)();
        void (*Update)();
        void (*OnExit)();

        State();

        void ToggleTransitionBit(int index);
        bool IsTransitionBitSet(int index);
    };

    struct Character {
        uint8_t* char_mem;

        int32_t* vars;
        State* states;
        Transition* transitions;

        uint32_t current_state_id;

        int32_t mem_size;

        int32_t num_vars;
        int32_t num_states;
        int32_t num_transitions;

        Character(int max_vars, int max_states, int max_transitions);

        void SetVariable(int index, int32_t value);

        void AddState(State* state);
        void AddTransition(Transition* transition);

        void Update();
    };

    struct EngineState {

    };

    struct Engine {
        Character* characters;
        int32_t max_characters;

        Engine(int max_characters);

        void AddCharacter(Character* character);
        void Update();

        void LoadEngineState(EngineState* state);
        EngineState* SaveEngineState();
    };
}
