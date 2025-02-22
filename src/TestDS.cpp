#include <iostream>
#include <cstring>
#include <cassert>

#include "gekko_ds.h"

using namespace Gekko::DS;

struct TestDS {
    // Custom trivially copyable struct
    struct Point {
        int x;
        int y;
    };

    // Utility function for comparing two Point instances
    bool are_points_equal(const Point& p1, const Point& p2) {
        return std::memcmp(&p1, &p2, sizeof(Point)) == 0;
    }

    // ------------------------ Function Tests ------------------------
    void TestFunctionPointerInvocation() {
        Function<int(int, int)> add([](int a, int b) { return a + b; });
        assert(add && "Function object should be valid");
        assert(add(2, 3) == 5 && "Expected 5");
        std::cout << "Function Pointer Invocation Test PASSED\n";
    }

    void TestDefaultReturnValue() {
        Function<int(int, int)> empty_fn;
        assert(!empty_fn && "Empty function object should be invalid");
        assert(empty_fn(2, 3) == 0 && "Expected 0 as default return");
        std::cout << "Default Return Value Test PASSED\n";
    }

    void TestBoolConversion() {
        Function<void()> empty_fn;
        assert(!empty_fn && "Empty function object should be false");

        Function<void()> valid_fn([]() {});
        assert(valid_fn && "Valid function object should be true");
        std::cout << "Bool Conversion Test PASSED\n";
    }

    // ------------------------ Vec Tests ------------------------
    void TestVecPushBackAndAccess() {
        Vec<int> vec;
        vec.push_back(10);
        vec.push_back(20);

        assert(vec.size() == 2 && "Size should be 2");
        assert(vec[0] == 10 && "First element should be 10");
        assert(vec[1] == 20 && "Second element should be 20");

        std::cout << "Vec PushBack and Access Test PASSED\n";
    }

    void TestVecPopBack() {
        Vec<int> vec;
        vec.push_back(30);
        vec.pop_back();
        assert(vec.size() == 0 && "Size should be 0 after pop");
        std::cout << "Vec PopBack Test PASSED\n";
    }

    void TestVecDynamicResizing() {
        Vec<int> vec;
        for (int i = 0; i < 100; ++i) {
            vec.push_back(i);
        }

        assert(vec.size() == 100 && "Size should be 100");

        for (int i = 0; i < 100; ++i) {
            assert(vec[i] == i && "Element mismatch");
        }
        std::cout << "Vec Dynamic Resizing Test PASSED\n";
    }

    // ------------------------ SparseSet Tests ------------------------
    void TestSparseSetInsertAndGet() {
        SparseSet<int, Point> set;
        int id = set.insert({ 1, 2 });

        assert(set.contains(id) && "ID not found after insertion");
        assert(are_points_equal(set.get(id), Point{ 1, 2 }) && "Point mismatch");

        std::cout << "SparseSet Insert and Get Test PASSED\n";
    }

    void TestSparseSetEnableDisable() {
        SparseSet<int, Point> set;
        int id = set.insert({ 3, 4 });
        set.disable(id);
        assert(!set.is_enabled(id) && "ID should be disabled");

        set.enable(id);
        assert(set.is_enabled(id) && "ID should be enabled");

        std::cout << "SparseSet Enable/Disable Test PASSED\n";
    }

    void TestSparseSetRemove() {
        SparseSet<int, Point> set;
        int id = set.insert({ 5, 6 });
        set.remove(id);
        assert(!set.contains(id) && "ID should be removed");

        std::cout << "SparseSet Remove Test PASSED\n";
    }

    TestDS() {
        // Function Tests
        TestFunctionPointerInvocation();
        TestDefaultReturnValue();
        TestBoolConversion();

        // Vec Tests
        TestVecPushBackAndAccess();
        TestVecPopBack();
        TestVecDynamicResizing();

        // SparseSet Tests
        TestSparseSetInsertAndGet();
        TestSparseSetEnableDisable();
        TestSparseSetRemove();

        std::cout << "All datastructure tests passed.\n";
    }
};

static TestDS __test_ds;
