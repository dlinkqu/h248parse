#ifndef H248_PARSER_H
#define H248_PARSER_H

#include "h248_types.h"

typedef enum {
    H248_PARSE_OK = 0,
    H248_PARSE_ERROR = -1,
    H248_PARSE_INCOMPLETE = -2,
    H248_PARSE_INVALID_SYNTAX = -3
} h248_parse_result_t;

int h248_parser_init(void);
void h248_parser_cleanup(void);
int h248_parse_message(const char *buffer, size_t length, h248_message_t *msg);
int h248_encode_message(const h248_message_t *msg, char *buffer, size_t max_length);
int h248_parse_command(const char *cmd_str, h248_command_t *cmd);
int h248_encode_command(const h248_command_t *cmd, char *buffer, size_t max_length);
int h248_parse_descriptor(const char *desc_str, h248_descriptor_t *desc);
int h248_encode_descriptor(const h248_descriptor_t *desc, char *buffer, size_t max_length);
int h248_validate_message(const h248_message_t *msg);
h248_command_type_t h248_get_command_type(const char *cmd_name);
const char* h248_get_command_name(h248_command_type_t type);

#endif
