#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void proc_info(const char * procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main(int argc, char *argv[]) {

    printf("%d ", argc);

    for(int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }
    fflush(stdout);// смыв буфера

    pid_t child_id = fork();
    if (child_id < 0) {
        perror("fork");
        return 1;
    }
    //this code is executed in child process only
    if (child_id == 0) {
        //что делает этот код
        if (dup2(fileno(stderr), fileno(stdout)) < 0) {
            perror("dup2");
        }
        proc_info("child");
        return 0;
    }
    //this code is executed in parent process only

    
    proc_info("parent");
    wait(NULL);

    return 0;
}
