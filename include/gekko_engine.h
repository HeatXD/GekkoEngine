#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <utility>
#include <type_traits>

#include "gekko_math.h"

template<typename>
class GekkoFunc;

template<typename R, typename... Args>
class GekkoFunc<R(Args...)> {
    using FuncPtr = R(*)(Args...);
    FuncPtr fn_ptr = nullptr;

    static_assert(std::is_void_v<R> ||
        (std::is_default_constructible_v<R> && !std::is_reference_v<R>),
        "Return type must be void or non-reference default-constructible type");

public:
    GekkoFunc() noexcept = default;
    GekkoFunc(FuncPtr ptr) noexcept : fn_ptr(ptr) {}  // Direct function pointer

    template<typename F, typename = std::enable_if_t<
        std::is_convertible_v<F, FuncPtr>
        >>
        GekkoFunc(F f) noexcept : fn_ptr(f) {}  // Safe conversion

    R operator()(Args... args) const {
        if (fn_ptr) {
            return fn_ptr(std::forward<Args>(args)...);
        }
        if constexpr (!std::is_void_v<R>) {
            return R{};  // Safe default return
        }
    }

    explicit operator bool() const noexcept { return fn_ptr != nullptr; }
};

struct Character;

struct Transition {
    int32_t priority;
    uint16_t target_state_idx;

    GekkoFunc<bool(Character*)> IsValid;
};

struct State {
    std::vector<Transition> transitions;

    GekkoFunc<void(Character*)> OnEnter;
    GekkoFunc<void(Character*, uint32_t)> OnUpdate;
    GekkoFunc<void(Character*)> OnExit;

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
