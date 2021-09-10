/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#ifndef LIBMQTTC_H
#define LIBMQTTC_H

#define LIBMQTTC_VERSION "0.0.1"

#include <sys/time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Timer {
	struct timeval end_time;
} Timer;

typedef struct Network {
	int my_socket;
	int (*read) (struct Network*, unsigned char*, int, int);
	int (*write) (struct Network*, unsigned char*, int, int);
} Network;

enum mqtt_errors {
	MQTTPACKET_BUFFER_TOO_SHORT = -2,
	MQTTPACKET_READ_ERROR = -1,
	MQTTPACKET_READ_COMPLETE
};

enum mqtt_msgTypes {
	CONNECT = 1, CONNACK, PUBLISH, PUBACK, PUBREC, PUBREL,
	PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK,
	PINGREQ, PINGRESP, DISCONNECT
};

enum mqtt_connack_return_codes {
    MQTT_CONNECTION_ACCEPTED = 0,
    MQTT_UNNACCEPTABLE_PROTOCOL = 1,
    MQTT_CLIENTID_REJECTED = 2,
    MQTT_SERVER_UNAVAILABLE = 3,
    MQTT_BAD_USERNAME_OR_PASSWORD = 4,
    MQTT_NOT_AUTHORIZED = 5,
};


/**
 * Bitfields for the MQTT header byte.
 */
typedef union
{
	unsigned char byte;	                /**< the whole byte */
#if defined(REVERSED)
	struct
	{
		unsigned int type : 4;			/**< message type nibble */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int retain : 1;		/**< retained flag bit */
	} bits;
#else
	struct
	{
		unsigned int retain : 1;		/**< retained flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int type : 4;			/**< message type nibble */
	} bits;
#endif
} mqtt_header;

typedef struct
{
	int len;
	char* data;
} mqtt_len_string;

typedef struct
{
	char* cstring;
	mqtt_len_string lenstring;
} mqtt_string;

#define mqtt_string_initializer {NULL, {0, NULL}}

typedef union
{
	unsigned char all;	/**< all connect flags */
#if defined(REVERSED)
	struct
	{
		unsigned int username : 1;			/**< 3.1 user name */
		unsigned int password : 1; 			/**< 3.1 password */
		unsigned int willRetain : 1;		/**< will retain setting */
		unsigned int willQoS : 2;				/**< will QoS value */
		unsigned int will : 1;			    /**< will flag */
		unsigned int cleansession : 1;	  /**< clean session flag */
		unsigned int : 1;	  	          /**< unused */
	} bits;
#else
	struct
	{
		unsigned int : 1;	     					/**< unused */
		unsigned int cleansession : 1;	  /**< cleansession flag */
		unsigned int will : 1;			    /**< will flag */
		unsigned int willQoS : 2;				/**< will QoS value */
		unsigned int willRetain : 1;		/**< will retain setting */
		unsigned int password : 1; 			/**< 3.1 password */
		unsigned int username : 1;			/**< 3.1 user name */
	} bits;
#endif
} mqtt_conn_flags;	/**< connect flags byte */



/**
 * Defines the MQTT "Last Will and Testament" (LWT) settings for
 * the connect packet.
 */
typedef struct
{
	/** The eyecatcher for this structure.  must be MQTW. */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** The LWT topic to which the LWT message will be published. */
	mqtt_string topicName;
	/** The LWT payload. */
	mqtt_string message;
	/**
      * The retained flag for the LWT message (see MQTTAsync_message.retained).
      */
	unsigned char retained;
	/**
      * The quality of service setting for the LWT message (see
      * MQTTAsync_message.qos and @ref qos).
      */
	char qos;
} mqtt_pkt_will_options;


#define mqtt_pkt_will_options_initializer { {'M', 'Q', 'T', 'W'}, 0, {NULL, {0, NULL}}, {NULL, {0, NULL}}, 0, 0 }


typedef struct
{
	/** The eyecatcher for this structure.  must be MQTC. */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
	  */
	unsigned char mqtt_version;
	mqtt_string clientID;
	unsigned short keepAliveInterval;
	unsigned char cleansession;
	unsigned char willFlag;
	mqtt_pkt_will_options will;
	mqtt_string username;
	mqtt_string password;
} mqtt_pkt_conn_data;

typedef union
{
	unsigned char all;	/**< all connack flags */
#if defined(REVERSED)
	struct
	{
    unsigned int reserved : 7;	  	    /**< unused */
		unsigned int sessionpresent : 1;    /**< session present flag */
	} bits;
#else
	struct
	{
		unsigned int sessionpresent : 1;    /**< session present flag */
    unsigned int reserved: 7;	     			/**< unused */
	} bits;
#endif
} mqtt_connack_flags;	/**< connack flags byte */



int mqtt_strlen(mqtt_string mqttstring);

int mqtt_serialize_publish(unsigned char* buf, int buflen, unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
		mqtt_string topicName, unsigned char* payload, int payloadlen);

