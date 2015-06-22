// conatra.h -- a DSL for microcoap routes
//
// If you don't understand what the fsck is going on here, start with this:
// https://en.wikipedia.org/wiki/X_Macro

#pragma once


#define CONTENT(ctype, body) \
	return coap_make_response(scratch, outpkt, (const uint8_t *)(body), strlen((body)), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, (ctype));

#define NOT_FOUND(ctype, body) \
	return coap_make_response(scratch, outpkt, (const uint8_t *)(body), strlen((body)), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_NOT_FOUND, (ctype));

#define BAD_REQUEST(ctype, body) \
	return coap_make_response(scratch, outpkt, (const uint8_t *)(body), strlen((body)), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, (ctype));

#define CHANGED(ctype, body) \
	return coap_make_response(scratch, outpkt, (const uint8_t *)(body), strlen((body)), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, (ctype));

#ifndef ROUTES
#error "You need to #define ROUTES for conatra to work!"
#endif

#define COAP_ROUTES \
	ROUTES \
	ROUTE_HIDDEN(COAP_well_known_autogen, COAP_METHOD_GET, URL(".well-known", "core"), { \
			return coap_make_response(scratch, outpkt, (const uint8_t *)(COAP_core_list), strlen(COAP_core_list) - 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT); \
	})
// we cut the last comma by using -1 on the length


// COAP_core_list goes first because it must be defined for COAP_ROUTES
#define URL1(a) "/" a
#define URL2(a, b) "/" a "/" b
#define URL3(a, b, c) "/" a "/" b "/" c
#define URL4(a, b, c, d) "/" a "/" b "/" c "/" d
#define URL5(a, b, c, d, e) "/" a "/" b "/" c "/" d "/" e
#define _GET_URL_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define URL(...) _GET_URL_MACRO(__VA_ARGS__, URL5, URL4, URL3, URL2, URL1)(__VA_ARGS__)
#define ROUTE_HIDDEN(name, method, path, body) ""
#define ROUTE(name, method, path, meta, body) "<" path ">" meta ","
static char* COAP_core_list =
COAP_ROUTES
;
#undef URL1
#undef URL2
#undef URL3
#undef URL4
#undef URL5
#undef _GET_URL_MACRO
#undef URL
#undef ROUTE_HIDDEN
#undef ROUTE

#define URL1(a) {1, {(a)}}
#define URL2(a, b) {2, {(a), (b)}}
#define URL3(a, b, c) {3, {(a), (b), (c)}}
#define URL4(a, b, c, d) {4, {(a), (b), (c), (d)}}
#define URL5(a, b, c, d, e) {5, {(a), (b), (c), (d), (e)}}
#define _GET_URL_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define URL(...) _GET_URL_MACRO(__VA_ARGS__, URL5, URL4, URL3, URL2, URL1)(__VA_ARGS__)
#define ROUTE_HIDDEN(name, method, path, body) \
	static const coap_endpoint_path_t COAP_path##method##name = path; \
	static int COAP_handle_##method##name(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo) body;
#define ROUTE(name, method, path, meta, body) \
	static const coap_endpoint_path_t COAP_path##method##name = path; \
	static int COAP_handle_##method##name(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo) body;
COAP_ROUTES
;
#undef URL1
#undef URL2
#undef URL3
#undef URL4
#undef URL5
#undef _GET_URL_MACRO
#undef URL
#undef ROUTE_HIDDEN
#undef ROUTE

#define URL(...) (void)
#define ROUTE_HIDDEN(name, method, path, body) {(method), COAP_handle_##method##name, &COAP_path##method##name, "ct=40"},
#define ROUTE(name, method, path, meta, body) ROUTE_HIDDEN(name, method, path, body)
extern "C" const coap_endpoint_t endpoints[] = {
COAP_ROUTES
{(coap_method_t)0, NULL, NULL, NULL}
};

#undef URL
#undef ROUTE_HIDDEN
#undef ROUTE
