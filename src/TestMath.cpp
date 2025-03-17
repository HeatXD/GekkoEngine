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
            Unit u = 5;
            assert(AlmostEqual(u.AsFloat(), 5.0f));
        }

        // Test addition and subtraction
        {
            Unit a = 3;
            Unit b = 4;
            Unit c = a + b;
            assert(c == 7);

            a = 10;
            b = 4;
            c = a - b;
            assert(c == 6);
        }

        // Test multiplication and division
        {
            Unit a = 3;
            Unit b = 2;
            Unit c = a * b;
            assert(c == 6);

            a = 6;
            b = 2;
            c = a / b;
            assert(c == 3);
        }

        // Test division with rounding
        {
            Unit a = 5;
            Unit b = 2;
            Unit c = a / b;
            assert(AlmostEqual(c.AsFloat(), 2.5f));
        }

        // Test division by zero throws an exception
        {
            bool exceptionThrown = false;
            try {
                Unit a = 5;
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
            Unit a = 1;
            a += 2;
            assert(a == 3);

            a -= 1;
            assert(a == 2);

            a *= 3;
            assert(a == 6);

            a /= 2;
            assert(AlmostEqual(a.AsFloat(), 3.0f));
        }

        // Square root tests
        {
            // Perfect squares
            std::cout << "sqrt(4): " << Unit::SqrtNewton(4).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(4).AsFloat(), 2.0f));
            std::cout << "sqrt(9): " << Unit::SqrtNewton(9).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(9).AsFloat(), 3.0f));
            std::cout << "sqrt(16): " << Unit::SqrtNewton(16).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(16).AsFloat(), 4.0f));
            std::cout << "sqrt(25): " << Unit::SqrtNewton(25).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(25).AsFloat(), 5.0f));

            // Non-perfect squares (should be close to true sqrt)
            std::cout << "sqrt(2): " << Unit::SqrtNewton(2).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(2).AsFloat(), std::sqrt(2.0f)));
            std::cout << "sqrt(3): " << Unit::SqrtNewton(3).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(3).AsFloat(), std::sqrt(3.0f)));
            std::cout << "sqrt(5): " << Unit::SqrtNewton(5).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(5).AsFloat(), std::sqrt(5.0f)));
            std::cout << "sqrt(7): " << Unit::SqrtNewton(7).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(7).AsFloat(), std::sqrt(7.0f)));

            // Harder cases
            std::cout << "sqrt(10): " << Unit::SqrtNewton(10).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(10).AsFloat(), std::sqrt(10.0f)));
            std::cout << "sqrt(50): " << Unit::SqrtNewton(50).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(50).AsFloat(), std::sqrt(50.0f)));
            std::cout << "sqrt(123): " << Unit::SqrtNewton(123).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(123).AsFloat(), std::sqrt(123.0f)));

            // Large numbers
            std::cout << "sqrt(100): " << Unit::SqrtNewton(100).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(100).AsFloat(), 10.0f));
            std::cout << "sqrt(10000): " << Unit::SqrtNewton(10000).AsFloat() << "\n";
            assert(AlmostEqual(Unit::SqrtNewton(10000).AsFloat(), 100.0f));

            // Edge cases
            assert(AlmostEqual(Unit::SqrtNewton(0).AsFloat(), 0.0f));
            assert(AlmostEqual(Unit::SqrtNewton(1).AsFloat(), 1.0f));

            // Negative input should throw an exception
            bool exceptionThrown = false;
            try {
                Unit neg = -1;
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
            Vec3 v(1, 2, 3);
            Vec3 expected(1, 2, 3);
            assert(v == expected);
        }

        // Test vector addition and subtraction
        {
            Vec3 v1(1, 2, 3);
            Vec3 v2(4, 5, 6);
            Vec3 sum = v1 + v2;
            Vec3 expected(5, 7, 9);
            assert(sum == expected);

            Vec3 diff = expected - v2;
            assert(diff == v1);
        }

        // Test compound vector addition and subtraction
        {
            Vec3 v(1, 2, 3);
            v += Vec3(1, 1, 1);
            assert(v == Vec3(2, 3, 4));
            v -= Vec3(1, 1, 1);
            assert(v == Vec3(1, 2, 3));
        }

        // Test scalar addition and subtraction
        {
            Vec3 v(1, 2, 3);
            Unit delta = 2;
            Vec3 vPlus = v + delta;
            Vec3 expected(3, 4, 5);
            assert(vPlus == expected);

            Vec3 vMinus = vPlus - delta;
            assert(vMinus == v);
        }

        // Test element-wise multiplication and scalar multiplication
        {
            Vec3 v1(1, 2, 3);
            Vec3 v2(4, 5, 6);
            Vec3 prod = v1 * v2;
            Vec3 expected(4, 10, 18);
            assert(prod == expected);

            Unit scalar = 3;
            prod = v1 * scalar;
            expected = Vec3(3, 6, 9);
            assert(prod == expected);
        }

        // Test dot product
        {
            Vec3 v1(1, 2, 3);
            Vec3 v2(4, 5, 6);
            Unit dot = v1.Dot(v2);
            assert(dot == 32);
        }

        // Test AsFloat conversion for visualization
        {
            Vec3 v(3, 4, 5);
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
