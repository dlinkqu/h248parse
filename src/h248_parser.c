#include "h248_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 256
#define MAX_TOKENS 128

static int parser_initialized = 0;

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int token_count;
} token_list_t;

static int tokenize(const char *input, token_list_t *tokens) {
    const char *p = input;
    int idx = 0;
    int token_idx = 0;
    
    memset(tokens, 0, sizeof(token_list_t));
    
    while (*p && token_idx < MAX_TOKENS) {
        if (isspace(*p)) {
            if (idx > 0) {
                idx = 0;
                token_idx++;
            }
            p++;
        } else if (*p == '{' || *p == '}' || *p == '[' || *p == ']' || 
                   *p == '(' || *p == ')' || *p == ',' || *p == ';') {
            if (idx > 0) token_idx++;
            tokens->tokens[token_idx][0] = *p;
            tokens->tokens[token_idx][1] = '\0';
            token_idx++;
            idx = 0;
            p++;
        } else {
            if (idx < MAX_TOKEN_LENGTH - 1) {
                tokens->tokens[token_idx][idx++] = *p;
                tokens->tokens[token_idx][idx] = '\0';
            }
            p++;
        }
    }
    
    if (idx > 0) token_idx++;
    tokens->token_count = token_idx;
    return token_idx;
}

int h248_parser_init(void) {
    parser_initialized = 1;
    return 0;
}

void h248_parser_cleanup(void) {
    parser_initialized = 0;
}

int h248_parse_message(const char *buffer, size_t length, h248_message_t *msg) {
    if (!parser_initialized || !buffer || !msg || length == 0) return H248_PARSE_ERROR;
    token_list_t tokens;
    int token_count = tokenize(buffer, &tokens);
    if (token_count < 5) return H248_PARSE_INCOMPLETE;
    
    memset(msg, 0, sizeof(h248_message_t));
    int idx = 0;
    if (strcmp(tokens.tokens[idx], "MEGACO") == 0 || strcmp(tokens.tokens[idx], "MegacoMessage") == 0) idx++;
    while (idx < tokens.token_count) {
        if (strcmp(tokens.tokens[idx], "version") == 0 && idx + 1 < tokens.token_count) {
            msg->header.version = atoi(tokens.tokens[++idx]);
            idx++;
        } else if (strcmp(tokens.tokens[idx], "mId") == 0 && idx + 1 < tokens.token_count) {
            msg->header.mId = atoi(tokens.tokens[++idx]);
            idx++;
        } else if (strcmp(tokens.tokens[idx], "transactions") == 0) {
            idx++;
            break;
        } else {
            idx++;
        }
    }
    return H248_PARSE_OK;
}

int h248_encode_message(const h248_message_t *msg, char *buffer, size_t max_length) {
    if (!msg || !buffer || max_length == 0) return H248_PARSE_ERROR;
    int len = snprintf(buffer, max_length,
        "MEGACO/3 [%s]\nTransaction \"%u\" { Context %u { Add %s,\n}\n}",
        "127.0.0.1:2944", msg->header.transaction_id, msg->header.context_id, "termination");
    return (len > 0 && len < (int)max_length) ? len : H248_PARSE_ERROR;
}

int h248_parse_command(const char *cmd_str, h248_command_t *cmd) {
    if (!cmd_str || !cmd || strlen(cmd_str) == 0) return H248_PARSE_ERROR;
    memset(cmd, 0, sizeof(h248_command_t));
    cmd->type = h248_get_command_type(cmd_str);
    return H248_PARSE_OK;
}

int h248_encode_command(const h248_command_t *cmd, char *buffer, size_t max_length) {
    if (!cmd || !buffer || max_length == 0) return H248_PARSE_ERROR;
    const char *cmd_name = h248_get_command_name(cmd->type);
    int len = snprintf(buffer, max_length, "%s %s", cmd_name, cmd->termination_id.id);
    return (len > 0 && len < (int)max_length) ? len : H248_PARSE_ERROR;
}

int h248_parse_descriptor(const char *desc_str, h248_descriptor_t *desc) {
    if (!desc_str || !desc || strlen(desc_str) == 0) return H248_PARSE_ERROR;
    memset(desc, 0, sizeof(h248_descriptor_t));
    return H248_PARSE_OK;
}

int h248_encode_descriptor(const h248_descriptor_t *desc, char *buffer, size_t max_length) {
    if (!desc || !buffer || max_length == 0) return H248_PARSE_ERROR;
    size_t offset = 0;
    offset += snprintf(buffer + offset, max_length - offset, "{");
    for (uint32_t i = 0; i < desc->count && i < 64; i++) {
        offset += snprintf(buffer + offset, max_length - offset, " %s %s", 
            desc->properties[i].name, desc->properties[i].value);
        if (i < desc->count - 1) offset += snprintf(buffer + offset, max_length - offset, ",");
    }
    offset += snprintf(buffer + offset, max_length - offset, " }");
    return (offset < max_length) ? (int)offset : H248_PARSE_ERROR;
}

int h248_validate_message(const h248_message_t *msg) {
    if (!msg) return 0;
    if (msg->header.version < 1 || msg->header.version > 3) return 0;
    if (msg->command_count > 32) return 0;
    return 1;
}

h248_command_type_t h248_get_command_type(const char *cmd_name) {
    if (!cmd_name) return H248_CMD_ADD;
    if (strcmp(cmd_name, "Add") == 0 || strcmp(cmd_name, "ADD") == 0) return H248_CMD_ADD;
    if (strcmp(cmd_name, "Remove") == 0 || strcmp(cmd_name, "REMOVE") == 0) return H248_CMD_SUBTRACT;
    if (strcmp(cmd_name, "Modify") == 0 || strcmp(cmd_name, "MODIFY") == 0) return H248_CMD_MODIFY;
    if (strcmp(cmd_name, "Move") == 0 || strcmp(cmd_name, "MOVE") == 0) return H248_CMD_MOVE;
    if (strcmp(cmd_name, "AuditValue") == 0 || strcmp(cmd_name, "AUDITVALUE") == 0) return H248_CMD_AUDIT_VALUE;
    if (strcmp(cmd_name, "AuditCapability") == 0 || strcmp(cmd_name, "AUDITCAPABILITY") == 0) return H248_CMD_AUDIT_CAPS;
    if (strcmp(cmd_name, "Notify") == 0 || strcmp(cmd_name, "NOTIFY") == 0) return H248_CMD_NOTIFY;
    if (strcmp(cmd_name, "ServiceChange") == 0 || strcmp(cmd_name, "SERVICECHANGE") == 0) return H248_CMD_SERVICE_CHANGE;
    return H248_CMD_ADD;
}

const char* h248_get_command_name(h248_command_type_t type) {
    switch (type) {
        case H248_CMD_ADD: return "Add";
        case H248_CMD_SUBTRACT: return "Remove";
        case H248_CMD_MODIFY: return "Modify";
        case H248_CMD_MOVE: return "Move";
        case H248_CMD_AUDIT_VALUE: return "AuditValue";
        case H248_CMD_AUDIT_CAPS: return "AuditCapability";
        case H248_CMD_NOTIFY: return "Notify";
        case H248_CMD_SERVICE_CHANGE: return "ServiceChange";
        default: return "Unknown";
    }
}
