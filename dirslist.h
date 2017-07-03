#ifndef DIRSLIST_H
#define DIRSLIST_H


typedef struct {
    unsigned int count;
    struct item **ilist;
} dirslist_t;

void items_list_free(dirslist_t **dl);
dirslist_t *items_list(char *dst_name);
//struct item *new_item(const char* item_name, enum item_type iamtype);

enum item_type {
    ISDIR = 10,
    ISFILE
};

struct item {
    char *name;
    enum item_type itype;
};


#endif /* DIRSLIST_H */

