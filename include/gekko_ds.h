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

    // A simple dynamic array for trivially copyable types.
    template <typename T>
    class Vec {
        static_assert(std::is_trivially_copyable_v<T>, "DS::Vec<T> only supports trivially copyable types");

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

        void free_pointers() {
            if constexpr (std::is_pointer_v<T>) {
                for (uint32_t i = 0; i < _size; ++i) {
                    delete _data[i];
                    _data[i] = nullptr;
                }
            }
        }

    public:
        ~Vec() {
            free_pointers();
            delete[] _data;
        }

        static Vec<T> copy_vec(const Vec<T>& other) {
            Vec<T> new_vec;

            new_vec._size = other._size;
            new_vec._capacity = other._capacity;
            new_vec._data = new T[other._capacity];

            if (new_vec._data) {
                std::memcpy(new_vec._data, other._data, new_vec._size * sizeof(T));
            }

            return new_vec;
        }

        void push_back(const T& value) {
            if (_size == _capacity) {
                grow();
            }
            _data[_size++] = value;
        }

        void pop_back() {
            if (_size > 0) {
                if constexpr (std::is_pointer_v<T>) {
                    delete _data[_size - 1];
                    _data[_size - 1] = nullptr;
                }
                _size--;
            }
        }

        void remove_at(uint32_t index) {
            if (index >= _size) return;
            // swap with the last element
            if (index != _size - 1) {
                std::swap(_data[index], _data[_size - 1]);
            }
            // pop off the last element
            pop_back();
        }

        void remove_first(const T& value) {
            for (uint32_t i = 0; i < size(); i++){
                if (_data[i] == value) {
                    remove_at(i);
                    return;
                }
            }
        }

        T& back() { return _data[_size - 1]; }
        const T& back() const { return _data[_size - 1]; }

        T& operator[](uint32_t i) { return _data[i]; }
        const T& operator[](uint32_t i) const { return _data[i]; }

        uint32_t size() const { return _size; }
        uint32_t capacity() const { return _capacity; }

        bool empty() const { return _size == 0; }

        void clear() {
            free_pointers();
            _size = 0;
        }

        T* data() { return _data; }

        T* begin() { return _data; }
        T* end() { return _data + _size; }

        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }
    };

    // SparseSet manages a collection of _entities and their associated data.
    // It uses a signed integer type for IDs, with -1 representing an invalid ID.
    // Active _entities are stored contiguously at the beginning of the _dense vector.
    // Removed IDs are stored in _free_ids for reuse.
    template <typename Q, typename T>
    class SparseSet {
        static_assert(std::is_integral_v<Q>, "DS::SparseSet<Q, T> requires Q to be an integral type");
        static_assert(std::is_signed_v<Q>, "DS::SparseSet<Q, T> requires Q to be signed because -1 is used as INVALID_ID");
        static_assert(std::is_trivially_copyable_v<T>, "DS::SparseSet<Q, T> requires T to be trivially copyable");

        Vec<T> _dense;       // Stores the actual data.

        Vec<Q> _sparse;      // Maps entity ID to its index in the dense vector.
        Vec<Q> _entities;    // Maps dense index back to its entity ID.
        Vec<Q> _free_ids;    // List of IDs available for reuse.

        Q _next_id = 0;      // Next new ID to assign.
        Q _active_count = 0; // Number of active (enabled) _entities.

        // Swap the entries at two indices in the _dense vector (and update the mapping).
        void swap_dense(Q index1, Q index2) {
            std::swap(_dense[index1], _dense[index2]);
            std::swap(_entities[index1], _entities[index2]);
            _sparse[_entities[index1]] = index1;
            _sparse[_entities[index2]] = index2;
        }

    public:
        static constexpr Q INVALID_ID = -1;

        // Checks if the given id is valid (allocated and not removed).
        bool is_valid(Q id) const {
            if (id < 0) return false;
            uint32_t index = static_cast<uint32_t>(id);
            return index < _sparse.size() && _sparse[index] != INVALID_ID;
        }

        bool is_enabled(Q id) const {
            return is_valid(id) && (_sparse[id] < _active_count);
        }

        // Returns true if the set contains the given entity (i.e. if it is valid).
        bool contains(Q id) const {
            return is_valid(id);
        }

        // Inserts a new element; new elements start as active.
        Q insert(const T& value) {
            Q id;
            if (!_free_ids.empty()) {
                id = _free_ids.back();
                _free_ids.pop_back();
            }
            else {
                id = _next_id;
                if (_next_id == INVALID_ID) {
                    return INVALID_ID; // No more IDs available.
                }
                _next_id++;
            }

            // Ensure the sparse vector is large enough.
            while (static_cast<uint32_t>(id) >= _sparse.size()) {
                _sparse.push_back(INVALID_ID);
            }

            // Map the new entity to its dense index.
            _sparse[id] = _dense.size();
            _dense.push_back(value);
            _entities.push_back(id);

            // Swap the new element into the active region.
            if (_sparse[id] != _active_count) {
                swap_dense(_sparse[id], _active_count);
            }
            _active_count++;
            return id;
        }

        // Removes an entity from the set.
        void remove(Q id) {
            if (!is_valid(id)) {
                return;
            }

            Q index = _sparse[id];
            Q last_index = _dense.size() - 1;

            if (is_enabled(id)) {
                _active_count--;
            }

            if (index != last_index) {
                swap_dense(index, last_index);
            }

            _dense.pop_back();
            _entities.pop_back();

            _sparse[id] = INVALID_ID;

            _free_ids.push_back(id);
        }

        // Disables an active entity
        void disable(Q id) {
            if (is_valid(id) && is_enabled(id)) {
                Q index = _sparse[id];
                Q last_active = _active_count - 1;
                swap_dense(index, last_active);
                _active_count--;
            }
        }

        // Enables a disabled entity
        void enable(Q id) {
            if (is_valid(id) && !is_enabled(id)) {
                Q index = _sparse[id];
                swap_dense(index, _active_count);
                _active_count++;
            }
        }

        // Retrieves an entity by its ID.
        T& get(Q id) {
            if (!is_valid(id)) {
                throw std::out_of_range("Invalid ID");
            }
            return _dense[_sparse[id]];
        }

        const T& get(Q id) const {
            if (!is_valid(id)) {
                throw std::out_of_range("Invalid ID");
            }
            return _dense[_sparse[id]];
        }

        // Clears all entities.
        void clear() {
            _dense.clear();
            _entities.clear();
            _sparse.clear();
            _free_ids.clear();
            _next_id = 0;
            _active_count = 0;
        }

        // Returns the total number of entities (active(enabled) + active(disabled) + free).
        uint32_t size() const { return static_cast<Q>(_dense.size()); }

        // Returns the number of active (enabled) entities.
        uint32_t active_size() const { return _active_count; }

        // Returns the number of active (disabled) entities.
        uint32_t disabled_size() const { return static_cast<Q>(_dense.size()) - _active_count; }

        // Iterators over active entities.
        T* begin() { return _dense.begin(); }
        T* end() { return _dense.begin() + _active_count; }
        const T* begin() const { return _dense.begin(); }
        const T* end() const { return _dense.begin() + _active_count; }
    };
} // namespace Gekko::DS
