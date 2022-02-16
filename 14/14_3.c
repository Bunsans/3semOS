#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

// last | wc -l

int main(void)
{
	// create pipe
	int pipe_fds[2]; //нулевой элемент -- откуда читаем, первый -- куда пишем
	if (pipe(pipe_fds) < 0)
	{
		perror("failed to create pipe");
		return 1;
	}

	pid_t child_id = fork(); // при создании дочернего процесса копируется информация о дескрипторах
	if (child_id < 0)
	{
		perror("fork");
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return 1;
	}

	// находимся в ребенке
	if (child_id == 0)
	{
		// закрываем чтение(выход)
		close(pipe_fds[0]);
		// копируем дескриптор pipe[1] в stdout
		if (dup2(pipe_fds[1], fileno(stdout)) < 0)
		{
			perror("dup2");
			close(pipe_fds[1]);
			return 1;
		}
		close(pipe_fds[1]); // закрываем вход pipe, тк уже скопировали дескриптор входа в pipe в stdout
							// и тем самым освобождаем дескриптор

		execlp(
			// filename to execute
			"last",
			// argv[0], argv[1], ...
			"last", NULL);
		//если выполняется этот код, значит что то пошло не так, тк execlp в конце вызывает exit
		perror("execlp");
		return 1;
	}

	// находимся в родительском процессе
	close(pipe_fds[1]); // закрываем вход, иначе можем зависнуть когда будем считывать(пайпа будет ждать что что-то придет, при этом в этом процессе ничего не записывается)
	// копируем дескриптор выхода pipe в stdin
	if (dup2(pipe_fds[0], fileno(stdin)) < 0)
	{
		perror("dup2");
		close(pipe_fds[0]);
		return 1;
	}
	close(pipe_fds[0]); // закрываем выход pipe, тк уже сохранили дескриптор в stdin
						// и тем самым освобождаем дескриптор
	// execute 'wc -l'
	execlp(
		// filename to execute
		"wc",
		// argv[0], argv[1], ...
		"wc", "-l", NULL);

	perror("failed to exec 'wc -l'");

	return 1;
}