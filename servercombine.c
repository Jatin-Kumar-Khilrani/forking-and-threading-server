#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_proc(void *arg);

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    int listensock;
    int newsock;
    pid_t pid;
    int result,x,nchildren=1;
    pthread_t thread_id;
    int val=1;
    listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0) {
        perror("server2");
        return 0;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(listensock, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (result < 0) {
        perror("exserver4");
        return 0;
    }

    result = listen(listensock, 5);
    if (result < 0) {
        perror("exserver4");
        return 0;
    }

	printf("all task comleted before fork\n");
    for (x = 0; x < nchildren; x++)
    {
		if ((pid = fork()) == 0)
		{
			while (1)
			{
				newsock = accept(listensock, NULL ,NULL);
				printf("client connected to child process %i.\n", getpid());
				result = pthread_create(&thread_id, NULL, thread_proc, (void *) newsock);
				if (result != 0)
				{
					  printf("Could not create thread.\n");
					  return 0;
				}
				pthread_detach(thread_id);
				printf("going to yield\n");
				sched_yield();
				printf("client disconnected from child process %i.\n", getpid());
			}
		}
	}

}

void* thread_proc(void *arg)
{
  int sock;
  char buffer[25];
  int nread;
	printf("\n %s: \n",__func__);
  printf("child thread %i with pid %i created.\n", pthread_self(), getpid());
  sock = (int) arg;
  nread = read(sock, buffer, 25);
  buffer[nread] = '\0';
  printf("%s\n", buffer);
  write(sock, buffer, nread);
  close(sock);
  printf("child thread %i with pid %i finished.\n", pthread_self(), getpid());
}

