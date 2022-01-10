#ifndef NEXUSPOOL_CONFIG_API_HPP
#define NEXUSPOOL_CONFIG_API_HPP

#include <string>
#include <vector>
#include <memory>
#include "config/types.hpp"
#include "common/types.hpp"

namespace nexuspool
{
namespace config
{
class Config_api
{
public:

	using Sptr = std::shared_ptr<Config_api>;
	virtual ~Config_api() = default;

	virtual bool read_config(std::string const& api_config_file) = 0;

	virtual	std::uint16_t get_listen_port() const = 0;
	virtual std::string get_public_ip() const = 0;
	virtual std::string get_wallet_ip() const = 0;
	virtual std::string get_auth_user() const = 0;
	virtual std::string get_auth_pw() const = 0;
	virtual common::Mining_mode get_mining_mode() const = 0;
	virtual std::uint16_t get_reward_calc_update_interval() const = 0;
	virtual std::vector<Hardware_config>& get_devices() = 0;
	virtual std::string get_nxs_api_user() const = 0;
	virtual std::string get_nxs_api_pw() const = 0;
	virtual void set_nxs_api_user(std::string nxs_api_user) = 0;
	virtual void set_nxs_api_pw(std::string nxs_api_pw) = 0;
};

Config_api::Sptr create_api_config();



}
}
#endif 