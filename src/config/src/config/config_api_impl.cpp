#include "config/config_api_impl.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace nexuspool
{
namespace config
{
Config_api_impl::Config_api_impl()
	: m_listen_port{ 0 }
	, m_public_ip{ "127.0.0.1" }
	, m_wallet_ip{ "127.0.0.1" }
	, m_reward_calc_update_interval{ 300 }
	, m_auth_user{}
	, m_auth_pw{}
	, m_mining_mode{ common::Mining_mode::HASH }
	, m_devices{}
	, m_nxs_api_user{}
	, m_nxs_api_pw{}
{
}

bool Config_api_impl::read_config(std::string const& api_config_file)
{
	std::cout << "Reading config file " << api_config_file << std::endl;

	std::ifstream config_file(api_config_file);
	if (!config_file.is_open())
	{
		std::cerr << "Unable to read " << api_config_file << std::endl;
		return false;
	}

	try
	{
		json j = json::parse(config_file);
		j.at("public_ip").get_to(m_public_ip);
		j.at("listen_port").get_to(m_listen_port);
		j.at("wallet_ip").get_to(m_wallet_ip);

		std::string mining_mode = j["mining_mode"];
		std::for_each(mining_mode.begin(), mining_mode.end(), [](char& c)
			{
				c = ::tolower(c);
			});

		if (mining_mode == "prime")
		{
			m_mining_mode = common::Mining_mode::PRIME;
		}
		else
		{
			m_mining_mode = common::Mining_mode::HASH;
		}

		j.at("auth_user").get_to(m_auth_user);
		j.at("auth_pw").get_to(m_auth_pw);

		if (j.contains("reward_calc_update_interval"))
		{
			m_reward_calc_update_interval = j["reward_calc_update_interval"];
		}

		for (auto& devices_json : j["devices"])
		{
			for (auto& hardware_json : devices_json)
			{
				Hardware_config hardware_config;
				hardware_config.m_model = hardware_json["model"];
				hardware_config.m_hashrate = hardware_json["hashrate"];
				hardware_config.m_power_consumption = hardware_json["power_consumption"];

				m_devices.push_back(hardware_config);
			}
		}

		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Failed to parse config file. Exception: " << e.what() << std::endl;
		return false;
	}
}
}
}