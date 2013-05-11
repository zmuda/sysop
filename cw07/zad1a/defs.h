#define CLIENTS 1
#define SERVERS 2
typedef struct message{
    long mtype;
    time_t when;
    char to[128];
    char what[256];
} message;
typedef struct id{
    long mtype;
    int id;
} itgr;
typedef struct text{
    long mtype;
    char text[128];
} text;

#define LIMIT 32
