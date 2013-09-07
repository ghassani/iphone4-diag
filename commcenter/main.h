//
//  main.h
//  injection
//
//  Created by Gassan Idriss on 7/16/13.
//
//

#ifndef injection_main_h
#define injection_main_h

void init(void) __attribute__((constructor));
void start_server();
int _open(const char *, int, mode_t);
int _read(int, void*, int);
int _write(int,  void*, int);
int _close(int);
//size_t _fread(void * __restrict, size_t, size_t, FILE * __restrict);

typedef struct interpose_s {
    void *new_func;
    void *orig_func;
} interpose_t;
#endif
