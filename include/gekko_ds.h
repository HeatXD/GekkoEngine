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

        T* data() { return _data; }
        T* begin() { return _data; }
        T* end() { return _data + _size; }

        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }
    };
} // namespace Gekko::DS
