#define CLIENTS 1
#define SERVERS 2

#define QUEUENAME "/servers"
#define QUEUESIZE 10

typedef struct message{
    time_t when;
    char to[128];
    char what[256];
} message;


#define LIMIT 32
