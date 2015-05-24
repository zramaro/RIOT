/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"

#include "atomic.h"

#include "tests-core.h"

static void test_atomic_set_return_null_null(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_one_null(void)
{
    unsigned int res = 1;

    TEST_ASSERT_EQUAL_INT(1, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_null_one(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, 1));
    TEST_ASSERT_EQUAL_INT(1, res);
}

static void test_atomic_set_return_limit_null(void)
{
    unsigned int res = UINT_MAX;

    TEST_ASSERT_EQUAL_INT(UINT_MAX, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_null_limit(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, UINT_MAX));
    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_atomic_set_return_null_random(void)
{
    unsigned int res = 0;
    unsigned int r = 45;    /* XXX: decided by fair dice-roll ;-) */

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, r));
    TEST_ASSERT_EQUAL_INT(r, res);
}

/* Test atomic_set_to_one on a variable set to 0 */
static void test_atomic_set_to_one_zero(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(1, atomic_set_to_one(&res));
    TEST_ASSERT_EQUAL_INT(1, ATOMIC_VALUE(res));
}

/* Test atomic_set_to_one on a variable set to 1 */
static void test_atomic_set_to_one_one(void)
{
    atomic_int_t res = ATOMIC_INIT(1);

    TEST_ASSERT_EQUAL_INT(0, atomic_set_to_one(&res));
    TEST_ASSERT_EQUAL_INT(1, ATOMIC_VALUE(res));
}

