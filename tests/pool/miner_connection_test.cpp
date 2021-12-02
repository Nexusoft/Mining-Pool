#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "pool/miner_connection.hpp"
#include "pool/session.hpp"
#include "network/connection_mock.hpp"
#include "pool/pool_manager_mock.hpp"
#include "pool/session_mock.hpp"
#include "LLC/random.h"

using namespace ::nexuspool;
using namespace ::testing;

class Miner_connection_fixture : public ::testing::Test
{
public:

	Miner_connection_fixture()
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);
		m_connection = std::make_shared<NiceMock<network::Connection_mock>>();
		m_pool_manager = std::make_shared<NiceMock<Pool_manager_mock>>();
		m_session_registry = std::make_shared<NiceMock<Session_registry_mock>>();

		m_miner_connection = create_miner_connection(m_logger, m_connection, m_pool_manager, LLC::GetRand256(), m_session_registry);
	}

protected:

	std::shared_ptr<spdlog::logger> m_logger;
	std::shared_ptr<network::Connection_mock> m_connection;
	std::shared_ptr<Pool_manager_mock> m_pool_manager;
	std::shared_ptr<Session_registry_mock> m_session_registry;
	std::shared_ptr<Miner_connection> m_miner_connection;


	void TearDown() override
	{
		spdlog::drop("logger");
	}
};

TEST_F(Miner_connection_fixture, stop_test)
{
	EXPECT_CALL(*m_connection, close()).Times(1);
	m_miner_connection->stop();
}

TEST_F(Miner_connection_fixture, set_current_height_test)
{
	EXPECT_CALL(*m_connection, transmit(_)).Times(1);
	m_miner_connection->set_current_height(99);
}