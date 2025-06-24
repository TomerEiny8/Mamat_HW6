/*
 * l4_packet.hpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#ifndef L4_H_
#define L4_H_

#include "packets.hpp"
#include <string>
#include <cstring>

enum NUMERIC_BASE { DEC = 10, HEX = 16 };

class l4_packet : public generic_packet {
	private:
		unsigned short 	src_prt = 0;
		unsigned short 	dst_prt = 0;
	    unsigned int 	addrs = 0;
	    unsigned char 	data[PACKET_DATA_SIZE] = {0};

	public:
		/* C'tor and D'tor */
		l4_packet(const l4_packet& other) = default;
		explicit l4_packet(const std::string& raw_data);
		virtual ~l4_packet() override = default;

		/* Getters */
		unsigned short get_src_port() const;
		unsigned short get_dst_port() const;
		unsigned int get_addrs() const;
		const unsigned char* get_data() const;
		virtual unsigned int calc_sum() const;

		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		virtual bool as_string(std::string &packet) override;

		/**
		 * @fn extarct_and_write
		 * @brief Extracts N Bytes of data from its delimited string
		 * representation and writes the data into a given destination of size N
		 *
		 * This function extracts a string-formatted data with a given delimiter
		 * (e.g. "45:60:6b:d9:15:8d", "102.52.229.159", "43 05 16 cd 47")
		 * and fills an array of size N with the corresponding numeric values
		 * interpreted in the specified base (DEC, HEX, ...)
		 *
		 *
		 *	@tparam T The type of each element in the destination array.
		 *	@tparam N The number of elements to extract and write.
		 *
		 * @param [in] src The string containing the data.
		 * @param [in] base The required numeric base to of extracted data.
		 * @param [in] delim The delimiter separating data bytes in the string.
		 * @param [out] dst The array to store the parsed data bytes into.
		 *
		 * @return None.
		 */
		template <typename T, size_t N>
		void extract_and_write(const std::string &src,
				T (&dst)[N], int base, char delim) {
			std::string byte_str;
			for (size_t i = 0; i < N; i++) {
				byte_str = extract_between_delimiters(src, delim, i, i);
				if (i == N-1) {
					byte_str = extract_between_delimiters(src, delim, N-1, -1);
				}
				if (byte_str.empty()) {
					byte_str = "0";
				}
				dst[i] = static_cast<T>(std::stoul(byte_str, nullptr, base));
			}
		}
};

#endif /* L4_H_ */
