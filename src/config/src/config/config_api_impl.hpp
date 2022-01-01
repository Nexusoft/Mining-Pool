#ifndef NEXUSPOOL_CONFIG_API_IMPL_HPP
#define NEXUSPOOL_CONFIG_API_IMPL_HPP

#include <string>
#include <vector>
#include "json/json.hpp"
#include "config/config_api.hpp"

namespace nexuspool
{
namespace config
{
class Config_api_impl : public Config_api
{
public:

	Config_api_impl();

	bool read_config(std::string const& api_config_file) override;

	std::uint16_t get_listen_port() const override { return m_listen_port; }
	std::string get_public_ip() const override { return m_public_ip; }
	std::string get_auth_user() const override { return m_auth_user; }
	std::string get_auth_pw() const override { return m_auth_pw; }
	common::Mining_mode get_mining_mode() const override { return m_mining_mode; }
	std::uint16_t get_reward_calc_update_interval() const override { return m_reward_calc_update_interval; }
	std::vector<Hardware_config>& get_devices() override { return m_devices; }

private:

	std::uint16_t m_listen_port;
	std::string m_public_ip;
	std::uint16_t m_reward_calc_update_interval;
	std::string m_auth_user;
	std::string m_auth_pw;
	common::Mining_mode	m_mining_mode;
	std::vector<Hardware_config> m_devices;
};

Config_api::Sptr create_api_config()
{
	return std::make_shared<Config_api_impl>();
}

}
}
#endif 