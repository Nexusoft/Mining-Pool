#ifndef NEXUSPOOL_CONFIG_CONFIG_MOCK_HPP
#define NEXUSPOOL_CONFIG_CONFIG_MOCK_HPP

#include <gmock/gmock.h>
#include "config/config.hpp"

namespace nexuspool {
namespace config {

class Config_mock : public Config
{
public:

    MOCK_METHOD(bool, read_config, (std::string const& pool_config_file), ( const override));
    MOCK_METHOD(std::string const&, get_wallet_ip, (), (const override));
    MOCK_METHOD(std::uint16_t, get_wallet_port, (), (const override));
    MOCK_METHOD(std::uint16_t, get_local_port, (), (const override));
    MOCK_METHOD(std::string const&, get_public_ip, (), (const override));
    MOCK_METHOD(std::uint16_t, get_miner_listen_port, (), (const override));
    MOCK_METHOD(std::string const&, get_local_ip, (), (const override));
    MOCK_METHOD(common::Mining_mode, get_mining_mode, (), (const override));
    MOCK_METHOD(std::string const&, get_logfile, (), (const override));
    MOCK_METHOD(std::uint8_t, get_log_level, (), (const override));
    MOCK_METHOD(std::uint16_t, get_connection_retry_interval, (), (const override));
    MOCK_METHOD(std::uint16_t, get_print_statistics_interval, (), (const override));
    MOCK_METHOD(std::uint16_t, get_height_interval, (), (const override));
    MOCK_METHOD(std::uint16_t, get_session_expiry_time, (), (const override));
    MOCK_METHOD(std::vector<Stats_printer_config>&, get_stats_printer_config, (), (const override));
    MOCK_METHOD(Pool_config const&, get_pool_config, (), (const override));
    MOCK_METHOD(Persistance_config const&, get_persistance_config, (), (const override));
    MOCK_METHOD(Api_config const&, get_api_config, (), (const override));
};


}
}

#endif 
