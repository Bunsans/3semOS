#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define BUF_SIZE (1024*1024)

int main(int argc, char const *argv[])
{
    int result = 0;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s src dest\n", argv[0]);
        return 1;
    }

    struct stat sb;
    if (lstat(argv[1], &sb) == -1)
    {
        perror("lstat failed");
        return 2;
    }

    if (!S_ISREG(sb.st_mode)) // проверка на регулярность
    {
        fprintf(stderr, "Not a regular file\n");
        return 3;
    }

    int fd1 = open(argv[1], O_RDONLY); // открываем первый файл
    if (fd1 < 0)
    {
        perror("Failed to open src file");
        return 4;
    }
    // 0644 - r/w for user, read-only for group and others
    int fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644); // открываем второй
    if (fd2 < 0)
    {
        perror("Failed to open or create dest file");
        close(fd1);
        return 5;
    }

    char buf[BUF_SIZE];
    ssize_t nbytes, nbytes_w;

    while ((nbytes = read(fd1, buf, sizeof(buf))) > 0) // считываем sb.st_size байтов из fd1 начиная с &buf + offset_r
    {
        off_t offset_w = 0;
        while (nbytes)
        {
            nbytes_w = write(fd2, buf + offset_w, (size_t)nbytes); // записываем nbytes байтов в fd2 начиная с &buf + offset_w
            if (nbytes_w == -1)
            {
                close(fd1);
                close(fd2);
                perror("pwrite failed");
                return 6;
            }
            nbytes -= nbytes_w;   // уменьшаем количество байтов которые нужно записать
            offset_w += nbytes_w; // увеличиваем смещение
        }
    }

    if (nbytes == -1) // обработка ошибки read
    {
        perror("read failed");
        result = 7;
    }

    // закрываем дескрипторы
    if (close(fd1) < 0)
    {
        perror("Closing src file failed");
        result = 8;
    }

    if (close(fd2) < 0)
    {
        perror("Closing dest file failed");
        result = 9;
    }

    return result;
}