int mqtt_deserialize_publish(unsigned char* dup, int* qos, unsigned char* retained, unsigned short* packetid, mqtt_string* topicName,
		unsigned char** payload, int* payloadlen, unsigned char* buf, int len);

int mqtt_serialize_puback(unsigned char* buf, int buflen, unsigned short packetid);
int mqtt_serialize_pubrel(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid);
int mqtt_serialize_pubcomp(unsigned char* buf, int buflen, unsigned short packetid);

int mqtt_serialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
		int count, mqtt_string topicFilters[], int requestedQoSs[]);

int mqtt_deserialize_subscribe(unsigned char* dup, unsigned short* packetid,
		int maxcount, int* count, mqtt_string topicFilters[], int requestedQoSs[], unsigned char* buf, int len);

int mqtt_serialize_suback(unsigned char* buf, int buflen, unsigned short packetid, int count, int* grantedQoSs);

int mqtt_deserialize_suback(unsigned short* packetid, int maxcount, int* count, int grantedQoSs[], unsigned char* buf, int len);

int mqtt_serialize_unsubscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
		int count, mqtt_string topicFilters[]);

int mqtt_deserialize_unsubscribe(unsigned char* dup, unsigned short* packetid, int max_count, int* count, mqtt_string topicFilters[],
		unsigned char* buf, int len);

int mqtt_serialize_unsuback(unsigned char* buf, int buflen, unsigned short packetid);

int mqtt_deserialize_unsuback(unsigned short* packetid, unsigned char* buf, int len);


const char* mqtt_pkt_getname(unsigned short packetid);
int mqtt_stringFormat_connect(char* strbuf, int strbuflen, mqtt_pkt_conn_data* data);
int mqtt_stringFormat_connack(char* strbuf, int strbuflen, unsigned char connack_rc, unsigned char sessionPresent);
int mqtt_stringFormat_publish(char* strbuf, int strbuflen, unsigned char dup, int qos, unsigned char retained,
		unsigned short packetid, mqtt_string topicName, unsigned char* payload, int payloadlen);
int mqtt_stringFormat_ack(char* strbuf, int strbuflen, unsigned char packettype, unsigned char dup, unsigned short packetid);
int mqtt_stringFormat_subscribe(char* strbuf, int strbuflen, unsigned char dup, unsigned short packetid, int count,
		mqtt_string topicFilters[], int requestedQoSs[]);
int mqtt_stringFormat_suback(char* strbuf, int strbuflen, unsigned short packetid, int count, int* grantedQoSs);
int mqtt_stringFormat_unsubscribe(char* strbuf, int strbuflen, unsigned char dup, unsigned short packetid,
		int count, mqtt_string topicFilters[]);
char* mqtt_fmt_toClientString(char* strbuf, int strbuflen, unsigned char* buf, int buflen);
char* mqtt_fmt_toServerString(char* strbuf, int strbuflen, unsigned char* buf, int buflen);

int mqtt_serialize_ack(unsigned char* buf, int buflen, unsigned char type, unsigned char dup, unsigned short packetid);
int mqtt_deserialize_ack(unsigned char* packettype, unsigned char* dup, unsigned short* packetid, unsigned char* buf, int buflen);

int mqtt_pkt_len(int rem_len);
int mqtt_pkt_equals(mqtt_string* a, char* b);

int mqtt_pkt_encode(unsigned char* buf, int length);
int mqtt_pkt_decode(int (*getcharfn)(unsigned char*, int), int* value);
int mqtt_pkt_decodeBuf(unsigned char* buf, int* value);

int mqtt_pkt_read(unsigned char* buf, int buflen, int (*getfn)(unsigned char*, int));

typedef struct {
	int (*getfn)(void *, unsigned char*, int); /* must return -1 for error, 0 for call again, or the number of bytes read */
	void *sck;	/* pointer to whatever the system may use to identify the transport */
	int multiplier;
	int rem_len;
	int len;
	char state;
}mqtt_transport;

int mqtt_pkt_readnb(unsigned char* buf, int buflen, mqtt_transport *trp);

#define MAX_PACKET_ID 65535 /* according to the MQTT specification - do not change! */

#if !defined(MAX_MESSAGE_HANDLERS)
#define MAX_MESSAGE_HANDLERS 5 /* redefinable - how many subscriptions do you want? */
#endif

enum QoS { QOS0, QOS1, QOS2, SUBFAIL=0x80 };

/* all failure return codes must be negative */
enum returnCode { BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0 };

typedef struct mqtt_msg
{
    enum QoS qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
} mqtt_msg;

typedef struct MessageData
{
    mqtt_msg* message;
    mqtt_string* topicName;
} MessageData;

typedef struct mqtt_connack_data
{
    unsigned char rc;
    unsigned char sessionPresent;
} mqtt_connack_data;

typedef struct mqtt_suback_data
{
    enum QoS grantedQoS;
} mqtt_suback_data;

typedef void (*messageHandler)(MessageData*);

