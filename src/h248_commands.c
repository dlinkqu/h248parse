#include "h248_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TERMINATIONS 512
#define MAX_CONTEXTS 128

typedef struct {
    h248_termination_id_t id;
    h248_descriptor_t descriptor;
    int in_use;
} termination_t;

typedef struct {
    h248_context_id_t id;
    h248_termination_id_t terminations[32];
    int termination_count;
    int in_use;
} context_t;

static termination_t terminations[MAX_TERMINATIONS];
static context_t contexts[MAX_CONTEXTS];
static int cmd_initialized = 0;
static h248_command_handler handlers[8];
static h248_context_id_t context_counter = 1;

int h248_commands_init(void) {
    if (cmd_initialized) {
        return 0;
    }
    
    memset(terminations, 0, sizeof(terminations));
    memset(contexts, 0, sizeof(contexts));
    memset(handlers, 0, sizeof(handlers));
    context_counter = 1;
    cmd_initialized = 1;
    
    return 0;
}

void h248_commands_cleanup(void) {
    memset(terminations, 0, sizeof(terminations));
    memset(contexts, 0, sizeof(contexts));
    memset(handlers, 0, sizeof(handlers));
    cmd_initialized = 0;
}

int h248_command_register_handler(h248_command_type_t type, h248_command_handler handler) {
    if (!cmd_initialized || type >= 8 || !handler) {
        return -1;
    }
    
    handlers[type] = handler;
    return 0;
}

int h248_command_process(const h248_command_t *cmd, h248_command_t *reply) {
    if (!cmd_initialized || !cmd || !reply) {
        return -1;
    }
    
    if (cmd->type >= 8 || !handlers[cmd->type]) {
        reply->error_code = H248_ERR_UNSUPPORTED_COMMAND;
        return -1;
    }
    
    return handlers[cmd->type](cmd, reply);
}

int h248_command_add_termination(const h248_termination_id_t *term_id, const h248_descriptor_t *desc) {
    if (!cmd_initialized || !term_id || !desc) {
        return -1;
    }
    
    for (int i = 0; i < MAX_TERMINATIONS; i++) {
        if (!terminations[i].in_use) {
            memcpy(&terminations[i].id, term_id, sizeof(h248_termination_id_t));
            memcpy(&terminations[i].descriptor, desc, sizeof(h248_descriptor_t));
            terminations[i].in_use = 1;
            return 0;
        }
    }
    
    return -1;
}

int h248_command_remove_termination(const h248_termination_id_t *term_id) {
    if (!cmd_initialized || !term_id) {
        return -1;
    }
    
    for (int i = 0; i < MAX_TERMINATIONS; i++) {
        if (terminations[i].in_use && 
            strcmp(terminations[i].id.id, term_id->id) == 0) {
            terminations[i].in_use = 0;
            memset(&terminations[i], 0, sizeof(termination_t));
            return 0;
        }
    }
    
    return -1;
}

int h248_command_modify_termination(const h248_termination_id_t *term_id, const h248_descriptor_t *desc) {
    if (!cmd_initialized || !term_id || !desc) {
        return -1;
    }
    
    for (int i = 0; i < MAX_TERMINATIONS; i++) {
        if (terminations[i].in_use && 
            strcmp(terminations[i].id.id, term_id->id) == 0) {
            memcpy(&terminations[i].descriptor, desc, sizeof(h248_descriptor_t));
            return 0;
        }
    }
    
    return -1;
}

int h248_command_audit_termination(const h248_termination_id_t *term_id, h248_descriptor_t *result) {
    if (!cmd_initialized || !term_id || !result) {
        return -1;
    }
    
    for (int i = 0; i < MAX_TERMINATIONS; i++) {
        if (terminations[i].in_use && 
            strcmp(terminations[i].id.id, term_id->id) == 0) {
            memcpy(result, &terminations[i].descriptor, sizeof(h248_descriptor_t));
            return 0;
        }
    }
    
    return -1;
}

h248_context_id_t h248_command_create_context(void) {
    if (!cmd_initialized) {
        return 0;
    }
    
    for (int i = 0; i < MAX_CONTEXTS; i++) {
        if (!contexts[i].in_use) {
            h248_context_id_t ctx_id = context_counter++;
            if (context_counter == 0) {
                context_counter = 1;
            }
            
            contexts[i].id = ctx_id;
            contexts[i].in_use = 1;
            contexts[i].termination_count = 0;
            return ctx_id;
        }
    }
    
    return 0;
}

int h248_command_delete_context(h248_context_id_t ctx_id) {
    if (!cmd_initialized) {
        return -1;
    }
    
    for (int i = 0; i < MAX_CONTEXTS; i++) {
        if (contexts[i].in_use && contexts[i].id == ctx_id) {
            contexts[i].in_use = 0;
            memset(&contexts[i], 0, sizeof(context_t));
            return 0;
        }
    }
    
    return -1;
}

int h248_command_list_contexts(h248_context_id_t *contexts_out, int max_count) {
    if (!cmd_initialized || !contexts_out || max_count <= 0) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < MAX_CONTEXTS && count < max_count; i++) {
        if (contexts[i].in_use) {
            contexts_out[count++] = contexts[i].id;
        }
    }
    
    return count;
}

int h248_command_get_context_info(h248_context_id_t ctx_id, h248_descriptor_t *info) {
    if (!cmd_initialized || !info) {
        return -1;
    }
    
    for (int i = 0; i < MAX_CONTEXTS; i++) {
        if (contexts[i].in_use && contexts[i].id == ctx_id) {
            memset(info, 0, sizeof(h248_descriptor_t));
            info->count = 1;
            snprintf(info->properties[0].name, sizeof(info->properties[0].name), "ContextID");
            snprintf(info->properties[0].value, sizeof(info->properties[0].value), "%u", ctx_id);
            return 0;
        }
    }
    
    return -1;
}
