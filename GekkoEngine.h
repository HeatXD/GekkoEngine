#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

struct Character;

struct Transition {
    int32_t priority;
    uint16_t target_state_idx;

    std::function<bool(Character* ctx)> IsValid = [](Character* ctx) { return false; };
};

struct State {
    std::vector<Transition> transitions;

    bool interrupt_combat_state = false;
    bool interrupt_movement_state = false;

    std::function<void(Character* ctx)> OnEnter = [](Character* ctx) {};
    std::function<void(Character* ctx, uint32_t frame)> OnUpdate = [](Character* ctx, uint32_t frame) {};
    std::function<void(Character* ctx)> OnExit = [](Character* ctx) {};

    void AddTransition(Transition* transition);
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
    std::vector<Character> characters;

    static std::unordered_map<std::string, const CharacterBehaviour*>& GetCharacterRegister();
    static void RegisterCharacterBehaviour(std::string name, const CharacterBehaviour* behaviour);

    int NumRegisteredCharacters();

    void CreateCharacterInstance(std::string chara_name, int player_controller);

    void Update();
};
