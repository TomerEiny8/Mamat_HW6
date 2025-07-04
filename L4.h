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
	    /**
	     * @fn l4_packet
	     * @brief Constructs an l4_packet from a given string
	     *
	     * @param [in] raw_data - string containing data formated as follows:
	     * 		src_port(2 Bytes)|dst_port(2 Bytes)|addrs(4 Byes)|data(32 Bytes)
	     *
	     * @return new L4 packet
	     */
		l4_packet(const l4_packet& other) = default;
		/* Explisit Constructor */
		explicit l4_packet(const std::string& raw_data);
		/* distructor */
		virtual ~l4_packet() override = default;

		/**
		 * @fn calc_sum
		 * @brief Calculates the byte-wise sum of all fields in the l4_packet.
		 *
		 * This includes the `addrs` field, both bytes of `src_prt` and `dst_prt`
		 * and each byte of the `data` field.
		 *
		 * @return The total sum as an unsigned integer.
		 */
		virtual unsigned int calc_sum() const;

		/*
		 * @fn validate_packet
		 * @brief Check whether the packet is valid.
		 * @param [in] open_ports - Vector containing all the NIC's open ports.
		 * @param [in] ip - NIC's IP address.
		 * @param [in] mask - NIC's mask; together with the IP,
		 *             determines the NIC's local net.
		 * @param [in] mac - NIC's MAC address.
		 *
		 * @return true if the packet is valid and ready for processing.
		 *         false if the packet isn't valid and should be discarded.
		 */
		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		/**
		 * @fn proccess_packet
		 * @brief Modify the packet and return the memory location it should be
		 *        stored in. In the case of local DRAM, the function will store
		 *        the packet as a string in the relevant 'open_port' struct.
		 *
		 * @param [in] open_ports - Vector containing all the NIC's open ports.
		 * @param [in] ip - NIC's IP address.
		 * @param [in] mask - NIC's mask; together with the IP, determines the NIC's
		 *        local net.
		 * @param [out] dst - Reference to enum that indicate the memory space where
		 *        the packet should be stored:
		 *         LOCAL_DRAM - the function stored it to the currect struct.
		 *         RQ - Should be stored as a string in RQ.
		 *         TQ - Should be stored as string in TQ.
		 *
		 * @return true in success, false in case packet is invalid.
		 */
		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		/**
		 * @fn as_string
		 * @brief Convert the packet to string.
		 *
		 * @param [out] packet - Packet as a string, ready to be stored in RQ/TQ.
		 *
		 * @return true always.
		 */
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
