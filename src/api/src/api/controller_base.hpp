#ifndef NEXUSPOOL_API_CONTROLLER_BASE_HPP
#define NEXUSPOOL_API_CONTROLLER_BASE_HPP

#include "api/dto.hpp"
#include "api/shared_data_reader.hpp"
#include "api/client.hpp"
#include "api/controller_helper.hpp"
#include "config/config_api.hpp"
#include "TAO/Register/types/address.h"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

namespace nexuspool
{
namespace api
{
using  oatpp::web::server::api::ApiController;

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Base_controller : public ApiController
{
public:

    Base_controller(chrono::Timer_factory::Sptr timer_factory,
        Shared_data_reader::Sptr data_reader,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange,
        config::Config_api::Sptr config_api,
        std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper)
    : m_data_reader{ std::move(data_reader) }
    , m_config_api{std::move(config_api)}
    , m_controller_helper{ std::move(timer_factory), 
        m_data_reader, std::move(pool_api_data_exchange),
        m_config_api->get_wallet_ip(), 
        m_config_api->get_reward_calc_update_interval(),
        m_config_api->get_nxs_api_user(),
        m_config_api->get_nxs_api_pw(),
        objectMapper }
    , ApiController(objectMapper)
    {
    }

protected:

    std::shared_ptr<ApiController::OutgoingResponse> get_meta_infos()
    {
        auto dto = Meta_infos_dto::createShared();
        auto const config = m_controller_helper.get_config_data();

        dto->pool_hashrate = m_data_reader->get_pool_hashrate();
        dto->round_duration = config.m_round_duration_hours;
        dto->fee = config.m_fee;
        dto->mining_mode = config.m_mining_mode.c_str();
        dto->active_miners = m_controller_helper.get_active_miners();
        auto const system_info = m_controller_helper.get_system_info();
        dto->wallet_version = system_info.m_wallet_version;
        dto->pool_version = system_info.m_pool_version;

        return createDtoResponse(Status::CODE_200, dto);
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_latest_blocks()
    {
        auto dto = Latest_blocks_dto::createShared();
        auto blocks = m_data_reader->get_latest_blocks();
        for (auto& block : blocks)
        {
            dto->blocks->push_back(Block_dto::createShared(block.m_height, block.m_hash.c_str(), block.m_mainnet_reward, block.m_block_found_time.c_str(), block.m_difficulty));
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_account_header(std::string account)
    {
        // is account a valid nxs address  
        TAO::Register::Address address_check{ account };
        if (!address_check.IsValid())
        {
            return createResponse(Status::CODE_400, "invalid account");
        }

        if (m_data_reader->does_account_exists(account))
        {
            return createResponse(Status::CODE_200, "");
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_account_detail(std::string account)
    {
        // is account a valid nxs address  
        TAO::Register::Address address_check{ account };
        if (!address_check.IsValid())
        {
            return createResponse(Status::CODE_400, "invalid account");
        }

        if (m_data_reader->does_account_exists(account))
        {
            auto dto = Account_dto::createShared();
            auto const account_data = m_data_reader->get_account(account);

            dto->account = account_data.m_address;
            dto->created_at = account_data.m_created_at;
            dto->last_active = account_data.m_last_active;
            dto->shares = account_data.m_shares;
            dto->hashrate = account_data.m_hashrate;
            dto->display_name = account_data.m_display_name;

            return createDtoResponse(Status::CODE_200, dto);
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_account_payout(std::string account)
    {
        // is account a valid nxs address  
        TAO::Register::Address address_check{ account };
        if (!address_check.IsValid())
        {
            return createResponse(Status::CODE_400, "invalid account");
        }

        if (m_data_reader->does_account_exists(account))
        {
            auto dto = Account_payouts_dto::createShared();
            auto const account_data = m_data_reader->get_account(account);

            auto const payments = m_data_reader->get_payments(account);
            for (auto const& payment : payments)
            {
                dto->payouts->push_back(Payout_dto::createShared(payment.m_payment_date_time.c_str(), payment.m_amount, payment.m_shares, payment.m_tx_id.c_str()));
            }

            return createDtoResponse(Status::CODE_200, dto);
        }
        else
        {
            return createResponse(Status::CODE_404, "account doesn't exist");
        }
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_reward_data()
    {
        auto mining_info = m_controller_helper.get_mining_info();
        if (!mining_info.is_valid())
        {
            return createResponse(Status::CODE_404, "get_mininginfo error");
        }

        auto dto = Reward_data_dto::createShared();
        if (m_config_api->get_mining_mode() == common::Mining_mode::HASH)
        {
            dto->block_reward = mining_info.m_hash_rewards;
            dto->network_diff = mining_info.m_hash_difficulty;
        }
        else
        {
            dto->block_reward = mining_info.m_prime_reward;
            dto->network_diff = mining_info.m_prime_difficulty;
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

    std::shared_ptr<ApiController::OutgoingResponse> get_hardware_data()
    {
        auto dto = Hardware_data_dto::createShared();

        auto const devices = m_config_api->get_devices();
        for (auto const& device : devices)
        {
            dto->devices->push_back(Hardware_dto::createShared(device.m_model.c_str(), device.m_hashrate, device.m_power_consumption));
        }

        return createDtoResponse(Status::CODE_200, dto);
    }

    Shared_data_reader::Sptr m_data_reader;
    config::Config_api::Sptr m_config_api;
    Controller_helper m_controller_helper;
};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- End codegen

}
}

#endif