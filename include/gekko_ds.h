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
    private:
        T* _data = nullptr;
        uint32_t _size = 0;
        uint32_t _capacity = 0;

        void alloc(uint16_t new_capacity) {
            T* new_data = new T[new_capacity];
            if (_data) {
                std::memcpy(new_data, _data, _size * sizeof(T)); // Copy old data
                delete[] _data; // Free old memory
            }
            _data = new_data;
            _capacity = new_capacity;
        }

    public:
        ~Vec() { delete[] _data; }

        void push_back(const T& value) {
            if (_size == _capacity) alloc(_capacity ? _capacity * 2 : 8);
            _data[_size++] = value;
        }

        void pop_back() { if (_size) _size--; }

        T& operator[](uint32_t index) {
            if (index >= _size) throw std::out_of_range("Index out of range");
            return _data[index];
        }

        const T& operator[](uint32_t index) const {
            if (index >= _size) throw std::out_of_range("Index out of range");
            return _data[index];
        }

        inline uint32_t size() const { return _size; }
        uint32_t capacity() const { return _capacity; }
        bool empty() const { return _size == 0; }

        void clear() { _size = 0; }

        T* data() { return _data; }
    };

} // namespace Gekko::DS
