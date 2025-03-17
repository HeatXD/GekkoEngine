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
    }

    void TestDefaultReturnValue() {
        Function<int(int, int)> empty_fn;
        assert(!empty_fn && "Empty function object should be invalid");
        assert(empty_fn(2, 3) == 0 && "Expected 0 as default return");
    }

    void TestBoolConversion() {
        Function<void()> empty_fn;
        assert(!empty_fn && "Empty function object should be false");

        Function<void()> valid_fn([]() {});
        assert(valid_fn && "Valid function object should be true");
    }

    // ------------------------ Vec Tests ------------------------
    void TestVecPushBackAndAccess() {
        Vec<int> vec1;
        vec1.push_back(10);
        vec1.push_back(20);

        assert(vec1.size() == 2 && "Size should be 2");
        assert(vec1[0] == 10 && "First element should be 10");
        assert(vec1[1] == 20 && "Second element should be 20");

        Vec<int*> vec2;
        vec2.push_back(new int(10));
        vec2.push_back(new int(20));

        assert(vec2.size() == 2 && "Size should be 2");
        assert(*vec2[0] == 10 && "First ptr element should be 10");
        assert(*vec2[1] == 20 && "Second ptr element should be 20");

        vec2.pop_back();
        vec2.push_back(new int(30));

        assert(*vec2[1] == 30 && "Second ptr element should be 30");
    }

    void TestVecPopBack() {
        Vec<int> vec1;
        vec1.push_back(30);
        vec1.pop_back();
        assert(vec1.size() == 0 && "Size should be 0 after pop");

        Vec<int*> vec2;
        vec2.push_back(new int(30));
        vec2.pop_back();
        assert(vec2.size() == 0 && "Size should be 0 after pop");
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
    }

    // ------------------------ SparseSet Tests ------------------------
    void TestSparseSetInsertAndGet() {
        SparseSet<int, Point> set;
        int id = set.insert({ 1, 2 });

        assert(set.contains(id) && "ID not found after insertion");
        assert(are_points_equal(set.get(id), Point{ 1, 2 }) && "Point mismatch");
    }

    void TestSparseSetEnableDisable() {
        SparseSet<int, Point> set;
        int id = set.insert({ 3, 4 });
        set.disable(id);
        assert(!set.is_enabled(id) && "ID should be disabled");

        set.enable(id);
        assert(set.is_enabled(id) && "ID should be enabled");
    }

    void TestSparseSetRemove() {
        SparseSet<int, Point> set;
        int id = set.insert({ 5, 6 });
        set.remove(id);
        assert(!set.contains(id) && "ID should be removed");
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
