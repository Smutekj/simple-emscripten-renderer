#pragma once

#include <cmath>
#include <cassert>
#include <type_traits>
#include <numeric>
#include <ostream>
#include <numbers>

namespace utils
{

    bool inline approx_equal(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        return std::abs(a - b) <= 1000. * std::max(std::abs(a), std::abs(b)) * epsilon;
    }
    bool inline approx_equal_zero(float a, float epsilon = std::numeric_limits<float>::epsilon())
    {
        return std::abs(a) <= 1000. * epsilon;
    }
    bool inline strictly_less(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        return (b - a) > std::max(std::abs(a), std::abs(b)) * 10000. * epsilon;
    }

    template <class T>
    struct Vector2
    {
        T x;
        T y;

        constexpr Vector2() = default;
        template <class T1, class T2>
        constexpr Vector2(T1 x, T2 y)
            : x(x), y(y) {}

        template <class VecType>
        constexpr Vector2<T> operator=(VecType &other_vec)
        {
            x = other_vec.x;
            y = other_vec.y;
            return {other_vec.x, other_vec.y};
        }

        template <class T1>
        constexpr Vector2(const struct Vector2<T1> &coords) : x(coords.x), y(coords.y) {}
        template <class T1>
        constexpr Vector2(const T1 &value) : x(value), y(value) {}

        constexpr Vector2 operator+(const Vector2 &v) const
        {
            return {x + v.x, y + v.y};
        }

        constexpr Vector2 operator/(float i) const
        {
            return {x / i, y / i};
        }
        constexpr Vector2 operator*(float i) const
        {
            return {x * i, y * i};
        }

        template <class T1>
        constexpr void operator+=(const utils::Vector2<T1> &v)
        {
            x += v.x;
            y += v.y;
        }

        template <class T1>
        constexpr void operator-=(const utils::Vector2<T1> &v)
        {
            x -= v.x;
            y -= v.y;
        }

        template <class T1>
        constexpr void operator/=(T1 i)
        {
            x /= i;
            y /= i;
        }

        template <class T1>
        constexpr void operator*=(T1 i)
        {
            x *= i;
            y *= i;
        }

        template <class Scalar>
        constexpr Vector2 operator*(Scalar i) const
        {
            return {x * i, y * i};
        }

        constexpr Vector2<T> operator-(const Vector2<T> &v) const
        {
                return {x - v.x, y - v.y};
        }

