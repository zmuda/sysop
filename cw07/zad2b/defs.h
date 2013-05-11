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

#define QUEUENAME "/server"
#define QUEUESIZE 10

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
    mqd_t queue_id = mq_open (name, O_RDWR | O_CREAT , 0664, &attr);
    if(queue_id<0)perror(NULL);
	return queue_id;
}

#define LIMIT 32
