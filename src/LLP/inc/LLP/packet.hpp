#ifndef NEXUSPOOL_LLP_PACKET_HPP
#define NEXUSPOOL_LLP_PACKET_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include "network/types.hpp"
#include "block.hpp"
#include "utils.hpp"

namespace nexuspool
{
    /** Class to handle sending and receiving of LLP Packets. **/
    class Packet
    {
    public:

		enum
		{
            /** DATA PACKETS **/
			BLOCK_DATA = 0,
			SUBMIT_BLOCK = 1,
			BLOCK_HEIGHT = 2,
			SET_CHANNEL = 3,
			BLOCK_REWARD = 4,
			SET_COINBASE = 5,
			GOOD_BLOCK = 6,
			ORPHAN_BLOCK = 7,

			//POOL RELATED
			LOGIN = 8,

			/** REQUEST PACKETS **/
			GET_BLOCK = 129,
			GET_HEIGHT = 130,
			GET_REWARD = 131,
			GET_PAYOUT = 132,

			//POOL RELATED
			LOGIN_SUCCESS = 134,
			LOGIN_FAIL = 135,


			/** RESPONSE PACKETS **/
			ACCEPT = 200,
			REJECT = 201,
			BLOCK = 202,
			STALE = 203,

			/** ROUND VALIDATIONS. **/
			NEW_ROUND = 204,
			OLD_ROUND = 205,


			/** GENERIC **/
			PING = 253,
			CLOSE = 254
		};

        Packet() 
			: m_header{255}
			, m_length{0}
			, m_is_valid{true}
		{
		}
		// creates a packet from received buffer
		explicit Packet(network::Shared_payload buffer)
		{
			m_is_valid = true;
			if(buffer->empty())
			{
				m_header = 255;
			}
			else
			{
				m_header = (*buffer)[0];
			}
			m_length = 0;
			if (buffer->size() > 1 && buffer->size() < 4)
			{
				m_is_valid = false;
			}
			else if (buffer->size() > 4)
			{
				m_length = ((*buffer)[1] << 24) + ((*buffer)[2] << 16) + ((*buffer)[3] << 8) + ((*buffer)[4]);
				m_data = std::make_shared<std::vector<uint8_t>>(buffer->begin() + 5, buffer->end());

				if (m_data->size() != m_length)
				{
					m_is_valid = false;
				}
			}
		}

        /** Components of an LLP Packet.
            BYTE 0       : Header
            BYTE 1 - 5   : Length
            BYTE 6 - End : Data      **/
        uint8_t			m_header;
        uint32_t		m_length;
        network::Shared_payload m_data;
		bool m_is_valid;

		inline bool is_valid() const
		{
			if (!m_is_valid)
			{
				return false;
			}

			// m_header == 0 because of LOGIN message
			return ((m_header == 0 && m_length == 0) ||(m_header < 128 && m_length > 0) || (m_header >= 128 && m_header < 255 && m_length == 0));
		}

		network::Shared_payload get_bytes()
		{
			std::vector<uint8_t> BYTES(1, m_header);

			/** Handle for Data Packets. **/
			if (m_header < 128 && m_length > 0)
			{
				BYTES.push_back((m_length >> 24)); 
				BYTES.push_back((m_length >> 16));
				BYTES.push_back((m_length >> 8));  
				BYTES.push_back(m_length);

				BYTES.insert(BYTES.end(), m_data->begin(), m_data->end());
			}

			return std::make_shared<std::vector<uint8_t>>(BYTES);
		}

		inline Packet get_packet(uint8_t header) const
		{
			Packet packet;
			packet.m_header = header;

			return packet;
		}
    };

}

#endif
