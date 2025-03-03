#include "gekko_math.h"

#include <cassert>
#include <stdexcept>
#include <cmath>
#include <iostream>

using namespace Gekko::Math;

struct TestMath {
    // Helper for comparing floats within a tolerance.
    bool AlmostEqual(float a, float b, float epsilon = 1e-4f) {
        return std::fabs(a - b) < epsilon;
    }

    void TestUnitArithmetic() {
        // Test From and AsFloat conversion using tolerance for float comparisons.
        {
            Unit u = Unit::From(5);
            assert(AlmostEqual(u.AsFloat(), 5.0f));
        }

        // Test addition and subtraction
        {
            Unit a = Unit::From(3);
            Unit b = Unit::From(4);
            Unit c = a + b;
            assert(c == Unit::From(7));

            a = Unit::From(10);
            b = Unit::From(4);
            c = a - b;
            assert(c == Unit::From(6));
        }

        // Test multiplication and division
        {
            Unit a = Unit::From(3);
            Unit b = Unit::From(2);
            Unit c = a * b;
            assert(c == Unit::From(6));

            a = Unit::From(6);
            b = Unit::From(2);
            c = a / b;
            assert(c == Unit::From(3));
        }

        // Test division with rounding
        {
            Unit a = Unit::From(5);
            Unit b = Unit::From(2);
            Unit c = a / b;
            assert(AlmostEqual(c.AsFloat(), 2.5f));
        }

        // Test division by zero throws an exception
        {
            bool exceptionThrown = false;
            try {
                Unit a = Unit::From(5);
                Unit b(0);
                Unit c = a / b;
            }
            catch (const std::runtime_error&) {
                exceptionThrown = true;
            }
            assert(exceptionThrown);
        }

        // Test compound assignment operators
        {
            Unit a = Unit::From(1);
            a += Unit::From(2);
            assert(a == Unit::From(3));

            a -= Unit::From(1);
            assert(a == Unit::From(2));

            a *= Unit::From(3);
            assert(a == Unit::From(6));

            a /= Unit::From(2);
            assert(AlmostEqual(a.AsFloat(), 3.0f));
        }

        // Square root tests
        {
            // Perfect squares
            std::cout << "sqrt(4): " << Unit::SqrtNewton(Unit::From(4)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(4)).AsFloat(), 2.0f));
            std::cout << "sqrt(9): " << Unit::SqrtNewton(Unit::From(9)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(9)).AsFloat(), 3.0f));
            std::cout << "sqrt(16): " << Unit::SqrtNewton(Unit::From(16)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(16)).AsFloat(), 4.0f));
            std::cout << "sqrt(25): " << Unit::SqrtNewton(Unit::From(25)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(25)).AsFloat(), 5.0f));

            // Non-perfect squares (should be close to true sqrt)
            std::cout << "sqrt(2): " << Unit::SqrtNewton(Unit::From(2)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(2)).AsFloat(), std::sqrt(2.0f), 1e-3f));
            std::cout << "sqrt(3): " << Unit::SqrtNewton(Unit::From(3)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(3)).AsFloat(), std::sqrt(3.0f), 1e-3f));
            std::cout << "sqrt(5): " << Unit::SqrtNewton(Unit::From(5)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(5)).AsFloat(), std::sqrt(5.0f), 1e-3f));
            std::cout << "sqrt(7): " << Unit::SqrtNewton(Unit::From(7)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(7)).AsFloat(), std::sqrt(7.0f), 1e-3f));

            // Harder cases
            std::cout << "sqrt(10): " << Unit::SqrtNewton(Unit::From(10)).AsFloat() << "\n";
            std::cout << "sqrt(50): " << Unit::SqrtNewton(Unit::From(50)).AsFloat() << "\n";
            std::cout << "sqrt(123): " << Unit::SqrtNewton(Unit::From(123)).AsFloat() << "\n";

            // Large numbers
            std::cout << "sqrt(100): " << Unit::SqrtNewton(Unit::From(100)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(100)).AsFloat(), 10.0f, 1e-3f));
            std::cout << "sqrt(10000): " << Unit::SqrtNewton(Unit::From(10000)).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(10000)).AsFloat(), 100.0f, 1e-3f));

            // Edge cases
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(0)).AsFloat(), 0.0f));
            assert(AlmostEqual(Unit::SqrtNewton(Unit::From(1)).AsFloat(), 1.0f));

            // Negative input should throw an exception
            bool exceptionThrown = false;
            try {
                Unit neg = Unit::From(-1);
                Unit::SqrtNewton(neg);
            }
            catch (const std::runtime_error&) {
                exceptionThrown = true;
            }
            assert(exceptionThrown);
        }
    }

    void TestVec3Arithmetic() {
        // Test constructor and equality
        {
            Vec3 v(Unit::From(1), Unit::From(2), Unit::From(3));
            Vec3 expected(Unit::From(1), Unit::From(2), Unit::From(3));
            assert(v == expected);
        }

        // Test vector addition and subtraction
        {
            Vec3 v1(Unit::From(1), Unit::From(2), Unit::From(3));
            Vec3 v2(Unit::From(4), Unit::From(5), Unit::From(6));
            Vec3 sum = v1 + v2;
            Vec3 expected(Unit::From(5), Unit::From(7), Unit::From(9));
            assert(sum == expected);

            Vec3 diff = expected - v2;
            assert(diff == v1);
        }

        // Test compound vector addition and subtraction
        {
            Vec3 v(Unit::From(1), Unit::From(2), Unit::From(3));
            v += Vec3(Unit::From(1), Unit::From(1), Unit::From(1));
            assert(v == Vec3(Unit::From(2), Unit::From(3), Unit::From(4)));
            v -= Vec3(Unit::From(1), Unit::From(1), Unit::From(1));
            assert(v == Vec3(Unit::From(1), Unit::From(2), Unit::From(3)));
        }

        // Test scalar addition and subtraction
        {
            Vec3 v(Unit::From(1), Unit::From(2), Unit::From(3));
            Unit delta = Unit::From(2);
            Vec3 vPlus = v + delta;
            Vec3 expected(Unit::From(3), Unit::From(4), Unit::From(5));
            assert(vPlus == expected);

            Vec3 vMinus = vPlus - delta;
            assert(vMinus == v);
        }

        // Test element-wise multiplication and scalar multiplication
        {
            Vec3 v1(Unit::From(1), Unit::From(2), Unit::From(3));
            Vec3 v2(Unit::From(4), Unit::From(5), Unit::From(6));
            Vec3 prod = v1 * v2;
            Vec3 expected(Unit::From(4), Unit::From(10), Unit::From(18));
            assert(prod == expected);

            Unit scalar = Unit::From(3);
            prod = v1 * scalar;
            expected = Vec3(Unit::From(3), Unit::From(6), Unit::From(9));
            assert(prod == expected);
        }

        // Test dot product
        {
            Vec3 v1(Unit::From(1), Unit::From(2), Unit::From(3));
            Vec3 v2(Unit::From(4), Unit::From(5), Unit::From(6));
            Unit dot = v1.Dot(v2);
            assert(dot == Unit::From(32));
        }

        // Test AsFloat conversion for visualization
        {
            Vec3 v(Unit::From(3), Unit::From(4), Unit::From(5));
            Vec3F vf = v.AsFloat();
            assert(AlmostEqual(vf.x, 3.0f));
            assert(AlmostEqual(vf.y, 4.0f));
            assert(AlmostEqual(vf.z, 5.0f));
        }
    }

    TestMath() {
        TestUnitArithmetic();
        TestVec3Arithmetic();
        std::cout << "All math tests passed.\n";
    }
};

static TestMath __test_math;
