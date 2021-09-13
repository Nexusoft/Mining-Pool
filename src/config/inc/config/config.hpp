#ifndef NEXUSPOOL_CONFIG_HPP
#define NEXUSPOOL_CONFIG_HPP

#include <string>
#include <vector>
#include "json/json.hpp"
#include "config/stats_printer_config.hpp"
#include "config/types.hpp"
#include "common/types.hpp"

namespace nexuspool
{
namespace config
{
class Config
{
public:

	Config();

	bool read_config(std::string const& pool_config_file);

	std::string const& get_wallet_ip() const { return m_wallet_ip; }
	std::uint16_t get_wallet_port() const { return m_wallet_port; }
	std::uint16_t get_local_port() const { return m_local_port; }
	std::uint16_t get_local_listen_port() const { return m_local_listen_port; }
	std::uint16_t get_api_listen_port() const { return m_api_listen_port; }
	std::string const& get_local_ip() const { return m_local_ip; }
	common::Mining_mode get_mining_mode() const { return m_mining_mode; }
	std::string const& get_logfile() const { return m_logfile; }
	std::uint16_t get_connection_retry_interval() const { return m_connection_retry_interval; }
	std::uint16_t get_print_statistics_interval() const { return m_print_statistics_interval; }
	std::uint16_t get_height_interval() const { return m_get_height_interval; }
	std::uint16_t get_session_expiry_time() const { return m_session_expiry_time; }
	std::vector<Stats_printer_config>& get_stats_printer_config() { return m_stats_printer_config; }
	Pool_config const& get_pool_config() const { return m_pool_config; }
	Persistance_config const& get_persistance_config() const { return m_persistance_config; }

private:

	bool read_stats_printer_config(nlohmann::json& j);

	std::string  m_wallet_ip;
	std::uint16_t m_wallet_port;
	std::uint16_t m_local_port;
	std::uint16_t m_local_listen_port;
	std::uint16_t m_api_listen_port;
	std::string m_local_ip;
	common::Mining_mode	 m_mining_mode;
	Pool_config  m_pool_config; 
	Persistance_config m_persistance_config;
	std::string  m_logfile;

	// stats printers
	std::vector<Stats_printer_config> m_stats_printer_config;

	// advanced configs
	std::uint16_t m_connection_retry_interval;
	std::uint16_t m_print_statistics_interval;
	std::uint16_t m_get_height_interval;
	std::uint16_t m_session_expiry_time;

};
}
}
#endif 