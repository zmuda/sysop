#define CLIENTS 1
#define SERVERS 2
#define LEN 256
typedef struct message{
    long mtype;
    char who[64];
    char what[LEN];
} message;
typedef struct nack{
    long mtype;
    char ok;
} nack;
#define LIMIT 32