/* Test atomic_set_to_one twice */
static void test_atomic_set_to_one_twice(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(1, atomic_set_to_one(&res));
    TEST_ASSERT_EQUAL_INT(1, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(0, atomic_set_to_one(&res));
    TEST_ASSERT_EQUAL_INT(1, ATOMIC_VALUE(res));
}

/* Test atomic_set_to_zero on a variable set to 0 */
static void test_atomic_set_to_zero_zero(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(0, atomic_set_to_zero(&res));
    TEST_ASSERT_EQUAL_INT(0, ATOMIC_VALUE(res));
}

/* Test atomic_set_to_zero on a variable set to 1 */
static void test_atomic_set_to_zero_one(void)
{
    atomic_int_t res = ATOMIC_INIT(1);

    TEST_ASSERT_EQUAL_INT(1, atomic_set_to_zero(&res));
    TEST_ASSERT_EQUAL_INT(0, ATOMIC_VALUE(res));
}

/* Test atomic_set_to_zero twice */
static void test_atomic_set_to_zero_twice(void)
{
    atomic_int_t res = ATOMIC_INIT(1);

    TEST_ASSERT_EQUAL_INT(1, atomic_set_to_zero(&res));
    TEST_ASSERT_EQUAL_INT(0, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(0, atomic_set_to_zero(&res));
    TEST_ASSERT_EQUAL_INT(0, ATOMIC_VALUE(res));
}

/* Test atomic_inc */
static void test_atomic_inc_positive(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(0, atomic_inc(&res));
    TEST_ASSERT_EQUAL_INT(1, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(1, atomic_inc(&res));
    TEST_ASSERT_EQUAL_INT(2, ATOMIC_VALUE(res));
    ATOMIC_VALUE(res) = 0;
    for (int i = 0; i < 512; ++i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_inc(&res));
        TEST_ASSERT_EQUAL_INT(i + 1, ATOMIC_VALUE(res));
    }
}

static void test_atomic_inc_negative(void)
{
    atomic_int_t res = ATOMIC_INIT(-99);

    for (int i = -99; i < 123; ++i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_inc(&res));
        TEST_ASSERT_EQUAL_INT(i + 1, ATOMIC_VALUE(res));
    }
}

static void test_atomic_inc_rollover(void)
{
    atomic_int_t res = ATOMIC_INIT(INT_MAX - 30);

    for (int i = 0; i < 30; ++i) {
        TEST_ASSERT_EQUAL_INT(INT_MAX - 30 + i, atomic_inc(&res));
        TEST_ASSERT_EQUAL_INT(INT_MAX - 30 + i + 1, ATOMIC_VALUE(res));
    }
    TEST_ASSERT_EQUAL_INT(INT_MAX, atomic_inc(&res));
    TEST_ASSERT_EQUAL_INT(INT_MIN, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(INT_MIN, atomic_inc(&res));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 1, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 1, atomic_inc(&res));
    TEST_ASSERT_EQUAL_INT(INT_MIN + 2, ATOMIC_VALUE(res));
}

/* Test atomic_dec */
static void test_atomic_dec_negative(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(0, atomic_dec(&res));
    TEST_ASSERT_EQUAL_INT(-1, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(-1, atomic_dec(&res));
    TEST_ASSERT_EQUAL_INT(-2, ATOMIC_VALUE(res));
    ATOMIC_VALUE(res) = 0;
    for (int i = 0; i < 512; ++i) {
        TEST_ASSERT_EQUAL_INT(-i, atomic_dec(&res));
        TEST_ASSERT_EQUAL_INT(-i - 1, ATOMIC_VALUE(res));
    }
}

static void test_atomic_dec_positive(void)
{
    atomic_int_t res = ATOMIC_INIT(99);

    for (int i = 99; i < -123; --i) {
        TEST_ASSERT_EQUAL_INT(i, atomic_dec(&res));
        TEST_ASSERT_EQUAL_INT(i - 1, ATOMIC_VALUE(res));
    }
}

static void test_atomic_dec_rollover(void)
{
    atomic_int_t res = ATOMIC_INIT(INT_MIN + 30);

    for (int i = 0; i < 30; ++i) {
        TEST_ASSERT_EQUAL_INT(INT_MIN + 30 - i, atomic_dec(&res));
        TEST_ASSERT_EQUAL_INT(INT_MIN + 30 - i - 1, ATOMIC_VALUE(res));
    }
    TEST_ASSERT_EQUAL_INT(INT_MIN, atomic_dec(&res));
    TEST_ASSERT_EQUAL_INT(INT_MAX, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(INT_MAX, atomic_dec(&res));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 1, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 1, atomic_dec(&res));
    TEST_ASSERT_EQUAL_INT(INT_MAX - 2, ATOMIC_VALUE(res));
}

/* Test atomic_cas with a correct old value */
static void test_atomic_cas_same(void)
{
    atomic_int_t res = ATOMIC_INIT(0);

    TEST_ASSERT_EQUAL_INT(1, atomic_cas(&res, 0, 1234567));
    TEST_ASSERT_EQUAL_INT(1234567, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(1, atomic_cas(&res, 1234567, -987654321));
    TEST_ASSERT_EQUAL_INT(-987654321, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(1, atomic_cas(&res, -987654321, -987654321));
    TEST_ASSERT_EQUAL_INT(-987654321, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(1, atomic_cas(&res, -987654321, 0));
    TEST_ASSERT_EQUAL_INT(0, ATOMIC_VALUE(res));
}

/* Test atomic_cas with a non-matching old value */
static void test_atomic_cas_diff(void)
{
    atomic_int_t res = ATOMIC_INIT(32767);

    TEST_ASSERT_EQUAL_INT(0, atomic_cas(&res, 65535, 12345));
    TEST_ASSERT_EQUAL_INT(32767, ATOMIC_VALUE(res));
    ATOMIC_VALUE(res) = -12345687;
    TEST_ASSERT_EQUAL_INT(0, atomic_cas(&res, 12345687, 123456789));
    TEST_ASSERT_EQUAL_INT(-12345687, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(0, atomic_cas(&res, 12345687, 12345687));
    TEST_ASSERT_EQUAL_INT(-12345687, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(0, atomic_cas(&res, 12345687, -12345687));
    TEST_ASSERT_EQUAL_INT(-12345687, ATOMIC_VALUE(res));
}

/* Test ATOMIC_VALUE */
static void test_atomic_value(void)
{
    atomic_int_t res = ATOMIC_INIT(12345);
    atomic_int_t *ptr = &res;

    TEST_ASSERT_EQUAL_INT(12345, ATOMIC_VALUE(res));
    ATOMIC_VALUE(res) = 54332;
    TEST_ASSERT_EQUAL_INT(54332, ATOMIC_VALUE(res));
    TEST_ASSERT_EQUAL_INT(54332, res.value);
    res.value = 1232342131;
    TEST_ASSERT_EQUAL_INT(ATOMIC_VALUE(res), res.value);
    TEST_ASSERT_EQUAL_INT(ATOMIC_VALUE(*ptr), res.value);
}

/* ATOMIC_INIT is implicitly tested by the other tests */

Test *tests_core_atomic_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_atomic_set_return_null_null),
        new_TestFixture(test_atomic_set_return_one_null),
        new_TestFixture(test_atomic_set_return_null_one),
        new_TestFixture(test_atomic_set_return_limit_null),
        new_TestFixture(test_atomic_set_return_null_limit),
        new_TestFixture(test_atomic_set_return_null_random),
        new_TestFixture(test_atomic_set_to_one_one),
        new_TestFixture(test_atomic_set_to_one_zero),
        new_TestFixture(test_atomic_set_to_one_twice),
        new_TestFixture(test_atomic_set_to_zero_one),
        new_TestFixture(test_atomic_set_to_zero_zero),
        new_TestFixture(test_atomic_set_to_zero_twice),
        new_TestFixture(test_atomic_inc_positive),
        new_TestFixture(test_atomic_inc_negative),
        new_TestFixture(test_atomic_inc_rollover),
        new_TestFixture(test_atomic_dec_positive),
        new_TestFixture(test_atomic_dec_negative),
        new_TestFixture(test_atomic_dec_rollover),
        new_TestFixture(test_atomic_cas_same),
        new_TestFixture(test_atomic_cas_diff),
        new_TestFixture(test_atomic_value),
    };

    EMB_UNIT_TESTCALLER(core_atomic_tests, NULL, NULL,
                        fixtures);

    return (Test *)&core_atomic_tests;
}
