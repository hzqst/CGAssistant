#pragma once
#include "function_traits.hpp"
#define TEST_MAIN
#include "unit_test.hpp"

void test_func(int , int )
{

}

TEST_CASE(function_traits_on_normal_function)
{
	TEST_CHECK(function_traits<decltype(test_func)>::arity == 2);
}

