#pragma once
#include <bitset>
#include <vector>

namespace Hamming
{
	constexpr auto N_DATA	  = 8;
	constexpr auto N_CODEWORD = 12;
	constexpr auto N_SYNDROME = N_CODEWORD - N_DATA;
}
using Data	   = std::bitset<Hamming::N_DATA>;
using Codeword = std::bitset<Hamming::N_CODEWORD>;
using Syndrome = std::bitset<Hamming::N_SYNDROME>;
using Payload  = std::vector<Codeword>;

#include <omnetpp.h>
using namespace omnetpp;

using byte	 = uint8_t;
using string = opp_string;

enum Error_Type : uint8_t
{
	ERROR_TYPE_NONE			= 0,
	ERROR_TYPE_DELAY		= 1 << 0,
	ERROR_TYPE_DUPLICATION	= 1 << 1,
	ERROR_TYPE_LOSS			= 1 << 2,
	ERROR_TYPE_MODIFICATION = 1 << 3,

	EVENT_ACK_TIMEOUT	  = 1 << 4,
	EVENT_ACK			  = 1 << 5,
	EVENT_NACK			  = 1 << 6,
	EVENT_DUPLICATE_FRAME = 1 << 7,
};

inline static const char *
error_type_to_c_str(uint8_t error)
{
	static char buf[1024];

	switch (error)
	{
	case ERROR_TYPE_NONE: return "No Errors";
	case EVENT_ACK_TIMEOUT: return "Internal: Timeout";
	case EVENT_ACK: return "Internal: ACK";
	case EVENT_NACK: return "Internal: NACK";
	case EVENT_DUPLICATE_FRAME: return "Internal: Duplicating Frame";
	default: strcpy(buf, "Errors:");
	}

	if (error & ERROR_TYPE_MODIFICATION)
		strcat(buf, " Modification");
	if (error & ERROR_TYPE_LOSS)
		strcat(buf, " Loss");
	if (error & ERROR_TYPE_DUPLICATION)
		strcat(buf, " Duplication");
	if (error & ERROR_TYPE_DELAY)
		strcat(buf, " Delay");

	return buf;
}

inline static const char *
message_type_to_c_str(uint8_t type)
{
	switch (type)
	{
	case 0: return "DATA";
	case 1: return "ACK";
	case 2: return "NACK";
	}
	return nullptr;
}

inline static std::string
payload_to_string(const Payload &payload)
{
	// Extract the first 8 bits
	std::string content{};
	for (auto &codeword : payload)
	{
		auto byte = static_cast<char>(codeword.to_ulong());
		content += byte;
	}
	return content;
}

namespace Hamming
{
	inline static constexpr uint8_t GENERATOR_MATRIX[N_CODEWORD] = {
		0b11011010, // P0
		0b10110110, // P1
		0x80 >> 0,	// M0
		0b01110001, // P2
		0x80 >> 1,	// M1
		0x80 >> 2,	// M2
		0x80 >> 3,	// M3
		0b00001111, // P3
		0x80 >> 4,	// M4
		0x80 >> 5,	// M5
		0x80 >> 6,	// M6
		0x80 >> 7,	// M7
	};

	inline static constexpr uint16_t PARITY_MATRIX[N_SYNDROME] = {
		0b101010101010, // P0
		0b011001100110, // P1
		0b000111100001, // P2
		0b000000001111, // P2
	};

	inline static constexpr uint16_t DECODER_MATRIX[N_DATA] = {
		0x800 >> 2,	 // M0
		0x800 >> 4,	 // M1
		0x800 >> 5,	 // M2
		0x800 >> 6,	 // M3
		0x800 >> 8,	 // M4
		0x800 >> 9,	 // M5
		0x800 >> 10, // M6
		0x800 >> 11, // M7
	};

	inline static Payload
	encode_payload(const string &content)
	{
		Payload payload{};

		// For each character in the message
		for (size_t c = 0; c < content.size(); c++)
		{
			auto byte = *(content.c_str() + c);

			// For each bit in the resulting codeword
			Codeword codeword{};
			for (size_t i = 0; i < N_CODEWORD; i++)
			{
				// Calculate the even parity
				Data product(GENERATOR_MATRIX[i] & byte);
				codeword[i] = product.count() & 1;
			}
		}
		return payload;
	}

	inline static std::string
	decode_payload(const Payload &payload, size_t *error_at = nullptr)
	{
		std::string message{};

		// For each codeword in the message
		for (size_t cw = 0; cw < payload.size(); cw++)
		{
			uint16_t codeword = payload[cw].to_ulong() & 0xFFFF;

			// For each bit in the resulting syndrome vector
			Syndrome syn{};
			for (size_t i = 0; i < N_SYNDROME; i++)
			{
				// Calculate the even parity
				Codeword product(PARITY_MATRIX[i] & codeword);
				syn[i] = product.count() & 1;
			}

			// Find and correct the error
			auto syn_result = syn.to_ulong();
			if (syn.to_ulong() != 0)
			{
				auto _codeword		  = payload[cw];
				_codeword[syn_result] = !_codeword[syn_result];
				codeword			  = _codeword.to_ulong() & 0xFFFF;
			}

			// 0 means no errors
			if (error_at != nullptr)
				*error_at = cw + 1;

			// For each bit in the resulting data byte
			Data byte{};
			for (size_t i = 0; i < N_DATA; i++)
			{
				// Multiply normally
				Codeword product(DECODER_MATRIX[i] & codeword);
				byte[i] = product.count();
			}

			message += static_cast<char>(byte.to_ulong());
		}
		return message;
	}
};

template <unsigned long long N>
inline std::ostream &
operator<<(std::ostream &out, const std::bitset<N> &vec)
{
	out << vec.to_string();
	return out;
}