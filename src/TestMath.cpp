#include "gekko_math.h"

#include <cassert>
#include <stdexcept>
#include <cmath>
#include <iostream>

using namespace Gekko::Math;

struct TestMath {
    // Helper for comparing floats within a tolerance.
    static bool AlmostEqual(float a, float b, float epsilon = 1e-4f) {
        return std::fabs(a - b) < epsilon;
    }

    static void TestUnitArithmetic() {
        // Test From and AsFloat conversion using tolerance for float comparisons.
        {
            Unit u = Unit::From(5);
            // Although u.AsFloat() should be 5.0 exactly, we allow a small tolerance.
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

        // Test division with rounding (e.g. 5 / 2 should be close to 2.5)
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
    }

    static void TestVec3Arithmetic() {
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

        // Test compound multiplication assignments
        {
            Vec3 v(Unit::From(1), Unit::From(2), Unit::From(3));
            v *= Vec3(Unit::From(2), Unit::From(2), Unit::From(2));
            Vec3 expected(Unit::From(2), Unit::From(4), Unit::From(6));
            assert(v == expected);

            v *= Unit::From(2);
            expected = Vec3(Unit::From(4), Unit::From(8), Unit::From(12));
            assert(v == expected);
        }

        // Test element-wise division and scalar division
        {
            Vec3 v1(Unit::From(4), Unit::From(6), Unit::From(8));
            Vec3 v2(Unit::From(2), Unit::From(2), Unit::From(2));
            Vec3 div = v1 / v2;
            Vec3 expected(Unit::From(2), Unit::From(3), Unit::From(4));
            assert(div == expected);

            Vec3 div2 = v1 / Unit::From(2);
            assert(div2 == expected);
        }

        // Test compound division assignments
        {
            Vec3 v(Unit::From(8), Unit::From(10), Unit::From(12));
            v /= Vec3(Unit::From(2), Unit::From(2), Unit::From(2));
            Vec3 expected(Unit::From(4), Unit::From(5), Unit::From(6));
            assert(v == expected);

            v /= Unit::From(2);
            // After dividing, components should be (2, 2.5, 3)
            assert(AlmostEqual(v.x.AsFloat(), 2.0f));
            assert(AlmostEqual(v.y.AsFloat(), 2.5f));
            assert(AlmostEqual(v.z.AsFloat(), 3.0f));
        }

        // Test dot product
        {
            Vec3 v1(Unit::From(1), Unit::From(2), Unit::From(3));
            Vec3 v2(Unit::From(4), Unit::From(5), Unit::From(6));
            Unit dot = v1.Dot(v2);
            // Expected dot: 1*4 + 2*5 + 3*6 = 32
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
        std::cout << "All tests passed.\n";
    }
};

static TestMath __test_math;
