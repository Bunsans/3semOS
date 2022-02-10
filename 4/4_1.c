#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s source file target file\n", argv[0]);
        return 1;
    }
    struct stat stat_buf;
    if (lstat(argv[1], &stat_buf) == -1)
    {
        perror("Failed to stat");
        return 3;
    }
    if (!S_ISREG(stat_buf.st_mode))
    {
        printf("Error: Not regular\n");
        return 2;
    }
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1){
        perror("Failed to open source file");
        return 2;
    }
    int dst_fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);//-rw-------
    if (dst_fd == -1){
        perror("Failed to open target file");
        return 2;
    }
#ifdef __APPLE__
    struct timespec am_time[2] = {stat_buf.st_atimespec,stat_buf.st_mtimespec};
#else
    struct timespec am_time[2] = {stat_buf.st_atim,stat_buf.st_mtim};
#endif
    while (1){
        uint8_t buf[4096];
        ssize_t buf_size = read(src_fd, buf, sizeof(buf));

        if (buf_size == -1){
            perror("Failed to read a block");
            if (close(src_fd) == -1) 
                perror("Failed to close");
            if (close(dst_fd) == -1) 
                perror("Failed to close");
        }
        if (buf_size == 0){
            break;
        }
        size_t local_buf_size = (size_t)buf_size;
        size_t bytes_written = 0;
        while (bytes_written < local_buf_size){
            ssize_t write_result = write(dst_fd, &buf[bytes_written], local_buf_size - bytes_written);
        
            if (write_result == -1){
                perror("Failed to write");
                if (close(src_fd) == -1) 
                    perror("Failed to close");
                if (close(dst_fd) == -1) 
                    perror("Failed to close");
                return 3;
            }
            bytes_written += (size_t)write_result;
        }
            
    }
    int err = 0;
    if (fsync(dst_fd) == -1)
    {
        perror("Failed to fsync");
        err = -1;
    }
    if (fchmod(dst_fd, stat_buf.st_mode & ALLPERMS) == -1) //разрешения
    {
        perror("Failed to fchmod");
        err = -1;
    }
    if (futimens(dst_fd, am_time) == -1) 
    {
        perror("Failed to futimens");
        err = -1;
    }
    if (close(src_fd) == -1)
    {
        perror("Failed to close");
        err = -1;
    }
    if (close(dst_fd) == -1)
    {
        perror("Failed to close");
        err = -1;
    }
    return err;
}