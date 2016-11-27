typedef struct {
    void (*callback)(void *);
    void *arg;
}threadStruct;

void init_mutex();
void P(int sem);
void V(int sem);
void *clientThread(void *arg);
int lanceThread(void (*function)(void *), void *arg, int taille);
int getThreadCount();