        constexpr bool operator==(const Vector2<T> &v) const
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                return approx_equal(v.x, x) && approx_equal(v.y, y);
            }
            else
            {
                return v.x == x && v.y == y;
            }
        }

        template <class X>
        friend std::ostream &operator<<(std::ostream &os, const Vector2<X> &vec);
    };

    template <class T>
    std::ostream &operator<<(std::ostream &os, const Vector2<T> &vec)
    {
        os << "[" << vec.x << ", " << vec.y << "] ";
        return os;
    }

    template <class T, class Scalar>
    constexpr Vector2<T> inline operator*(Scalar i, const Vector2<T> &v)
    {
        return v * i;
    }
    template <class T>
    constexpr Vector2<T> inline operator-(const Vector2<T> &v)
    {
        return {-v.x, -v.y};
    }

    using Vector2d = Vector2<double>;
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;

    template <typename T>
    constexpr inline float dot(const T &a, const T &b) { return a.x * b.x + a.y * b.y; }
    template <typename T>
    constexpr inline float dot(const T &&a, const T &&b) { return a.x * b.x + a.y * b.y; }

    template <typename T>
    constexpr inline float norm2(const T &a) { return dot(a, a); }
    template <typename T>
    constexpr inline float norm(const T &a) { return std::sqrt(norm2(a)); }
    template <typename T>
    constexpr inline float dist(const T &a, const T &b) { return std::sqrt(dot(a - b, a - b)); }

    template <class T>
    float inline cross(const utils::Vector2<T> &a, const utils::Vector2<T> &b)
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            return static_cast<float>(a.x * b.y) - static_cast<float>(a.y * b.x);
        }
        return a.x * b.y - a.y * b.x;
    }

    template <class T>
    float inline orient(const utils::Vector2<T> &a, const utils::Vector2<T> &b, const utils::Vector2<T> &c)
    {
        return cross(b - a, c - a);
    }

    template <class VecType>
    float inline orient2(const VecType &a, const VecType &b, const VecType &c)
    {
        return -cross(b - a, c - a);
    }

    constexpr float TOLERANCE = 0.0001f;
    inline bool vequal(const utils::Vector2f &a, const utils::Vector2f &b) { return dist(a, b) < TOLERANCE; }

    bool inline segmentsIntersect(utils::Vector2f a, utils::Vector2f b, utils::Vector2f c, utils::Vector2f d, utils::Vector2f &hit_point)
    {
        float oa = orient(c, d, a),
              ob = orient(c, d, b),
              oc = orient(a, b, c),
              od = orient(a, b, d);
        // Proper intersection exists iff opposite signs
        bool ab_cond = strictly_less(oa * ob, 0); // || approx_equal_zero(oa) || approx_equal_zero(ob);
        bool cd_cond = strictly_less(oc * od, 0); // || approx_equal_zero(oc) || approx_equal_zero(od);
        if (ab_cond && cd_cond)
        {
            hit_point = (a * ob - b * oa) / (ob - oa);
            assert(!std::isnan(hit_point.x) && !std::isnan(hit_point.y));
            return true;
        }
        return false;
    }

    template <class VecType>
    bool inline segmentsIntersect(const VecType &a, const VecType &b, const VecType &c, const VecType &d)
    {

        float oa = orient(c, d, a),
              ob = orient(c, d, b),
              oc = orient(a, b, c),
              od = orient(a, b, d);
        // Proper intersection exists iff opposite signs
        bool ab_cond = strictly_less(oa * ob, 0); // || approx_equal_zero(oa) || approx_equal_zero(ob);
        bool cd_cond = strictly_less(oc * od, 0); // || approx_equal_zero(oc) || approx_equal_zero(od);
        return ab_cond && cd_cond;
    }

    template <class VecType>
    bool inline segmentsIntersectOrTouch(const VecType &a, const VecType &b, const VecType &c, const VecType &d)
    {
        float oa = orient(c, d, a),
              ob = orient(c, d, b),
              oc = orient(a, b, c),
              od = orient(a, b, d);
        // Proper intersection exists iff opposite signs
        bool ab_cond = strictly_less(oa * ob, 0) || approx_equal_zero(oa) || approx_equal_zero(ob);
        bool cd_cond = strictly_less(oc * od, 0) || approx_equal_zero(oc) || approx_equal_zero(od);
        return ab_cond && cd_cond;
    }

    template <class VecType>
    bool inline segmentsIntersectOrTouch(const VecType &a, const VecType &b, const VecType &c, const VecType &d, utils::Vector2f &hit_point)
    {
        float oa = orient(c, d, a),
              ob = orient(c, d, b),
              oc = orient(a, b, c),
              od = orient(a, b, d);
        // Proper intersection exists iff opposite signs
        bool ab_cond = strictly_less(oa * ob, 0) || approx_equal_zero(oa) || approx_equal_zero(ob);
        bool cd_cond = strictly_less(oc * od, 0) || approx_equal_zero(oc) || approx_equal_zero(od);
        if (ab_cond && cd_cond)
        {
            hit_point = (a * ob - b * oa) / (ob - oa);
            assert(!std::isnan(hit_point.x) && !std::isnan(hit_point.y));
            return true;
        }
        return false;
    }

    //! ANGLES AND DIRECTIONS
    const auto to_radains = std::numbers::pi_v<float> / 180.f;
    const auto to_degrees = 180.f / std::numbers::pi_v<float>;
    inline utils::Vector2f angle2dir(float angle)
    {
        return {std::cos(angle * to_radains), std::sin(angle * to_radains)};
    }
    inline float dir2angle(const utils::Vector2f& dir)
    {
        return to_degrees * std::atan2(dir.y, dir.x);
    }

    inline float angleBetween(const utils::Vector2f& v1, const utils::Vector2f& v2)
    {
        return to_degrees * std::atan2(cross(v1, v2), dot(v1, v2));
    }

    inline void truncate(utils::Vector2f &vec, float max_value)
    {
        auto speed2 = utils::norm2(vec);
        if (speed2 > max_value*max_value)
        {
            vec *= max_value / std::sqrt(speed2);
        }
    }

}

inline utils::Vector2f asFloat(const utils::Vector2i &r) { return static_cast<utils::Vector2f>(r); }
