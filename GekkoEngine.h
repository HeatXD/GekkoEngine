#pragma once

#include <cstdint>
#include <memory>
#include <functional>

namespace Gekko {
    struct Util {
        static void SetBit(uint32_t& bitfield, int position);
        static bool IsBitSet(const uint32_t& bitfield, int position);
    };

    enum TriggerAction {
        None = -1,
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
        int32_t trigger_value;

        Trigger();
        Trigger(TriggerAction act, int var_idx, int32_t trig_value);

        bool IsActive(int32_t* vars) const;
    };

    struct Transition {
        uint32_t from_state_id;
        uint32_t to_state_id;
        Trigger trigger;

        // checks whether this transition slot is available to be used.
        bool in_use;

        void Init();

        bool CanTransition(uint32_t current_id, int32_t* vars) const;
    };

    struct State {
        // checks whether this state slot is available to be used.
        bool in_use;

        // for now a 32 bit as a bitfield should be enough 
        // once a character has more then 32 unique transitions from a single state look at this! 
        uint32_t allowed_transitions;

        void Init();

        std::function<void()> on_enter;
        std::function<void()> on_exit;
        std::function<void()> on_update;
    };

    struct Character {
        // custom character data
        std::unique_ptr<uint8_t[]> chara_mem;

        int32_t* vars;
        State* states;
        Transition* transitions;

        uint32_t current_state_id;

        int32_t chara_mem_size;

        int32_t max_var_count;
        int32_t max_state_count;
        int32_t max_transition_count;

        // checks whether this character slot is available to be used.
        bool in_use;

        uint32_t frame_counter;

        Character();

        void Init(int max_vars, int max_states, int max_transitions);

        void SetVariable(int index, int32_t value);

        void AddState(State* state);
        void AddTransition(Transition* transition);

        void Update();

    private:
         int32_t CalcCharaSize();
        void SetCharaOffsets();
    };

    struct EngineState {

    };

    struct Engine {
        std::unique_ptr<Character[]> characters;

        int32_t max_character_count;

        Engine(int max_characters);

        Character* GetCharacter(int index);
        Character* CreateCharacter(int max_vars, int max_states, int max_transitions);
        void Update();

        void LoadEngineState(EngineState* state);
        EngineState* SaveEngineState(int* state_size);
    };
}
