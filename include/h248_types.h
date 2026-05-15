#ifndef H248_TYPES_H
#define H248_TYPES_H

#include <stdint.h>
#include <time.h>

typedef enum {
    H248_MSG_REQUEST = 0,
    H248_MSG_REPLY = 1,
    H248_MSG_ACK = 2
} h248_message_type_t;

typedef enum {
    H248_CMD_ADD = 0,
    H248_CMD_SUBTRACT = 1,
    H248_CMD_MODIFY = 2,
    H248_CMD_MOVE = 3,
    H248_CMD_AUDIT_VALUE = 4,
    H248_CMD_AUDIT_CAPS = 5,
    H248_CMD_NOTIFY = 6,
    H248_CMD_SERVICE_CHANGE = 7
} h248_command_type_t;

typedef enum {
    H248_ERR_OK = 0,
    H248_ERR_INVALID_MESSAGE = 400,
    H248_ERR_INVALID_COMMAND = 401,
    H248_ERR_UNSUPPORTED_COMMAND = 402,
    H248_ERR_INVALID_TERMINATION_ID = 410,
    H248_ERR_TRANSACTION_NOT_FOUND = 411,
    H248_ERR_NO_RESOURCES = 412,
    H248_ERR_INTERNAL_ERROR = 500,
    H248_ERR_GATEWAY_BUSY = 503
} h248_error_code_t;

typedef enum {
    H248_TRANS_IDLE = 0,
    H248_TRANS_PENDING = 1,
    H248_TRANS_REPLY_SENT = 2,
    H248_TRANS_COMPLETE = 3,
    H248_TRANS_ERROR = 4
} h248_transaction_state_t;

typedef enum {
    H248_PKG_GENERIC = 0,
    H248_PKG_AUDIO = 1,
    H248_PKG_VIDEO = 2,
    H248_PKG_SIGNALING = 3,
    H248_PKG_MEDIA_STREAM = 4,
    H248_PKG_RTP = 5,
    H248_PKG_CUSTOM = 255
} h248_package_type_t;

typedef struct {
    char id[256];
    uint32_t type;
} h248_termination_id_t;

typedef uint32_t h248_context_id_t;
typedef uint32_t h248_transaction_id_t;

typedef struct {
    char name[64];
    char value[512];
} h248_property_t;

typedef struct {
    uint32_t count;
    h248_property_t properties[64];
} h248_descriptor_t;

typedef struct {
    h248_command_type_t type;
    h248_termination_id_t termination_id;
    h248_context_id_t context_id;
    h248_descriptor_t descriptors;
    uint32_t error_code;
    char error_text[256];
} h248_command_t;

typedef struct {
    h248_transaction_id_t id;
    h248_transaction_state_t state;
    h248_message_type_t type;
    uint32_t command_count;
    h248_command_t commands[32];
    time_t timestamp;
    uint32_t timeout;
} h248_transaction_t;

typedef struct {
    uint32_t version;
    uint32_t mId;
    h248_transaction_id_t transaction_id;
    h248_context_id_t context_id;
} h248_message_header_t;

typedef struct {
    h248_message_header_t header;
    h248_message_type_t type;
    uint32_t command_count;
    h248_command_t commands[32];
    uint32_t error_code;
    char error_text[256];
} h248_message_t;

typedef struct {
    char local_ip[64];
    uint16_t local_port;
    char remote_ip[64];
    uint16_t remote_port;
    int socket_fd;
    uint32_t mId;
    time_t connected_time;
} h248_connection_t;

#endif
