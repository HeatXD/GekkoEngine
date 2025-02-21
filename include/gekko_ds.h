#pragma once

#include <stdexcept>
#include <utility>
#include <type_traits>

namespace Gekko::DS {

    template<typename>
    class Function;

    template<typename R, typename... Args>
    class Function<R(Args...)> {
        using FuncPtr = R(*)(Args...);
        FuncPtr fn_ptr = nullptr;

        static_assert(std::is_void_v<R> ||
            (std::is_default_constructible_v<R> && !std::is_reference_v<R>),
            "Return type must be void or non-reference default-constructible type");

    public:
        Function() noexcept = default;
        Function(FuncPtr ptr) noexcept : fn_ptr(ptr) {}  // Direct function pointer

        template<typename F, typename = std::enable_if_t<
            std::is_convertible_v<F, FuncPtr>
            >>
            Function(F f) noexcept : fn_ptr(f) {}  // Safe conversion

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

    template <typename T>
    struct Vec {
        static_assert(std::is_trivially_copyable_v<T>, "DS::Vec<T> only supports trivially copyable types");

    private:
        T* _data = nullptr;
        uint32_t _size = 0, _capacity = 0;

        void grow() {
            uint32_t new_capacity = _capacity ? _capacity * 2 : 8;
            T* new_data = new T[new_capacity];

            if (_data) {
                std::memcpy(new_data, _data, _size * sizeof(T));
                delete[] _data;
            }

            _data = new_data;
            _capacity = new_capacity;
        }

    public:
        ~Vec() { delete[] _data; }

        void push_back(const T& value) {
            if (_size == _capacity) grow();
            _data[_size++] = value;
        }

        void pop_back() { if (_size) _size--; }

        T& back() { return _data[_size - 1]; }
        const T& back() const { return _data[_size - 1]; }

        T& operator[](uint32_t i) { return _data[i]; }
        const T& operator[](uint32_t i) const { return _data[i]; }

        uint32_t size() const { return _size; }
        uint32_t capacity() const { return _capacity; }

        bool empty() const { return _size == 0; }

        void clear() { _size = 0; }

        int32_t contains(const T& value) const {
            for (uint32_t i = 0; i < _size; i++) {
                if (_data[i] == value) {
                    return i;
                }
            }
            return -1;
        }

        T* data() { return _data; }
        T* begin() { return _data; }
        T* end() { return _data + _size; }

        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }
    };

    template <typename Q, typename T>
    class SparseSet {
        static_assert(std::is_signed_v<Q>, "SparseSet<Q, T> requires Q to be a signed integer");
        static_assert(std::is_trivially_copyable_v<T>, "SparseSet<Q, T> requires T to be trivially copyable");

        Vec<T> dense;
        Vec<Q> sparse;
        Vec<Q> entities;
        Vec<Q> free_ids;
        Vec<uint32_t> enabled;

        Q next_id = 0;
        Q active_count = 0;

        // Consolidated index validation
        bool is_valid(Q id) const {
            return id >= 0 && id < sparse.size() && sparse[id] != INVALID_ID && entities[sparse[id]] == id;
        }

        // Centralized bit manipulation
        void set_bit(Q index, bool value) {
            ensure_enabled_size(index);
            uint32_t& bitset = enabled[index / 32];
            uint32_t mask = (1u << (index % 32));
            if (value) {
                bitset |= mask;
            }
            else {
                bitset &= ~mask;
            }
        }

        void ensure_enabled_size(Q id) {
            Q required_size = (id / 32) + 1;
            while (enabled.size() < required_size) {
                enabled.push_back(0);
            }
        }

        bool is_bit_enabled(Q index) const {
            return index / 32 < enabled.size() && (enabled[index / 32] & (1u << (index % 32)));
        }

        // Centralized swap logic for dense and entities vectors
        void swap_dense(Q index1, Q index2) {
            std::swap(dense[index1], dense[index2]);
            std::swap(entities[index1], entities[index2]);
            sparse[entities[index1]] = index1;
            sparse[entities[index2]] = index2;
        }

    public:
        static constexpr Q INVALID_ID = static_cast<Q>(-1);

        Q insert(const T& value) {
            Q id;
            if (!free_ids.empty()) {
                id = free_ids.back();
                free_ids.pop_back();
            }
            else {
                id = next_id++;
                if (next_id < 0) return INVALID_ID;
            }

            if (id >= sparse.size()) {
                while (id >= sparse.size()) {
                    sparse.push_back(INVALID_ID);
                }
            }

            sparse[id] = dense.size();
            dense.push_back(value);
            entities.push_back(id);
            set_bit(id, true);
            active_count++;
            return id;
        }

        void remove(Q id) {
            if (!is_valid(id)) return;

            Q index = sparse[id];
            Q last_index = dense.size() - 1;

            if (is_bit_enabled(id)) {
                active_count--;
            }

            if (index != last_index) {
                swap_dense(index, last_index);
            }

            dense.pop_back();
            entities.pop_back();

            sparse[id] = INVALID_ID;
            free_ids.push_back(id);
            set_bit(id, false);
        }

        void enable(Q id) {
            if (is_valid(id) && !is_enabled(id)) {
                Q index = sparse[id];
                Q first_inactive = active_count;

                swap_dense(index, first_inactive);

                set_bit(id, true);
                active_count++;
            }
        }

        void disable(Q id) {
            if (is_valid(id) && is_enabled(id)) {
                Q index = sparse[id];
                Q last_active = active_count - 1;

                swap_dense(index, last_active);

                set_bit(id, false);
                active_count--;
            }
        }

        bool is_enabled(Q id) const {
            return is_valid(id) && is_bit_enabled(id);
        }

        bool contains(Q id) const {
            return is_valid(id);
        }

        T& get(Q id) {
            if (!is_valid(id) || !is_enabled(id)) {
                throw std::out_of_range("Invalid or Disabled ID");
            }
            return dense[sparse[id]];
        }

        const T& get(Q id) const {
            if (!is_valid(id) || !is_enabled(id)) {
                throw std::out_of_range("Invalid or Disabled ID");
            }
            return dense[sparse[id]];
        }

        void clear() {
            dense.clear();
            entities.clear();
            sparse.clear();
            free_ids.clear();
            enabled.clear();
            next_id = 0;
            active_count = 0;
        }

        Q size() const {
            return static_cast<Q>(dense.size());
        }

        Q active_size() const {
            return active_count;
        }

        T* begin() { return dense.begin(); }
        T* end() { return dense.begin() + active_count; }
        const T* begin() const { return dense.begin(); }
        const T* end() const { return dense.begin() + active_count; }
    };

} // namespace Gekko::DS
