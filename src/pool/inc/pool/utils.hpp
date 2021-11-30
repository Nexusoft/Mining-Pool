#ifndef NEXUSPOOL_POOL_UTILS_HPP
#define NEXUSPOOL_POOL_UTILS_HPP

#include <cstdint>
#include <chrono>
#include <array>
#include "common/types.hpp"
#include "LLP/utils.hpp"

namespace nexuspool
{
class Hashrate_helper
{
public:
	Hashrate_helper(common::Mining_mode mining_mode)
		: m_mining_mode{ mining_mode }
		, m_t1{ std::chrono::steady_clock::now() }
		, m_t2{ std::chrono::steady_clock::now() }
		, m_share_timepoints{}
		, m_current_timepoint_index{ 0U }
		, m_hashrate{ 0 }
	{}

	void add_share()
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
				std::chrono::milliseconds total_time{ 0 };
				for (auto& time : m_share_timepoints)
				{
					total_time += time;
				}
				m_average_time = total_time / m_share_timepoints.size();
			}
		}
	}

	double get_hashrate() const
	{
		return m_hashrate;
	}

private:

	void calculate_hashrate(std::uint32_t pool_nbits, std::uint32_t network_nbits, double prime_shares_to_blocks_ratio)
	{
		if (m_average_time.count() == 0)
		{
			return;
		}


		int const channel = m_mining_mode == common::Mining_mode::PRIME ? 1 : 2;
		m_hashrate = get_miner_hash_rate(pool_nbits, network_nbits, channel, m_average_time.count() / 1000, prime_shares_to_blocks_ratio);
	}

	common::Mining_mode m_mining_mode;
	std::chrono::steady_clock::time_point m_t1, m_t2;
	std::array<std::chrono::milliseconds, 5U> m_share_timepoints;
	std::chrono::milliseconds m_average_time{ 0 };
	std::size_t m_current_timepoint_index;
	double m_hashrate;

};

}

#endif