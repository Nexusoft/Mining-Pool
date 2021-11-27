#ifndef NEXUSPOOL_API_METHOD_IMPL_HPP
#define NEXUSPOOL_API_METHOD_IMPL_HPP

#include "api/method.hpp"
#include "api/shared_data_reader.hpp"
#include <memory>
#include <vector>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
class Method_meta_infos : public Method
{
public:

    Method_meta_infos(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);
    Method_result execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;
};

class Method_latest_blocks : public Method
{
public:

    Method_latest_blocks(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);
    Method_result execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;

};

class Method_account : public Method
{
public:

    Method_account(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);
    Method_result execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;

};

class Method_account_header : public Method
{
public:

    Method_account_header(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);
    Method_result execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;

};

class Method_account_payouts: public Method
{
public:

    Method_account_payouts(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);
    Method_result execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;

};

class Methods_factory_impl : public Methods_factory
{
public:

    Methods_factory_impl(std::shared_ptr<spdlog::logger> logger, Shared_data_reader::Sptr data_reader);

    Methods create_api_methods() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_reader::Sptr m_data_reader;
};




}
}

#endif
