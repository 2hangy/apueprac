#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#define ERRSTR (strerror(errno))

void cat(const char * file_path) {
	int fd = -1;
	char * data = NULL;
	do {
		fd = open(file_path, O_RDONLY);
		if (fd == -1) {
			printf("open file %s fail\n", file_path);
			break;
		}
		off_t offset = lseek(fd, 0, SEEK_END);
		if (offset == -1) {
			printf("lseek to file end fail\n");
			break;
		}
		size_t size = (size_t)offset;
		if (size == 0) {
			printf("file size is 0\n");
			break;
		}
		lseek(fd, 0, SEEK_SET);
		data = (char*)malloc(size);
		if (data == NULL) {
			printf("malloc memory fail\n");
			break;
		}
		ssize_t read_size = read(fd, data, size);
		if (read_size == -1) {
			printf("read fail\n");
			break;
		}
		if (read_size != (ssize_t)size) {
			printf("only read %zu bytes(total %zd bytes)", read_size, size);
		}
		printf("data is:\n%s\n", data);
	} while (0);
	if (data != NULL) {
		free(data);
	}
	if (fd != -1) {
		close(fd);
	}
}

void ls(const char * path) {
	DIR * dp = NULL;
	struct dirent * dirp = NULL;
	do {
		dp = opendir(path);
		if (dp == NULL) {
			printf("open dir %s fail\n", path);
			break;
		}
		while ((dirp = readdir(dp)) != NULL) {
			printf("%s ", dirp->d_name);
		}
		printf("\n");
	} while (0);
	if (dp != NULL) {
		closedir(dp);
	}
}

void process_ctl() {
	printf("pid is %d\n", getpid());
	// fork exec waitpid
	pid_t pid = 0;
	if ( (pid = fork()) == -1) {
		printf("fork fail %s\n", ERRSTR);
	} else if (pid == 0) {
		printf("child pid is %d\n", getpid());
		sleep(5);
		execlp("pwd", "pwd", 0);
		printf("should't enter here\n");
		exit(127);
	}
	printf("parent, child pid is %d\n", pid);
	int status = 0;
	waitpid(pid, &status, 0);
	printf("wait end %d\n", status);
}

void sig_int(int signo) {
	printf("enter here %s\n %d", __FUNCTION__, signo);
	exit(0);
}

void test_signal() {
	printf("%d %d\n", getuid(), getgid());
	printf("--%ld--\n", sysconf(_SC_CLK_TCK));
	if (signal(SIGINT, sig_int) == SIG_ERR) {
		printf("register sigint fail\n");
	} else {
		sleep(1000);
	}
}

int main() {
	// const char * file_path = "/etc/passwd";
	// cat(file_path);
	// ls("/a");
	// process_ctl();
	// test_signal();
	return 0;
}
