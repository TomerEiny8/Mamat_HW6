/**
 * @file NIC_sim.hpp
 * @brief This header defines the NIC simulator class, responsible for managing
 *        NIC parameters, processing packets, and storing simulation results.
 *
 * The purpose of this class is to simulate the behavior of a Network Interface
 * Card by updating configurations, handling packet flows, and outputting the 
 * final state of memory and queues.
 */


#ifndef __NIC_SIM__
#define __NIC_SIM__

#include <fstream>
#include "common.hpp"
#include "packets.hpp"
#include "L2.h"
#include "L3.h"
#include "L4.h"

enum PACKET_TYPE { L2 = 2, L3, L4 };

class nic_sim {
    public:
    /**
     * @fn nic_sim
     * @brief Constructor of the class.
     * 
     * @param param_file - File name containing the NIC's parameters.
     *
     * @return New simulation object.
     */
    nic_sim(const std::string param_file);

    /**
     * @fn nic_flow
     * @brief Process and store to relevant location all packets in packet_file.
     *
     * @param packet_file - Name of file containing packets as strings.
     *
     * @return None.
     */
    void nic_flow(const std::string packet_file);

    /**
     * @fn nic_print_results
     * @brief Prints all data stored in memory to stdout in the following format:
     *
     *        LOCAL DRAM:
     *        [src] [dst]: [data - DATA_ARR_SIZE bytes]
     *        [src] [dst]: [data - DATA_ARR_SIZE bytes]
     *        ...
     *
     *        RQ:
     *        [each packet in separate line]
     *
     *        TQ:
     *        [each packet in separate line]
     *
     * @return None.
     */
    void nic_print_results();

    /**
     * @fn ~nic_sim
     * @brief Destructor of the class.
     *
     * @return None.
     */
    ~nic_sim() = default;

    private:
    /**
     * @fn packet_factory
     * @brief Gets a string representing a packet, creates the corresponding
     *        packet type, and returns a pointer to a generic_packet.
     *
     * @param packet - String representation of a packet.
     *
     * @return Pointer to a generic_packet object.
     */
    generic_packet *packet_factory(const std::string &packet);

    /**
     * @param open_ports - Vector containing all open communications.
     * @param RQ - Vector of strings to store packets that sent to RQ.
     * @param TQ - Vector of strings to store packets that sent to TQ.
     * @param ip - The NIC's IPv4.
     * @param mask - The NIC's IPv4's mask .
     * @param mac - The NIC's MAC.
     */
    common::open_port_vec 		open_ports;
    std::vector<std::string> 	RQ;
    std::vector<std::string> 	TQ;
    uint8_t 					ip[IP_V4_SIZE];
    uint8_t 					mask;
    uint8_t 					mac[MAC_SIZE];

    /**
     * @note It is recommended and even encouraged to add new functions or
     *       additional parameters to the object, but the existing functionality
     *       must be implemented.
     */

    void print_port_data(const open_port &port);

    /**
	 * @fn detect_packet_type
	 * @brief Gets a string representing a packet, returns the packet type.
	 *
	 * @param packet - String representation of a packet.
	 *
	 * @return PACKET_TYPE corresponding to the packet's type.
	 */
    PACKET_TYPE detect_packet_type(const std::string &packet);

    /**
	 * @fn extract_between_delimiters
	 * @brief Extracts a substring between two delimiters in a string.
	 *
	 * This function extracts the substring starting immediately after the
	 * delimiter at start_index and ending just before the delimiter at end_index.
	 * If end_index is -1, it extracts until the end of the string. if start_index
	 * is 0 it extract from the beginning of the string.
	 *
	 * @param input The input string to extract substring from.
	 * @param delimiter The character used as delimiter in the string.
	 * @param start_index The index (1-based) of the delimiter after which
	 *                    extraction begins.
	 *                    if 0 start from the beginning of the string.
	 * @param end_index The index (0-based: the first delimiter will be count as
	 *                  0) of the delimiter before which extraction ends.
	 *                  If -1, extracts until the end of the string.
	 *
	 * @return Substring starting after the delimiter at start_index and
	 *         ending just before the delimiter at end_index or end of string if end_index is -1.
	 *         Returns an empty string if indices are invalid.
	 */
	static std::string extract_between_delimiters(const std::string& input,
										char delimiter,
										int start_index,
										int end_index = -1)
	{
		std::vector<size_t> positions;
		size_t pos = input.find(delimiter);
		size_t start = 0;

		/* Create position vector. */
		while (pos != std::string::npos) {
			positions.push_back(pos);
			pos = input.find(delimiter, pos + 1);
		}

		/* Check start index. */
		if (start_index < 0 || start_index > static_cast<int>(positions.size())) {
			return "";
		}

		if (start_index > 0) {
			if (start_index > positions.size()) return ""; // invalid
			start = positions[start_index - 1] + 1;
		}

		/* Extract everything after start delimiter to end of string. */
		if (end_index == -1) {
			return input.substr(start);
		}

		/* Validate end_index. */
		if (end_index < start_index || end_index >= static_cast<int>(positions.size())) {
			return "";
		}

		size_t len = positions[end_index] - start;

		return input.substr(start, len);
	}

    /**
	 * @fn extract_and_write
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

#endif
