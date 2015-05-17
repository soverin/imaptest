#ifndef CLIENT_H
#define CLIENT_H

#include "client-state.h"
#include "user.h"

struct mailbox_source;

struct client_vfuncs {
	void (*input)(struct client *client);
	int (*output)(struct client *client);
	void (*connected)(struct client *client);
	int (*send_more_commands)(struct client *client);
	void (*logout)(struct client *client);
	void (*free)(struct client *client);
};

struct client {
	int refcount;
	struct user *user;
	struct user_client *user_client;
	struct client_vfuncs v;
	enum client_protocol protocol;

        unsigned int idx, global_id;
        unsigned int cur;

	int fd, rawlog_fd;
	struct istream *input;
	struct ostream *output;
	struct ssl_iostream *ssl_iostream;
	struct io *io;
	struct timeout *to;

	enum login_state login_state;
	enum client_state state;
        time_t last_io;

	unsigned int delayed:1;
	unsigned int disconnected:1;
	unsigned int logout_sent:1;
	unsigned int idling:1;
};
ARRAY_DEFINE_TYPE(client, struct client *);

extern int clients_count;
extern unsigned int total_disconnects;
extern ARRAY_TYPE(client) clients;
extern bool stalled, disconnect_clients, no_new_clients;

struct client *client_new(unsigned int i, struct user *user);
int client_init(struct client *client, unsigned int idx,
		struct user *user, struct user_client *uc);
bool client_unref(struct client *client, bool reconnect);
void client_logout(struct client *client);
void client_disconnect(struct client *client);

void client_input_stop(struct client *client);
void client_input_continue(struct client *client);
void client_delay(struct client *client, unsigned int msecs);
int client_send_more_commands(struct client *client);

unsigned int clients_get_random_idx(void);

void clients_init(void);
void clients_deinit(void);

#endif
