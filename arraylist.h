typedef struct {
    unsigned length;
    unsigned capacity;
    int *data;
} arraylist_t;


void al_init(arraylist_t *, unsigned);
void al_destroy(arraylist_t *);

void al_append(arraylist_t *, int);
