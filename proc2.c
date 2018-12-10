#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd[2][2];
	pid_t pid;
	char buf[BUFSIZ];
	ssize_t n, m, k;
	int ret, f;

	if (pipe(fd[0]) || pipe(fd[1]))
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
		if (close(fd[0][1]))
		{
			perror("close 0 1");
		}
		if (dup2(fd[0][0], 0) == -1)
		{
			perror("dup2 1 0");
		}
		if (close(fd[0][0]))
		{
			perror("close 0 0");
		}

		if (close(fd[1][0]))
		{
			perror("close 1 1");
		}
		if (dup2(fd[1][1], 1) == -1)
		{
			perror("dup2 0 1");
		}
		if (dup2(fd[1][1], 2) == -1)
		{
			perror("dup2 0 2");
		}
		if (close(fd[1][1]))
		{
			perror("close 1 1");
		}

		ret = execlp("tr", "tr", "a-z", "A-Z", NULL);
		perror("execlp");
		return ret;
	}

	k = sprintf(buf, "Hello World!");
	if (k < 0)
	{
		perror("sprintf");
		return 1;
	}

	for (m = n = 0; m < k; m += n)
	{
		n = write(fd[0][1], buf + m, k - m);
		if (n == -1)
		{
			perror("write 0 1");
			return 1;
		}
	}

	if (close(fd[0][1]))
	{
		perror("close 0 1");
	}

	for (m = n = 0; m < k; m += n)
	{
		n = read(fd[1][0], buf + m, k - m);
		if (n == -1)
		{
			perror("read 1 0");
			return 1;
		}
		if (n == 0) break;
	}

	if (close(fd[1][0]))
	{
		perror("close 1 0");
		return 1;
	}

	printf("%s\n", buf);
	return 0;
}

