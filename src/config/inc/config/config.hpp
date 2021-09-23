#ifndef NEXUSPOOL_CONFIG_HPP
#define NEXUSPOOL_CONFIG_HPP

#include <string>
#include <vector>
#include <memory>
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

	using Sptr = std::shared_ptr<Config>;
	virtual ~Config() = default;

	virtual bool read_config(std::string const& pool_config_file) = 0;

	virtual std::string const& get_wallet_ip() const = 0;
	virtual	std::uint16_t get_wallet_port() const = 0;
	virtual	std::uint16_t get_local_port() const = 0;
	virtual std::uint16_t get_local_listen_port() const = 0;
	virtual std::uint16_t get_api_listen_port() const = 0;
	virtual std::string const& get_local_ip() const = 0;
	virtual common::Mining_mode get_mining_mode() const = 0;
	virtual std::string const& get_logfile() const = 0;
	virtual std::uint16_t get_connection_retry_interval() const = 0;
	virtual std::uint16_t get_print_statistics_interval() const = 0;
	virtual std::uint16_t get_height_interval() const = 0;
	virtual std::uint16_t get_session_expiry_time() const = 0;
	virtual std::vector<Stats_printer_config>& get_stats_printer_config() = 0;
	virtual Pool_config const& get_pool_config() const = 0;
	virtual Persistance_config const& get_persistance_config() const = 0;
};

Config::Sptr create_config();



}
}
#endif 