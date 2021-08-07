#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_IMPL_HPP

#include "persistance/data_writer.hpp"
#include "persistance/data_storage.hpp"
#include "persistance/command/command.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

#include <memory>
#include <string>
#include <mutex>

namespace nexuspool
{
namespace persistance
{
namespace command { class Command_factory; }
using ::nexuspool::persistance::command::Command;

class Data_writer_impl : public Data_writer
{
public:

    Data_writer_impl(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_storage::Sptr data_storage,
        std::shared_ptr<persistance::command::Command_factory> command_factory);

    bool create_tables() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    // needed commands
    std::shared_ptr<Command> m_create_tables_cmd;


};

class Shared_data_writer_impl : public Shared_data_writer
{
public:

    Shared_data_writer_impl(Data_writer::Uptr data_writer);

    bool create_tables() override;

private:

    std::mutex m_writer_mutex;
    Data_writer::Uptr m_data_writer;
};

}
}

#endif
