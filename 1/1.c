#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BL_SIZE 512

struct filetype {
    const char * typename;
    char ctype;
};

#define MAKE_FILETYPE(description, c) (const struct filetype) {(description), (c)}

struct filetype get_file_type(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:  return MAKE_FILETYPE("block device", 'b');
        case S_IFCHR:  return MAKE_FILETYPE("character device", 'c');
        case S_IFDIR:  return MAKE_FILETYPE("directory", 'd');
        case S_IFIFO:  return MAKE_FILETYPE("FIFO/pipe", 'p');
        case S_IFLNK:  return MAKE_FILETYPE("symbolic link", 'l');
        case S_IFREG:  return MAKE_FILETYPE("regular file", '-');
        case S_IFSOCK: return MAKE_FILETYPE("socket", 's');
        default:       return MAKE_FILETYPE("unknown", '?');
    }
}


void format_mode(char * buf, mode_t mode) {
    char * p = buf;

    // fill buf with info for basic mode bits
    *p++ = get_file_type(mode).ctype;
    for(int i = 8; i >= 0; i--) {
        *p++ = (((int)mode & (1 << i)) ? "xwr"[i % 3] : '-');
    }
    
    *p = '\0';//terminate string

    //0123456789
    //?rwxrwxrwx
    if(mode & S_ISUID) {//проверка на UID
        buf[3] = 's';
    }
    if(mode & S_ISGID) {
        buf[6] = 's';
    }
    if(mode & S_ISUID) {
        buf[9] = S_ISDIR(mode) ? 't' : 'T';
    }        
}

int main(int argc, char *argv[]) {
    
    struct stat sb;
    const char * pathname = argv[1];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (lstat(pathname, &sb) == -1) {  
        perror("lstat");
        exit(1);
    }

   
    printf("File:                     %s\n", pathname);
    printf("Size:                     %ju bytes\n", (uintmax_t) sb.st_size); 
    printf("Blocks:                   %ju (%ju bytes)\n", (uintmax_t) sb.st_blocks, (uintmax_t) sb.st_blocks * BL_SIZE);
    printf("IO Block:                 %ju bytes\n", (uintmax_t) sb.st_blksize);
    printf("File type:                %s\n", get_file_type(sb.st_mode).typename);
    printf("Device ID:                %lxh/%ldd\n", (long) sb.st_dev, (long) sb.st_dev);
    printf("Inode:                    %ju\n", (uintmax_t) sb.st_ino);
    printf("Links:                    %ju\n", (uintmax_t) sb.st_nlink);

    //get filetype and permissions
    char mode_str[sizeof("?rwxrwxrwx")];
    format_mode(mode_str, sb.st_mode);

    printf("Access:                   (%04lo/%s)  UID:%jd  GID:%jd  \n", (unsigned long) (sb.st_mode & ALLPERMS), mode_str, (intmax_t) sb.st_uid, (intmax_t) sb.st_gid);
    printf("Access (UTC):              %s", asctime(gmtime(&sb.st_atime)));
    printf("Modify (UTC):              %s", asctime(gmtime(&sb.st_mtime)));
    printf("Change (UTC):              %s", asctime(gmtime(&sb.st_ctime)));

    exit(0);
}








