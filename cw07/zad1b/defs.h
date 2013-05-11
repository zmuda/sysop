#define CLIENTS 1
#define SERVERS 2

#define QUEUENAME "/servers"
#define QUEUESIZE 10

typedef struct message{
    time_t when;
    char to[128];
    char what[256];
} message;

/**
* tworzy plik kolejki o nazwie wynikajacej z unikalnej nazwy uzyszkodnika
* (dzieki temu nie nalezy przekazywac id kolejki (otworzyc kolejke z pliku mozna)
*    - ale i tak to robie)
* otwiera kolejke w tym pliku
*/
mqd_t createQueue(char* name,size_t size){
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUESIZE;
    attr.mq_msgsize = size;
    attr.mq_flags = 0;
    mqd_t queue_id = mq_open (name, O_RDWR | O_CREAT |O_NONBLOCK, 0664, &attr);
    if(queue_id<0)perror(NULL);
	return queue_id;
}

#define LIMIT 32
