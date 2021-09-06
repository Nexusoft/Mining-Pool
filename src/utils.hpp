#ifndef NEXUSPOOL_UTILS_HPP
#define NEXUSPOOL_UTILS_HPP

#include <cstdint>
#include <chrono>
#include <array>
#include <cmath>
#include <optional>
#include "config/types.hpp"

namespace nexuspool
{
class Hashrate_helper
{
public:
	Hashrate_helper(config::Mining_mode mining_mode)
		: m_mining_mode{ mining_mode }
		, m_t1{ std::chrono::steady_clock::now() }
		, m_t2{ std::chrono::steady_clock::now() }
		, m_share_timepoints{}
		, m_current_timepoint_index{ 0U }
		, m_hashrate{ 0 }
	{}

	void add_share(std::uint32_t pool_nbits)
	{
		if (m_t1 >= m_t2)
		{
			m_t1 = std::chrono::steady_clock::now();
		}
		else
		{
			m_t2 = std::chrono::steady_clock::now();
			m_share_timepoints[m_current_timepoint_index] = std::chrono::duration_cast<std::chrono::milliseconds>(m_t2 - m_t1);
			m_current_timepoint_index++;

			if (m_current_timepoint_index >= m_share_timepoints.size())
			{
				m_current_timepoint_index = 0U;
				calculate_hashrate(pool_nbits);
			}
		}
	}

	double get_hashrate() const
	{
		return m_hashrate;
	}

private:

	void calculate_hashrate(std::uint32_t pool_nbits)
	{
		std::chrono::milliseconds total_time{ 0 };
		std::chrono::milliseconds average_time{ 0 };
		for (auto& time : m_share_timepoints)
		{
			total_time += time;
		}
		average_time = total_time / m_share_timepoints.size();

		if (m_mining_mode == config::Mining_mode::HASH)
		{
			m_hashrate = (pool_nbits * std::pow(2, 34)) / average_time.count();
		}
	}

	config::Mining_mode m_mining_mode;
	std::chrono::steady_clock::time_point m_t1, m_t2;
	std::array<std::chrono::milliseconds, 5U> m_share_timepoints;
	std::size_t m_current_timepoint_index;
	double m_hashrate;

};

}

#endif