#ifndef H248_TRANSACTION_H
#define H248_TRANSACTION_H

#include "h248_types.h"

typedef void (*h248_transaction_reply_cb)(const h248_transaction_t *trans, const h248_message_t *reply);
typedef void (*h248_transaction_timeout_cb)(const h248_transaction_t *trans);

int h248_transaction_init(uint32_t max_transactions, uint32_t default_timeout_ms);
void h248_transaction_cleanup(void);
h248_transaction_id_t h248_transaction_create(h248_message_type_t type);
int h248_transaction_add_command(h248_transaction_id_t trans_id, const h248_command_t *cmd);
int h248_transaction_send(h248_transaction_id_t trans_id, int conn_id);
int h248_transaction_reply(const h248_message_t *reply_msg);
h248_transaction_state_t h248_transaction_get_state(h248_transaction_id_t trans_id);
int h248_transaction_get_message(h248_transaction_id_t trans_id, h248_message_t *msg);
void h248_transaction_set_reply_callback(h248_transaction_reply_cb cb);
void h248_transaction_set_timeout_callback(h248_transaction_timeout_cb cb);
int h248_transaction_process_timeouts(void);
int h248_transaction_cancel(h248_transaction_id_t trans_id);
int h248_transaction_get_count(void);

#endif
