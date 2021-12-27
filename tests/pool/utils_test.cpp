#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "pool/utils.hpp"

using namespace ::nexuspool;
using namespace ::testing;

TEST(Utils_test, Hashrate_helper_test)
{
	Hashrate_helper hashrate_helper{ common::Mining_mode::PRIME };
	hashrate_helper.add_share();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	hashrate_helper.add_share();
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
	hashrate_helper.add_share();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	hashrate_helper.add_share();
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	hashrate_helper.add_share();

}