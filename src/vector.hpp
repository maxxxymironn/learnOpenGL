#pragma once

template <typename T>
struct vec2 {
    T x{}, y{};

    vec2() = default;
    vec2(T x, T y): x(x), y(y) {}
    template <typename U>
    vec2(const vec2<U>& other): x(other.x), y(other.y) { }

    vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2& operator*=(const T& n) {
        x *= n;
        y *= n;
        return *this;
    }
    
    vec2& operator/=(const T& n) {
        x /= n;
        y /= n;
        return *this;
    }
};

template <typename T>
vec2<T> operator+(vec2<T> v1, const vec2<T>& v2) {
    return v1 += v2;
}

template <typename T>
vec2<T> operator-(vec2<T> v1, const vec2<T>& v2) {
    return v1 -= v2;
}

template <typename T>
vec2<T> operator*(vec2<T> v, const T& n) {
    return v *= n;
}

template <typename T>
vec2<T> operator/(vec2<T> v, const T& n) {
    return v /= n;
}

using vec2i = vec2<int>;
using vec2f = vec2<float>;
using vec2d = vec2<double>;
