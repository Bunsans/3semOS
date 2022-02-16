
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(void)
{
    pid_t child_id = fork();
    if (child_id < 0)
    {
        perror("fork");
        return 1;
    }

    //находимся в ребенке
    if (child_id == 0)
    {
        printf("Child process ");
        pid_t child = getpid();
        printf("PID = %d\n", child);
        sleep(180);
    }
    else // находимся в родителе
    {
        int status = 0;
        if (waitpid(child_id, &status, WUNTRACED | WCONTINUED) < 0) // родительский процесс ждет изменений в дочернем
        {                                                           // also return if a child has stopped | also return if a stopped child has been resumed by delivery of SIGCONT
            perror("waitpid");
            return 2;
        }
        while (WIFEXITED(status) == 0 && WIFSIGNALED(status) == 0)
        { // returns true if the child terminated normally && returns true if the child process was terminated by a signal
            if (WIFSTOPPED(status))
            {                                                                                         // returns  true  if the child process was stopped by delivery of a signal
                printf("Child with PID = %d was stopped by signal %d\n", child_id, WSTOPSIG(status)); // returns the number of the signal which caused the child to stop.
            }
            if (WIFCONTINUED(status))
            {
                printf("Child with PID = %d continue to work\n", child_id);
            }
            if (waitpid(child_id, &status, WUNTRACED | WCONTINUED) < 0)
            {
                perror("waitpid");
                return 3;
            }
        }

        if (WIFEXITED(status)) // returns true if the child terminated normally, that is, by calling exit(3) or _exit(2), or by returning from main()
        {
            printf("Child process with PID = %d exited with code %d\n", child_id, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            int child_signal = WTERMSIG(status); // returns the number of the signal that caused the child process to terminate
            printf("Child with PID = %d was killed by signal %d\n", child_id, child_signal);
            if (WCOREDUMP(status))
            { // returns true if the child produced a core dump
                printf("core dump\n");
            }
        }
    }
    return 0;
}

/*
SIGINT	 2	        Term	Прерывание с клавиатуры
SIGQUIT	 3	        Core	Прекратить работу с клавиатурой
SIGILL	 4	        Core	Некорректная инструкция от процессора
SIGABRT	 6	        Core	Сигнал о прекращении, посланный abort(3)
SIGFPE	 8	        Core	Неправильная операция с "плавающей" запятой
SIGKILL	 9	        Term	Сигнал Kill
SIGSEGV	11	        Core	Некорректное обращение к памяти
SIGPIPE	13	        Term	Запись в канале, не имеющем считывающих процессов
SIGALRM	14	        Term	Сигнал таймера от alarm(2)
SIGTERM	15	        Term	Сигнал снятия
SIGUSR1	30,10,16	Term	Определяемый пользователем сигнал #1
SIGUSR2	31,12,17	Term	Определяемый пользователем сигнал #2
SIGCHLD	20,17,18	Ign	    Дочерний процесс остановлен или прерван
SIGCONT	19,18,25	        Продолжить в случае остановки
SIGSTOP	17,19,23	Stop	Процесс остановлен
SIGTSTP	18,20,24	Stop	Остановка с помощью клавиатуры
SIGTTIN	21,21,26	Stop	Запрос на ввод с терминала для фонового процесса
SIGTTOU	22,22,27	Stop	Запрос на вывод с терминала для фонового процесса

Сигналы SIGKILL и SIGSTOP не могут быть пойманы, блокированы или проигнорированы.
*/
