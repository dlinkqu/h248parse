#include "h248_transaction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRANSACTIONS 256

static h248_transaction_t transactions[MAX_TRANSACTIONS];
static int transaction_count = 0;
static uint32_t transaction_counter = 1;
static uint32_t default_timeout = 30000;
static h248_transaction_reply_cb g_reply_cb = NULL;
static h248_transaction_timeout_cb g_timeout_cb = NULL;
static int trans_initialized = 0;

int h248_transaction_init(uint32_t max_transactions, uint32_t default_timeout_ms) {
    if (trans_initialized) {
        return 0;
    }
    
    memset(transactions, 0, sizeof(transactions));
    transaction_count = 0;
    transaction_counter = 1;
    default_timeout = default_timeout_ms;
    trans_initialized = 1;
    
    return 0;
}

void h248_transaction_cleanup(void) {
    memset(transactions, 0, sizeof(transactions));
    transaction_count = 0;
    trans_initialized = 0;
}

h248_transaction_id_t h248_transaction_create(h248_message_type_t type) {
    if (!trans_initialized || transaction_count >= MAX_TRANSACTIONS) {
        return 0;
    }
    
    h248_transaction_id_t trans_id = transaction_counter++;
    if (transaction_counter == 0) {
        transaction_counter = 1;
    }
    
    int idx = transaction_count++;
    memset(&transactions[idx], 0, sizeof(h248_transaction_t));
    transactions[idx].id = trans_id;
    transactions[idx].type = type;
    transactions[idx].state = H248_TRANS_PENDING;
    transactions[idx].timestamp = time(NULL);
    transactions[idx].timeout = default_timeout;
    
    return trans_id;
}

int h248_transaction_add_command(h248_transaction_id_t trans_id, const h248_command_t *cmd) {
    if (!trans_initialized || !cmd) {
        return -1;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == trans_id) {
            if (transactions[i].command_count < 32) {
                memcpy(&transactions[i].commands[transactions[i].command_count], cmd, sizeof(h248_command_t));
                transactions[i].command_count++;
                return 0;
            }
            return -1;
        }
    }
    
    return -1;
}

int h248_transaction_send(h248_transaction_id_t trans_id, int conn_id) {
    if (!trans_initialized) {
        return -1;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == trans_id) {
            h248_message_t msg;
            memset(&msg, 0, sizeof(msg));
            msg.header.transaction_id = trans_id;
            msg.type = transactions[i].type;
            msg.command_count = transactions[i].command_count;
            memcpy(msg.commands, transactions[i].commands, 
                   transactions[i].command_count * sizeof(h248_command_t));
            
            transactions[i].state = H248_TRANS_PENDING;
            transactions[i].timestamp = time(NULL);
            
            return 0;
        }
    }
    
    return -1;
}

int h248_transaction_reply(const h248_message_t *reply_msg) {
    if (!trans_initialized || !reply_msg) {
        return -1;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == reply_msg->header.transaction_id) {
            transactions[i].state = H248_TRANS_REPLY_SENT;
            
            if (g_reply_cb) {
                g_reply_cb(&transactions[i], reply_msg);
            }
            
            return 0;
        }
    }
    
    return -1;
}

h248_transaction_state_t h248_transaction_get_state(h248_transaction_id_t trans_id) {
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == trans_id) {
            return transactions[i].state;
        }
    }
    
    return H248_TRANS_IDLE;
}

int h248_transaction_get_message(h248_transaction_id_t trans_id, h248_message_t *msg) {
    if (!trans_initialized || !msg) {
        return -1;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == trans_id) {
            memset(msg, 0, sizeof(h248_message_t));
            msg->header.transaction_id = trans_id;
            msg->type = transactions[i].type;
            msg->command_count = transactions[i].command_count;
            memcpy(msg->commands, transactions[i].commands, 
                   transactions[i].command_count * sizeof(h248_command_t));
            return 0;
        }
    }
    
    return -1;
}

void h248_transaction_set_reply_callback(h248_transaction_reply_cb cb) {
    g_reply_cb = cb;
}

void h248_transaction_set_timeout_callback(h248_transaction_timeout_cb cb) {
    g_timeout_cb = cb;
}

int h248_transaction_process_timeouts(void) {
    if (!trans_initialized) {
        return 0;
    }
    
    int timeouts = 0;
    time_t now = time(NULL);
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].state == H248_TRANS_PENDING) {
            uint32_t elapsed = (uint32_t)(now - transactions[i].timestamp) * 1000;
            if (elapsed >= transactions[i].timeout) {
                transactions[i].state = H248_TRANS_ERROR;
                if (g_timeout_cb) {
                    g_timeout_cb(&transactions[i]);
                }
                timeouts++;
            }
        }
    }
    
    return timeouts;
}

int h248_transaction_cancel(h248_transaction_id_t trans_id) {
    if (!trans_initialized) {
        return -1;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].id == trans_id) {
            transactions[i].state = H248_TRANS_IDLE;
            return 0;
        }
    }
    
    return -1;
}

int h248_transaction_get_count(void) {
    return (trans_initialized) ? transaction_count : 0;
}
