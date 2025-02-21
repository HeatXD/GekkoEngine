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
            if (_size == _capacity)
                grow();
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

        T* data() { return _data; }
        T* begin() { return _data; }
        T* end() { return _data + _size; }
        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }
    };

    // SparseSet manages a collection of entities and their associated data.
    // It uses a signed integer type for IDs, with -1 representing an invalid ID.
    // Active entities are stored contiguously at the beginning of the dense vector.
    // Removed IDs are stored in free_ids for reuse.
    template <typename Q, typename T>
    class SparseSet {
        static_assert(std::is_integral_v<Q>, "SparseSet<Q, T> requires Q to be an integral type");
        static_assert(std::is_signed_v<Q>, "SparseSet<Q, T> requires Q to be signed because -1 is used as INVALID_ID");

        Vec<T> dense;       // Stores the actual data.

        Vec<Q> sparse;      // Maps entity ID to its index in the dense vector.
        Vec<Q> entities;    // Maps dense index back to its entity ID.
        Vec<Q> free_ids;    // List of IDs available for reuse.

        Q next_id = 0;      // Next new ID to assign.
        Q active_count = 0; // Number of active (enabled) entities.

        // Swap the entries at two indices in the dense vector (and update the mapping).
        void swap_dense(Q index1, Q index2) {
            std::swap(dense[index1], dense[index2]);
            std::swap(entities[index1], entities[index2]);
            sparse[entities[index1]] = index1;
            sparse[entities[index2]] = index2;
        }

    public:
        static constexpr Q INVALID_ID = -1;

        // Checks if the given id is valid (allocated and not removed).
        bool is_valid(Q id) const {
            if (id < 0)
                return false;
            uint32_t index = static_cast<uint32_t>(id);
            return index < sparse.size() && sparse[index] != INVALID_ID;
        }

  
        bool is_enabled(Q id) const {
            return is_valid(id) && (sparse[id] < active_count);
        }

        // Returns true if the set contains the given entity (i.e. if it is valid).
        bool contains(Q id) const {
            return is_valid(id);
        }

        // Inserts a new element; new elements start as active.
        Q insert(const T& value) {
            Q id;
            if (!free_ids.empty()) {
                id = free_ids.back();
                free_ids.pop_back();
            }
            else {
                id = next_id;
                if (next_id == INVALID_ID)
                    return INVALID_ID; // No more IDs available.
                next_id++;
            }

            // Ensure the sparse vector is large enough.
            while (static_cast<uint32_t>(id) >= sparse.size()) {
                sparse.push_back(INVALID_ID);
            }

            // Map the new entity to its dense index.
            sparse[id] = dense.size();
            dense.push_back(value);
            entities.push_back(id);

            // Swap the new element into the active region.
            if (sparse[id] != active_count) {
                swap_dense(sparse[id], active_count);
            }
            active_count++;
            return id;
        }

        // Removes an entity from the set.
        void remove(Q id) {
            if (!is_valid(id))
                return;

            Q index = sparse[id];
            Q last_index = dense.size() - 1;
            if (is_enabled(id))
                active_count--;

            if (index != last_index) {
                swap_dense(index, last_index);
            }

            dense.pop_back();
            entities.pop_back();

            sparse[id] = INVALID_ID;

            free_ids.push_back(id);
        }

        // Disables an active entity
        void disable(Q id) {
            if (is_valid(id) && is_enabled(id)) {
                Q index = sparse[id];
                Q last_active = active_count - 1;
                swap_dense(index, last_active);
                active_count--;
            }
        }

        // Enables a disabled entity
        void enable(Q id) {
            if (is_valid(id) && !is_enabled(id)) {
                Q index = sparse[id];
                swap_dense(index, active_count);
                active_count++;
            }
        }

        // Retrieves an active entity by its ID.
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

        // Clears all entities.
        void clear() {
            dense.clear();
            entities.clear();
            sparse.clear();
            free_ids.clear();
            next_id = 0;
            active_count = 0;
        }

        // Returns the total number of entities (active + disabled).
        Q size() const { return static_cast<Q>(dense.size()); }

        // Returns the number of active (enabled) entities.
        Q active_size() const { return active_count; }

        // Returns the number of active (disabled) entities.
        Q disabled_size() const { return static_cast<Q>(dense.size()) - active_count; }

        // Iterators over active entities.
        T* begin() { return dense.begin(); }
        T* end() { return dense.begin() + active_count; }
        const T* begin() const { return dense.begin(); }
        const T* end() const { return dense.begin() + active_count; }
    };
} // namespace Gekko::DS
