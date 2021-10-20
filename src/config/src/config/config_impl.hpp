#ifndef NEXUSPOOL_CONFIG_IMPL_HPP
#define NEXUSPOOL_CONFIG_IMPL_HPP

#include <string>
#include <vector>
#include "json/json.hpp"
#include "config/stats_printer_config.hpp"
#include "config/config.hpp"

namespace nexuspool
{
namespace config
{
class Config_impl : public Config
{
public:

	Config_impl();

	bool read_config(std::string const& pool_config_file) override;

	std::string const& get_wallet_ip() const override  { return m_wallet_ip; }
	std::uint16_t get_wallet_port() const override  { return m_wallet_port; }
	std::uint16_t get_local_port() const override  { return m_local_port; }
	std::string const& get_public_ip() const override { return m_public_ip; }
	std::uint16_t get_miner_listen_port() const override  { return m_miner_listen_port; }
	std::uint16_t get_api_listen_port() const override  { return m_api_listen_port; }
	std::string const& get_local_ip() const override  { return m_local_ip; }
	common::Mining_mode get_mining_mode() const override  { return m_mining_mode; }
	std::string const& get_logfile() const override  { return m_logfile; }
	std::uint8_t get_log_level() const override { return m_log_level; }
	std::uint16_t get_connection_retry_interval() const override  { return m_connection_retry_interval; }
	std::uint16_t get_print_statistics_interval() const override  { return m_print_statistics_interval; }
	std::uint16_t get_height_interval() const override  { return m_get_height_interval; }
	std::uint16_t get_session_expiry_time() const override  { return m_session_expiry_time; }
	std::vector<Stats_printer_config>& get_stats_printer_config() override  { return m_stats_printer_config; }
	Pool_config const& get_pool_config() const override  { return m_pool_config; }
	Persistance_config const& get_persistance_config() const override  { return m_persistance_config; }

private:

	bool read_stats_printer_config(nlohmann::json& j);

	std::string  m_wallet_ip;
	std::uint16_t m_wallet_port;
	std::uint16_t m_local_port;
	std::string  m_public_ip;
	std::uint16_t m_miner_listen_port;
	std::uint16_t m_api_listen_port;
	std::string m_local_ip;
	common::Mining_mode	 m_mining_mode;
	Pool_config  m_pool_config;
	Persistance_config m_persistance_config;
	std::string  m_logfile;
	std::uint8_t m_log_level;

	// stats printers
	std::vector<Stats_printer_config> m_stats_printer_config;

	// advanced configs
	std::uint16_t m_connection_retry_interval;
	std::uint16_t m_print_statistics_interval;
	std::uint16_t m_get_height_interval;
	std::uint16_t m_session_expiry_time;

};

Config::Sptr create_config()
{
	return std::make_shared<Config_impl>();
}

}
}
#endif 