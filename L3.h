/*
 * l3_packet.hpp
 *
 *  Created on: 21 Jun 2025
 *      Author: mamat
 */

#ifndef L4_H_
#define L3_H_

#include "L4.h"

class l3_packet : public l4_packet {
	private:
	    uint8_t		src_ip[IP_V4_SIZE] = {0};
	    uint8_t 	dst_ip[IP_V4_SIZE] = {0};
	    unsigned int 	TTL = 0;
	    unsigned int 	CS_l3 = 0;

	public:
		/* C'tor and D'tor */
		l3_packet(const l3_packet& other) = default;
		explicit l3_packet(const std::string& raw_data);
		virtual ~l3_packet() override = default;

		/* Getters */
		uint8_t* get_src_ip() const;
		uint8_t* get_dst_ip() const;
		unsigned int get_TTL() const;
		unsigned int get_CS_l3() const;

		unsigned int get_src_ip_uint() const;
   		unsigned int get_dst_ip_uint() const;

		// Setters
    		void set_src_ip(const uint8_t* ip);
    		void set_dst_ip(const uint8_t* ip);
   		void set_TTL(unsigned int ttl);
   		void set_CS_l3(unsigned int cs);

		virtual bool validate_packet(open_port_vec open_ports,
                uint8_t ip[IP_V4_SIZE],
                uint8_t mask,
                uint8_t mac[MAC_SIZE]) override;

		virtual bool proccess_packet(open_port_vec &open_ports,
	                                uint8_t ip[IP_V4_SIZE],
	                                uint8_t mask,
	                                memory_dest &dst) override;

		virtual bool as_string(std::string &packet) override;
};

#endif /* L3_H_ */
