// WiFi+coap.h -- boilerplate for connecting microcoap and WiFi (official Arduino WiFi or ESP8266)

#pragma once

#ifndef COAP_PORT
#define COAP_PORT 5683
#endif

#ifndef PACKET_BUF_SIZE
#define PACKET_BUF_SIZE 190
#endif


uint8_t packetbuf[PACKET_BUF_SIZE];
static uint8_t scratch_raw[32];
static coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};
WiFiUDP COAP_udp;

void coap_wifi_loop() {
	if (!COAP_udp.parsePacket()) return;
	int len = COAP_udp.read(packetbuf, PACKET_BUF_SIZE);
	if (len > 0) packetbuf[len] = 0;
	uint8_t rc;
	coap_packet_t pkt;
	if (0 != (rc = coap_parse(&pkt, packetbuf, len))) {
#ifdef DEBUG
		Serial.print("badpkt rc=");
		Serial.println(rc, DEC);
#endif
	} else {
		size_t rsplen = sizeof(packetbuf);
		coap_packet_t rsppkt;
		coap_handle_req(&scratch_buf, &pkt, &rsppkt);
		memset(packetbuf, 0, rsplen);
		if (0 != (rc = coap_build(packetbuf, &rsplen, &rsppkt))) {
#ifdef DEBUG
			Serial.print("buildfail rc=");
			Serial.println(rc, DEC);
#endif
		} else {
			COAP_udp.beginPacket(COAP_udp.remoteIP(), COAP_udp.remotePort());
			COAP_udp.write(packetbuf, rsplen);
			COAP_udp.endPacket();
		}
	}
}


void coap_wifi_begin() {
	COAP_udp.begin(COAP_PORT);
	// coap_setup(); // It's empty in microcoap
}
