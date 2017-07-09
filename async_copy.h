
#ifndef ASYNC_COPY_H
#define ASYNC_COPY_H

struct copy_status {
    unsigned long int size;
    unsigned long int read_size;
};

void async_copy(char *src, char *dst, struct copy_status *cps);

#endif /* ASYNC_COPY_H */

