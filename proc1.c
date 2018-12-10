#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd[2];
	pid_t pid;
	char buf[BUFSIZ];
	ssize_t n, m;
	int ret;

	if (pipe(fd))
	{
		perror("pipe");
		return 0;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 0;
	}

	if (pid == 0)
	{
		if (close(fd[0]))
		{
			perror("close 0");
		}
		if (dup2(fd[1], 1) == -1)
		{
			perror("dup2 1");
		}
		if (close(fd[1]))
		{
			perror("close 1");
		}

		ret = execlp("ls", "ls", NULL);
		perror("execlp");
		return ret;
	}

	if (close(fd[1]))
	{
		perror("close 1");
		return 1;
	}

	for (m = n = 0; 1; m += n)
	{
		size_t const sz = sizeof buf;
		n = read(fd[0], buf + m, sz - m);
		if (n == -1)
		{
			perror("read");
			return 1;
		}
		if (n == 0)
		{
			break;
		}
	}

	if (close(fd[0]))
	{
		perror("close 0");
		return 1;
	}

	printf("%s\n", buf);
	return 0;
}

