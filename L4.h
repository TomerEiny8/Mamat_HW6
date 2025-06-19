/*
 * l4_packet.hpp
 *
 *  Created on: 19 Jun 2025
 *      Author: mamat
 */

#ifndef l4_packet_HPP_
#define l4_packet_HPP_

#include "packets.hpp"

class l4_packet : public generic_packet {
	private:
		uint16_t src_port = 0;				// 2 Bytes
		uint16_t dst_port = 0;				// 2 Bytes
		uint32_t addrs = 0; 				// 4 Bytes
		uint8_t data[PACKET_DATA_SIZE]{0};	//32 Bytes

	public:
		/* C'tor and D'tor */
		l4_packet(const l4_packet& other) = default;
		explicit l4_packet(const std::string& raw_data);
		virtual ~l4_packet() override = default;

		/* Getters */
		uint16_t get_src_port() const;
		uint16_t get_dst_port() const;
		uint32_t get_addrs() const;

		virtual bool validate_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                uint8_t mac[MAC_SIZE]) override;

		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		virtual bool as_string(std::string &packet) override;

	    virtual bool full_packet_string(std::string &packet) override;



#endif /* l4_packet_HPP_ */
