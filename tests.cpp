#include <gtest/gtest.h>
#include <string>
#include "optional/bmstu_optional.h"

TEST(Optional, dummy_test_1) {
    int boeing = 777;
    bmstu::optional<int> opt;
    ASSERT_FALSE(opt.has_any_val());
    opt = boeing;
    ASSERT_TRUE(opt.has_any_val());
    ASSERT_EQ(opt.value(), 777);
}

TEST(Optional, dummy_test_2) {
    std::string flight = "SU-274";
    bmstu::optional<std::string> opt;
    ASSERT_FALSE(opt.has_any_val());
    opt = flight;
    ASSERT_TRUE(opt.has_any_val());
    ASSERT_STREQ(opt.value().c_str(), "SU-274");
}

struct OVS {
    OVS() noexcept {
        ++def_constr;
    }

    OVS(const OVS &v) noexcept {
        v.update();
        ++copy_constr;
    }

    OVS(OVS &&v) noexcept {
        update(std::move(v));
        ++move_constr;
    }

    OVS &operator=(const OVS &other) noexcept {
        if (this != &other) {
            other.update();
            ++copy_assign;
        }
        return *this;
    }

    OVS &operator=(OVS &&other) noexcept {
        update(std::move(other));
        ++move_assign;
        return *this;
    }

    ~OVS() {
        ++destruct;
    }

    void update() &{
        ++lvalue_call_count;
    }

    void update() const &{
        ++c_lvalue_call_count;
    }

    void update(OVS &&) {
        ++rvalue_call_count;
    }

    static size_t real_time_counter() {
        return def_constr + copy_constr + move_constr - destruct;
    }

    static void reset() {
        size_t def_constr = 0;
        size_t copy_constr = 0;
        size_t move_constr = 0;
        size_t copy_assign = 0;
        size_t move_assign = 0;
        size_t destruct = 0;
        size_t lvalue_call_count = 0;
        size_t rvalue_call_count = 0;
        size_t c_lvalue_call_count = 0;
    }

    static void printing() {
        std::cout << " " << std::endl;
        std::cout << "def_constr = " << def_constr << " " << std::endl;
        std::cout << "copy_constr = " << copy_constr << " " << std::endl;
        std::cout << "move_constr = " << move_constr << " " << std::endl;
        std::cout << "copy_assign = " << copy_assign << " " << std::endl;
        std::cout << "move_assign = " << move_assign << " " << std::endl;
        std::cout << "destruct = " << destruct << " " << std::endl;
        std::cout << "lvalue_call_count = " << lvalue_call_count << " " << std::endl;
        std::cout << "rvalue_call_count = " << rvalue_call_count << " " << std::endl;
        std::cout << "c_lvalue_call_count = " << c_lvalue_call_count << " " << std::endl;
    }

    inline static size_t def_constr = 0;
    inline static size_t copy_constr = 0;
    inline static size_t move_constr = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t destruct = 0;
    inline static size_t lvalue_call_count = 0;
    inline static size_t rvalue_call_count = 0;
    inline static size_t c_lvalue_call_count = 0;
};

TEST(Optional, test_3_empty) {
    OVS::reset();
    bmstu::optional<OVS> svo;
    ASSERT_FALSE(svo.has_any_val());
    OVS::printing();
    ASSERT_EQ(OVS::real_time_counter(), 0);
}

TEST(Optional, test_4_def_constr) {
    OVS::reset();
    OVS::printing();
    ASSERT_EQ(OVS::real_time_counter(), 0);
    {
        OVS svo;
        OVS::printing();
        ASSERT_EQ(OVS::def_constr, 1);
        ASSERT_EQ(OVS::copy_constr, 0);
        ASSERT_EQ(OVS::move_constr, 0);
        ASSERT_EQ(OVS::copy_assign, 0);
        ASSERT_EQ(OVS::move_assign, 0);
        ASSERT_EQ(OVS::real_time_counter(), 1);
    }
    ASSERT_EQ(OVS::real_time_counter(), 0);
}

TEST(Optional, test_5_copy_constr) {
    OVS::reset();
    ASSERT_EQ(OVS::real_time_counter(), 0);
    {
        OVS svo;
        bmstu::optional<OVS> opt(svo);
        ASSERT_TRUE(opt.has_any_val());
        ASSERT_EQ(OVS::real_time_counter(), 2);
        OVS::printing();
        ASSERT_EQ(OVS::def_constr, 1);
        ASSERT_EQ(OVS::copy_constr, 1);
        ASSERT_EQ(OVS::move_constr, 0);
        ASSERT_EQ(OVS::copy_assign, 0);
        ASSERT_EQ(OVS::move_assign, 0);
        ASSERT_EQ(OVS::lvalue_call_count, 0);
        ASSERT_EQ(OVS::c_lvalue_call_count, 1);
        ASSERT_EQ(OVS::rvalue_call_count, 0);
    }
    ASSERT_TRUE(OVS::def_constr == 1 && OVS::copy_constr == 1 && OVS::destruct == 2);
    ASSERT_EQ(OVS::real_time_counter(), 0);
}

TEST(Optional, test_6_move_ctor) {
    OVS::reset();
    ASSERT_EQ(OVS::real_time_counter(), 0);
    {
        OVS svo;
        bmstu::optional<OVS> opt(std::move(svo));
        ASSERT_TRUE(opt.has_any_val());
        ASSERT_EQ(OVS::real_time_counter(), 2);
        OVS::printing();
        ASSERT_EQ(OVS::def_constr, 1);
        ASSERT_EQ(OVS::copy_constr, 0);
        ASSERT_EQ(OVS::move_constr, 1);
        ASSERT_EQ(OVS::copy_assign, 0);
        ASSERT_EQ(OVS::move_assign, 0);
        ASSERT_EQ(OVS::lvalue_call_count, 0);
        ASSERT_EQ(OVS::c_lvalue_call_count, 0);
        ASSERT_EQ(OVS::rvalue_call_count, 1);
    }
    OVS::printing();
    ASSERT_TRUE(OVS::def_constr == 1 && OVS::move_constr == 1 && OVS::destruct == 2);
    ASSERT_EQ(OVS::real_time_counter(), 0);
}
