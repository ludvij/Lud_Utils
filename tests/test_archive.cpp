#include "ludutils/lud_archive.hpp"

#include <catch2/catch_all.hpp>

#include <random>
#include <ranges>

TEST_CASE("Compression from view", "[archive][compression]") 
{
	SECTION("Singular chunk")
	{
		std::vector<uint8_t> test_input = {'t', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't'};
		auto deflated = Lud::Compress(test_input);
		auto inflated = Lud::Uncompress(deflated);
		
		REQUIRE(inflated == test_input);
	}

	SECTION("Multiple Chunks")
	{
		std::vector<uint8_t> junk(16384 * 128);

		std::mt19937 mt(2051920);
		std::uniform_int_distribution<uint8_t> dist;
		for(auto& elem : junk)
		{
			elem = dist(mt);
		}

		auto deflated = Lud::Compress(junk);
		auto inflated = Lud::Uncompress(deflated);

		REQUIRE(junk == inflated);
	}
}