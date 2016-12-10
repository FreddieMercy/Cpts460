#ifndef PIPE_H
#define PIPE_T

int kpipe(int pd[2]);

int close_pipe(int fd);

int read_pipe(int fd, char *buf, int n);
int write_pipe(int fd, char *buf, int n);

int pipe_demo();

int pfd();

#endif