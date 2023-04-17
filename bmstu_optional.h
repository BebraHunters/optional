#pragma once

#include <cstdint>
#include <exception>


namespace bmstu {
    class bad_optional_access : public std::exception {
        const char *what() const noexcept override {
            return "bad optional access";
        }
    };

    template<typename T>
    class optional {
    public:
        optional() = default;

        optional(const T &value) {
            is_initialized_ = true;
            T *val = new(&data_[0]) T{value};
            (void) (&val);
        }

        optional(T &&value) {
            is_initialized_ = true;
            T *val = new(&data_[0]) T(std::move(value));
            (void) (&val);
        }

        optional(const optional &other) {
            if (is_initialized_ && other.is_initialized_) {
                this->value() = other.value();
            } else if (!is_initialized_ && other.is_initialized_) {
                T *val = new(&data_[0])T(other.value());
                is_initialized_ = true;
                (void) &val;
            } else if (is_initialized_ && !other.is_initialized_) {
                this->reset();
            } else if (!is_initialized_ && !other.is_initialized_) {
                (void) (&data_[0]);
            }
        }

        optional &operator=(const T &value) {
            if (is_initialized_) {
                this->value() = value;
            } else {
                T *val = new(&data_[0])T(value);
                is_initialized_ = true;
                (void) (&val);
            }
            return *this;
        }

        optional &operator=(T &&value) {
            if (is_initialized_) {
                this->value() = std::move(value);
            } else {
                T *val = new(&data_[0])T(std::move(value));
                is_initialized_ = true;
                (void) (&val);
            }
            return *this;
        }

        optional &operator=(const optional &value) {
            if (this == &value) {
                return *this;
            }
            if (is_initialized_ && value.is_initialized_) {
                this->value() = value.value();
            } else if (!is_initialized_ && value.is_initialized_) {
                T *val = new(&data_[0])T(value.value());
                is_initialized_ = true;
                (void) &val;
            } else if (is_initialized_ && !value.is_initialized_) {
                this->reset();
            } else if (!is_initialized_ && !value.is_initialized_) {
                return *this;
            }
            return *this;
        }

        optional &operator=(optional &&value) {
            if (is_initialized_ && value.is_initialized_) {
                this->value() = std::move(value.value());
            } else if (!is_initialized_ && value.is_initialized_) {
                T *val = new(&data_[0])T(std::move(value.value()));
                is_initialized_ = true;
                (void) &val;
            } else if (is_initialized_ && !value.is_initialized_) {
                this->reset();
            } else if (!is_initialized_ && !value.is_initialized_) {
                return *this;
            }
            return *this;
        }

        T &operator*() &{
            return *(static_cast<T *>((void *) &data_[0]));
        }

        const T &operator*() const &{
            return *(static_cast<const T *>((void *) &data_[0]));
        }

        T *operator->() {
            return static_cast<T *>((void *) &data_[0]);
        }

        const T *operator->() const {
            return static_cast<const T *>((void *) &data_[0]);
        }

        T &&operator*() &&{
            return std::move(reinterpret_cast<T &>(*(T *) &data_[0]));
        }

        T &value() &{
            if (!is_initialized_) {
                throw bad_optional_access();
            }
            return reinterpret_cast<T &>(*(T *) &data_[0]);
        }

        const T &value() const &{
            if (!is_initialized_) {
                throw bad_optional_access();
            }
            return reinterpret_cast<const T &>(*(T *) &data_[0]);
        }

        template<typename ...Args>
        void emplace(Args &&... args) {
            if (is_initialized_) {
                reset();
            }
            is_initialized_ = true;
            T *val = new(&data_[0])T(std::forward<Args>(args)...);
            (void) (&val);
        }

        void reset() {
            if (is_initialized_) {
                static_cast<T *>((void *) &data_[0])->~T();
                is_initialized_ = false;
            }
        }

        ~optional() {
            if (is_initialized_) {
                T *ptr = static_cast<T *>((void *) &data_[0]);
                ptr->~T();
            }
        }

        bool has_any_val() const {
            return is_initialized_;
        }

    private:
        alignas(T) uint8_t data_[sizeof(T)];
        bool is_initialized_ = false;
    };
}
