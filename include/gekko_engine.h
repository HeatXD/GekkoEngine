#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "gekko_ds.h"
#include "gekko_physics.h"

namespace Gekko::Engine {

    struct Character;

    struct Transition {
        int32_t priority;
        uint16_t target_state_idx;

        DS::Function<bool(Character*)> IsValid;
    };

    struct State {
        DS::Vec<Transition> transitions;

        DS::Function<void(Character*)> OnEnter;
        DS::Function<void(Character*, uint32_t)> OnUpdate;
        DS::Function<void(Character*)> OnExit;

        void AddTransition(Transition transition);
    };

    struct CharacterBehaviour {
        std::unique_ptr<State[]> combat_states;
        std::unique_ptr<State[]> movement_states;

        uint16_t max_vars;
        uint16_t max_combat_states;
        uint16_t max_movement_states;

        CharacterBehaviour();

        void Init(int num_vars, int num_combat_states, int num_movement_states);
    };

    struct Character {
        std::unique_ptr<int32_t[]> vars;

        const CharacterBehaviour* base;

        uint16_t combat_state_idx;
        uint16_t movement_state_idx;

        uint32_t combat_state_frame;
        uint32_t movement_state_frame;

        Character();

        void Init(const CharacterBehaviour* bhvr);

        void Update();

    private:
        void HandleStateTransition(uint16_t& state_idx, uint32_t& state_frame, const State* states);
    };

    struct Engine {
        Physics::World phys_world;
        std::vector<Character> characters;

        static std::unordered_map<std::string, const CharacterBehaviour*>& GetCharacterRegister();
        static void RegisterCharacterBehaviour(std::string name, const CharacterBehaviour* behaviour);

        int NumRegisteredCharacters();

        void CreateCharacterInstance(std::string chara_name, int player_controller);

        void Update();
    };
}
