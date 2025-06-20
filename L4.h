/*
 * l4_packet.hpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#ifndef L4_H_
#define L4_H_

#include "packets.hpp"

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

		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		virtual bool as_string(std::string &packet) override;

	/* test HLEPER func - DELETE LATER */
	// returns the i’th byte of the payload (0 ≤ i < PACKET_DATA_SIZE)
		unsigned char get_data_byte(size_t i) const { return data[i]; }

};

#endif /* L4_H_ */
