#ifndef H248_COMMANDS_H
#define H248_COMMANDS_H

#include "h248_types.h"

typedef int (*h248_command_handler)(const h248_command_t *cmd, h248_command_t *reply);

int h248_commands_init(void);
void h248_commands_cleanup(void);
int h248_command_register_handler(h248_command_type_t type, h248_command_handler handler);
int h248_command_process(const h248_command_t *cmd, h248_command_t *reply);
int h248_command_add_termination(const h248_termination_id_t *term_id, const h248_descriptor_t *desc);
int h248_command_remove_termination(const h248_termination_id_t *term_id);
int h248_command_modify_termination(const h248_termination_id_t *term_id, const h248_descriptor_t *desc);
int h248_command_audit_termination(const h248_termination_id_t *term_id, h248_descriptor_t *result);
h248_context_id_t h248_command_create_context(void);
int h248_command_delete_context(h248_context_id_t ctx_id);
int h248_command_list_contexts(h248_context_id_t *contexts, int max_count);
int h248_command_get_context_info(h248_context_id_t ctx_id, h248_descriptor_t *info);

#endif