typedef struct mqtt_client
{
    unsigned int next_packetid,
      command_timeout_ms;
    size_t buf_size,
      readbuf_size;
    unsigned char *buf,
      *readbuf;
    unsigned int keepAliveInterval;
    char ping_outstanding;
    int isconnected;
    int cleansession;

    struct MessageHandlers
    {
        const char* topicFilter;
        void (*fp) (MessageData*);
    } messageHandlers[MAX_MESSAGE_HANDLERS];      /* Message handlers are indexed by subscription topic */

    void (*defaultMessageHandler) (MessageData*);

    Network* ipstack;
    Timer last_sent, last_received;
#if defined(MQTT_TASK)
    Mutex mutex;
    Thread thread;
#endif
} mqtt_client;

/**
 * Create an MQTT client object
 * @param client
 * @param network
 * @param command_timeout_ms
 * @param
 */
void mqtt_clientInit(mqtt_client* client, Network* network, unsigned int command_timeout_ms,
		unsigned char* sendbuf, size_t sendbuf_size, unsigned char* readbuf, size_t readbuf_size);

/** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
 *  The nework object must be connected to the network endpoint before calling this
 *  @param options - connect options
 *  @return success code
 */
int mqtt_connWithResults(mqtt_client* client, mqtt_pkt_conn_data* options,
    mqtt_connack_data* data);

/** MQTT Connect - send an MQTT connect packet down the network and wait for a Connack
 *  The nework object must be connected to the network endpoint before calling this
 *  @param options - connect options
 *  @return success code
 */
int mqtt_conn(mqtt_client* client, mqtt_pkt_conn_data* options);

/** MQTT Publish - send an MQTT publish packet and wait for all acks to complete for all QoSs
 *  @param client - the client object to use
 *  @param topic - the topic to publish to
 *  @param message - the message to send
 *  @return success code
 */
int mqtt_publish(mqtt_client* client, const char*, mqtt_msg*);

/** MQTT SetMessageHandler - set or remove a per topic message handler
 *  @param client - the client object to use
 *  @param topicFilter - the topic filter set the message handler for
 *  @param messageHandler - pointer to the message handler function or NULL to remove
 *  @return success code
 */
int mqtt_set_msg_handler(mqtt_client* c, const char* topicFilter, messageHandler messageHandler);

/** MQTT Subscribe - send an MQTT subscribe packet and wait for suback before returning.
 *  @param client - the client object to use
 *  @param topicFilter - the topic filter to subscribe to
 *  @param message - the message to send
 *  @return success code
 */
int mqtt_subscribe(mqtt_client* client, const char* topicFilter, enum QoS, messageHandler);

/** MQTT Subscribe - send an MQTT subscribe packet and wait for suback before returning.
 *  @param client - the client object to use
 *  @param topicFilter - the topic filter to subscribe to
 *  @param message - the message to send
 *  @param data - suback granted QoS returned
 *  @return success code
 */
int mqtt_subscribeWithResults(mqtt_client* client, const char* topicFilter, enum QoS, messageHandler, mqtt_suback_data* data);

/** MQTT Subscribe - send an MQTT unsubscribe packet and wait for unsuback before returning.
 *  @param client - the client object to use
 *  @param topicFilter - the topic filter to unsubscribe from
 *  @return success code
 */
int mqtt_unsubscribe(mqtt_client* client, const char* topicFilter);

/** MQTT Disconnect - send an MQTT disconnect packet and close the connection
 *  @param client - the client object to use
 *  @return success code
 */
int mqtt_disconn(mqtt_client* client);

/** MQTT Yield - MQTT background
 *  @param client - the client object to use
 *  @param time - the time, in milliseconds, to yield for
 *  @return success code
 */
int mqtt_yield(mqtt_client* client, int time);

/** MQTT isConnected
 *  @param client - the client object to use
 *  @return truth value indicating whether the client is connected to the server
 */
int mqtt_is_connected(mqtt_client* client);

#if defined(MQTT_TASK)
/** MQTT start background thread for a client.  After this, mqtt_yield should not be called.
*  @param client - the client object to use
*  @return success code
*/
int mqtt_start_task(mqtt_client* client);
#endif

#define mqtt_pkt_conn_data_initializer { {'M', 'Q', 'T', 'C'}, 0, 4, {NULL, {0, NULL}}, 60, 1, 0, \
		mqtt_pkt_will_options_initializer, {NULL, {0, NULL}}, {NULL, {0, NULL}} }

int mqtt_serialize_connect(unsigned char* buf, int buflen, mqtt_pkt_conn_data* options);
int mqtt_deserialize_connect(mqtt_pkt_conn_data* data, unsigned char* buf, int len);

int mqtt_serialize_connack(unsigned char* buf, int buflen, unsigned char connack_rc, unsigned char sessionPresent);
int mqtt_deserialize_connack(unsigned char* sessionPresent, unsigned char* connack_rc, unsigned char* buf, int buflen);

int mqtt_serialize_disconnect(unsigned char* buf, int buflen);
int mqtt_serialize_pingreq(unsigned char* buf, int buflen);

#ifdef __cplusplus
}
#endif
#endif
