#include "config/validator.hpp"
#include "config/types.hpp"
#include "json/json.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

namespace nexuspool
{
namespace config
{
Validator::Validator()
: m_mandatory_fields{}
, m_optional_fields{}
{
}

bool Validator::check(std::string const& config_file)
{
    std::ifstream config(config_file);
    if (!config.is_open())
    {
        std::cerr << "Unable to read " << config_file << std::endl;
        return false;
    }

    try
    {
        json j = json::parse(config);
        // check mandatory fields;
        if (j.count("wallet_ip") == 0)
        {
            m_mandatory_fields.push_back(Validator_error{"wallet_ip", ""});
        }

        if (j.count("wallet_port") == 0)
        {
            m_mandatory_fields.push_back(Validator_error{"wallet_port", ""});
        }
        else
        {
            if(!j.at("wallet_port").is_number())
            {
                m_mandatory_fields.push_back(Validator_error{"wallet_port", "Not a number"});
            }
        }

        if (j.count("mining_mode") == 0)
        {
            m_mandatory_fields.push_back(Validator_error{"mining_mode", ""});
        }
        else
        {
            // check content of mining_mode
            std::string mining_mode = j["mining_mode"];
            std::for_each(mining_mode.begin(), mining_mode.end(), [](char & c) {
                c = ::tolower(c);
    	    });
            if(mining_mode != "prime" && mining_mode != "hash")
            {
                m_mandatory_fields.push_back(Validator_error{"mining_mode", "Not 'prime' or 'hash'"});
            }
        }


        if(j.count("pool")["address"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{"pool/address", ""});
        }
        if (j.count("pool")["account"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/account", "" });
        }
        if (j.count("pool")["pin"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/pin", "" });
        }
        if (j.count("pool")["fee"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/fee", "" });
        }
        else
        {
            if (!j.at("pool")["fee"].is_number())
            {
                m_mandatory_fields.push_back(Validator_error{ "pool/fee", "Not a number" });
            }
        }
        if (j.count("pool")["min_share"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/min_share", "" });
        }
        else
        {
            if (!j.at("pool")["min_share"].is_number())
            {
                m_mandatory_fields.push_back(Validator_error{ "pool/min_share", "Not a number" });
            }
        }
        if (j.count("pool")["difficulty_divider"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/difficulty_divider", "" });
        }
        else
        {
            if (!j.at("pool")["difficulty_divider"].is_number())
            {
                m_mandatory_fields.push_back(Validator_error{ "pool/difficulty_divider", "Not a number" });
            }
        }       
        if (j.count("pool")["round_duration_hours"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "pool/round_duration_hours", "" });
        }
        else
        {
            if (!j.at("pool")["round_duration_hours"].is_number())
            {
                m_mandatory_fields.push_back(Validator_error{ "pool/round_duration_hours", "Not a number" });
            }
        }

        // persistance config
        if (j.count("persistance")["type"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "persistance/type", "" });
        }
        else
        {
            if (j.at("persistance")["type"] != "database" &&
                j.at("persistance")["type"] != "sqlite")
            {
                m_mandatory_fields.push_back(Validator_error{ "persistance/type", "unsupported persistance type! Must be 'database' or 'sqlite'" });
            }
        }
        if (j.count("persistance")["file"] == 0)
        {
            m_mandatory_fields.push_back(Validator_error{ "persistance/file", "" });
        }


        //stats printers
        for (auto& stats_printers_json : j["stats_printers"])
        {
            for(auto& stats_printer_config_json : stats_printers_json)
            {
                auto stats_printer_mode = stats_printer_config_json["mode"];
                if(!stats_printer_mode.is_string())
                {
                    m_optional_fields.push_back(Validator_error{"stats_printers/stats_printer/mode", "Not a string"});
                    break;
                }

                if(stats_printer_mode != "console" && stats_printer_mode != "file")
                {
                    m_optional_fields.push_back(Validator_error{"stats_printers/stats_printer/mode", "Not 'console' or 'file"});
                    break;
                }
            }
        }

        //advanced config
		if (j.count("connection_retry_interval") != 0)
		{
            if(!j.at("connection_retry_interval").is_number())
            {
                m_optional_fields.push_back(Validator_error{"connection_retry_interval", "Not a number"});
            }
		}
		if (j.count("print_statistics_interval") != 0)
		{
            if(!j.at("print_statistics_interval").is_number())
            {
                m_optional_fields.push_back(Validator_error{"print_statistics_interval", "Not a number"});
            }
		}
		if (j.count("get_height_interval") != 0)
		{
                        if(!j.at("get_height_interval").is_number())
            {
                m_optional_fields.push_back(Validator_error{"get_height_interval", "Not a number"});
            }
		}
        if (j.count("session_expiry_time") != 0)
        {
            if (!j.at("session_expiry_time").is_number())
            {
                m_optional_fields.push_back(Validator_error{ "session_expiry_time", "Not a number" });
            }
        } 
    }
    catch(const std::exception& e)
    {
        std::cerr << "Incomplete json file" <<std::endl;
        std::cerr << e.what();
        return false;
    }

    if(m_mandatory_fields.empty() && m_optional_fields.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string Validator::get_check_result() const
{
    std::stringstream result;
    result << "Mandatory fields errors: " << m_mandatory_fields.size() << std::endl;
    result << "Optional fields errors: " << m_optional_fields.size() << std::endl;

    if(!m_mandatory_fields.empty())
    {
        result << "--- Mandatory fields ---" << std::endl;
    }
    for(auto const& error : m_mandatory_fields)
    {
        result << "[" << error.m_field << "]\t" << (error.m_message.empty() ? " is missing" : error.m_message) << std::endl; 
    }

    if(!m_optional_fields.empty())
    {
        result << "--- Optional fields ---" << std::endl;
    }
    for(auto const& error : m_optional_fields)
    {
        result << "[" << error.m_field << "]\t" << (error.m_message.empty() ? " is missing" : error.m_message) << std::endl; 
    }

    return result.str();
}

}
}
