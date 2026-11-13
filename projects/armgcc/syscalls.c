/**
 * @file syscalls.c
 * @brief System call stubs for embedded systems
 *
 * This file provides stub implementations for system calls that are
 * required by the standard C library but not available in bare-metal
 * embedded systems.
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

/* External variables */
extern int errno;

/**
 * @brief Close a file
 * @param file File descriptor
 * @return Always returns -1 (not implemented)
 */
int _close(int file)
{
    (void)file;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Get process times
 * @param buf Buffer to store process times
 * @return Always returns -1 (not implemented)
 */
clock_t _times(struct tms *buf)
{
    (void)buf;
    return -1;
}

/**
 * @brief Get process ID
 * @return Always returns 1
 */
int _getpid(void)
{
    return 1;
}

/**
 * @brief Kill a process
 * @param pid Process ID
 * @param sig Signal number
 * @return Always returns -1 (not implemented)
 */
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Exit a process
 * @param status Exit status
 */
void _exit(int status)
{
    (void)status;
    while (1)
    {
        /* Infinite loop */
    }
}

/**
 * @brief Write to a file
 * @param file File descriptor
 * @param ptr Pointer to data
 * @param len Length of data
 * @return Always returns -1 (not implemented)
 */
int _write(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Read from a file
 * @param file File descriptor
 * @param ptr Pointer to buffer
 * @param len Length to read
 * @return Always returns -1 (not implemented)
 */
int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Seek in a file
 * @param file File descriptor
 * @param ptr Offset
 * @param dir Direction
 * @return Always returns -1 (not implemented)
 */
int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Get file status
 * @param file File descriptor
 * @param st Status structure
 * @return Always returns -1 (not implemented)
 */
int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief Get terminal attributes
 * @param file File descriptor
 * @param termios Terminal attributes
 * @return Always returns -1 (not implemented)
 */
int _isatty(int file)
{
    (void)file;
    return 1;
}

/**
 * @brief Allocate memory
 * @param nbytes Number of bytes to allocate
 * @return Pointer to allocated memory or NULL
 */
caddr_t _sbrk(int incr)
{
    extern char end; /* Defined by the linker */
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &end;
    }

    prev_heap_end = heap_end;

    /* Simple heap implementation */
    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

/**
 * @brief Link a file
 * @param old Old path
 * @param new New path
 * @return Always returns -1 (not implemented)
 */
int _link(char *old, char *new)
{
    (void)old;
    (void)new;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Unlink a file
 * @param name File name
 * @return Always returns -1 (not implemented)
 */
int _unlink(char *name)
{
    (void)name;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Open a file
 * @param path File path
 * @param flags Open flags
 * @param mode File mode
 * @return Always returns -1 (not implemented)
 */
int _open(char *path, int flags, ...)
{
    (void)path;
    (void)flags;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Wait for child process
 * @param status Status pointer
 * @return Always returns -1 (not implemented)
 */
int _wait(int *status)
{
    (void)status;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Execute a program
 * @param path Program path
 * @param argv Argument vector
 * @param envp Environment vector
 * @return Always returns -1 (not implemented)
 */
int _execve(char *path, char **argv, char **envp)
{
    (void)path;
    (void)argv;
    (void)envp;
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Fork a process
 * @return Always returns -1 (not implemented)
 */
int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

/**
 * @brief Get system statistics
 * @param name Statistic name
 * @param buf Buffer for statistics
 * @return Always returns -1 (not implemented)
 */
int _stat(char *file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}
