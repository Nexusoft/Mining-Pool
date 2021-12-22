
#include "config/config_impl.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace nexuspool
{
namespace config
{
	Config_impl::Config_impl()
		: m_wallet_ip{ "127.0.0.1" }
		, m_wallet_port{9325}
		, m_local_port{ 0 }
		, m_public_ip{ "127.0.0.1" }
		, m_miner_listen_port{ 0 }
		, m_local_ip{"127.0.0.1"}
		, m_mining_mode{ common::Mining_mode::HASH}
		, m_pool_config{}
		, m_persistance_config{}
		, m_api_config{}
		, m_logfile{""}		// no logfile usage, default
		, m_log_level{ 2 }	// info level
		, m_connection_retry_interval{5}
		, m_print_statistics_interval{5}
		, m_get_height_interval{2}
		, m_session_expiry_time{5}
	{
	}

	bool Config_impl::read_config(std::string const& pool_config_file)
	{
		std::cout << "Reading config file " << pool_config_file << std::endl;

		std::ifstream config_file(pool_config_file);
		if (!config_file.is_open())
		{
			std::cerr << "Unable to read " << pool_config_file << std::endl;
			return false;
		}

		try
		{
			json j = json::parse(config_file);
			j.at("wallet_ip").get_to(m_wallet_ip);
			j.at("wallet_port").get_to(m_wallet_port);
			if (j.count("local_ip") != 0)
			{
				j.at("local_ip").get_to(m_local_ip);
			}
			if (j.count("local_port") != 0)
			{
				j.at("local_port").get_to(m_local_port);
			}
			if (j.count("public_ip") != 0)
			{
				j.at("public_ip").get_to(m_public_ip);
			}
			if (j.count("miner_listen_port") != 0)
			{
				j.at("miner_listen_port").get_to(m_miner_listen_port);
			}

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

			m_pool_config.m_account = j.at("pool")["account"];
			m_pool_config.m_pin = j.at("pool")["pin"];
			m_pool_config.m_fee = j.at("pool")["fee"];
			m_pool_config.m_difficulty_divider = j.at("pool")["difficulty_divider"];
			m_pool_config.m_round_duration_hours = j.at("pool")["round_duration_hours"];

			auto persistance_type = j.at("persistance")["type"];
			m_persistance_config.m_file = j.at("persistance")["file"];

			if (persistance_type == "database")
			{
				m_persistance_config.m_type = Persistance_type::database;
			}
			else
			{
				m_persistance_config.m_type = Persistance_type::sqlite;
			}

			// api config
			if (j.count("api") != 0)
			{
				m_api_config.m_use_api = true;
				m_api_config.m_listen_port = j.at("api")["listen_port"];
			}

			// read stats printer config
			if (!read_stats_printer_config(j))
			{
				return false;
			}

			// advanced configs
			if (j.count("connection_retry_interval") != 0)
			{
				j.at("connection_retry_interval").get_to(m_connection_retry_interval);
			}
			if (j.count("print_statistics_interval") != 0)
			{
				j.at("print_statistics_interval").get_to(m_print_statistics_interval);
			}
			if (j.count("get_height_interval") != 0)
			{
				j.at("get_height_interval").get_to(m_get_height_interval);
			}
			if (j.count("session_expiry_time") != 0)
			{
				j.at("session_expiry_time").get_to(m_session_expiry_time);
			}

			if (j.count("logfile") != 0)
			{
				j.at("logfile").get_to(m_logfile);
			}

			if (j.count("log_level") != 0)
			{
				j.at("log_level").get_to(m_log_level);
			}

			return true;
		}
		catch (std::exception& e)
		{
			std::cerr << "Failed to parse config file. Exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Config_impl::read_stats_printer_config(nlohmann::json& j)
	{
		for (auto& stats_printers_json : j["stats_printers"])
		{
			for(auto& stats_printer_config_json : stats_printers_json)
			{
				Stats_printer_config stats_printer_config;
				auto stats_printer_mode = stats_printer_config_json["mode"];

				if(stats_printer_mode == "console")
				{
					stats_printer_config.m_mode = Stats_printer_mode::CONSOLE;
					stats_printer_config.m_printer_mode = Stats_printer_config_console{};
				}
				else if(stats_printer_mode == "file")
				{
					stats_printer_config.m_mode = Stats_printer_mode::FILE;
					stats_printer_config.m_printer_mode = Stats_printer_config_file{stats_printer_config_json["filename"]};
				}
				else
				{
					// invalid config
					return false;
				}

				m_stats_printer_config.push_back(stats_printer_config);		
			}
		}
		return true;	
	}
}
}