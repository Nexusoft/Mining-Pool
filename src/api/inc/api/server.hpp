#ifndef NEXUSPOOL_API_SERVER_HPP
#define NEXUSPOOL_API_SERVER_HPP

#include "persistance/data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "config/config_api.hpp"

#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <atomic>

namespace asio { class io_context; }
namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
    class Connection;
    class Shared_data_reader;

class Server 
{
public:

    Server(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_reader::Uptr data_reader,
        config::Config_api::Sptr config_api,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange);

    void start();
    void stop();

private:

    std::shared_ptr<::asio::io_context> m_io_context;
    std::thread m_server_thread;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<Shared_data_reader> m_shared_data_reader;
    config::Config_api::Sptr m_config_api;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
    std::atomic_bool m_server_stopped;

};

}
}

#endif
