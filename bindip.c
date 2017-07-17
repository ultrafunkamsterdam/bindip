/* PLEASE READ INSTRUCTIONS BEFORE USING */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <dlfcn.h>

#include <arpa/inet.h>

/* THIS CAN BE OVERRIDDEN LATER /*

#define SRC_ADDR	"123.123.123.123"

#define LIBC_NAME	"libc.so.6" 

#define YES	1
#define NO	0

int
bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen)
{

	struct sockaddr_in src_addr;
	void	*libc;
	int	(*bind_ptr)(int, void *, int);
	int	ret;
	int	passthru;
	char 	*bind_src;

#ifdef DEBUG
	fprintf(stderr, "bind() override called for addr: %s\n", SRC_ADDR);
#endif

	libc = dlopen(LIBC_NAME, RTLD_LAZY);

	if (!libc)
	{
		fprintf(stderr, "Unable to open libc!\n");
		exit(-1);
	}

	*(void **) (&bind_ptr) = dlsym(libc, "bind");

	if (!bind_ptr)
	{
		fprintf(stderr, "Unable to locate bind function in lib\n");
		exit(-1);
	}
	
	passthru = YES;	/* By default, we just call regular bind() */

	if (my_addr==NULL)
	{
	

		passthru = NO;

#ifdef DEBUG
		fprintf(stderr, "bind() Received NULL address.\n");
#endif

	}
	else
	{

		if (my_addr->sa_family == AF_INET)
		{
			struct sockaddr_in	myaddr_in;

			passthru = NO;

			memcpy(&myaddr_in, my_addr, addrlen);

			src_addr.sin_port = myaddr_in.sin_port;


		}
		else
		{
			passthru = YES;
		}

	}

	if (!passthru)
	{

#ifdef DEBUG
		fprintf(stderr, "Proceeding with bind hack\n");
#endif

		src_addr.sin_family = AF_INET;

		bind_src=getenv("BIND_SRC");

		if (bind_src)
		{
			ret = inet_pton(AF_INET, bind_src, &src_addr.sin_addr);
			if (ret<=0)
			{
				/* If the above failed, then try the
				   built in address. */

				inet_pton(AF_INET, SRC_ADDR, 
						&src_addr.sin_addr);
			}
		}
		else
		{
			inet_pton(AF_INET, SRC_ADDR, &src_addr.sin_addr);
		}


		ret = (int)(*bind_ptr)(sockfd, 
					(void *)&src_addr, 
					sizeof(src_addr));
	}
	else
	{

#ifdef DEBUG
		fprintf(stderr, "Calling real bind unmolested\n");
#endif

	
		ret = (int)(*bind_ptr)(sockfd, 
					(void *)my_addr, 
					addrlen);

	}
#ifdef DEBUG
	fprintf(stderr, "The real bind function returned: %d\n", ret);
#endif

	
	dlclose(libc);

	return ret;

}

int
connect(int  sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
{
	int	(*connect_ptr)(int, void *, int);
	void	*libc;
	int	ret;

#ifdef DEBUG
	fprintf(stderr, "connect() override called for addr: %s\n", SRC_ADDR);
#endif

	ret = bind(sockfd, NULL, 0); /* Our fake bind doesn't really need
					those params */

	libc = dlopen(LIBC_NAME, RTLD_LAZY);

	if (!libc)
	{
		fprintf(stderr, "Unable to open libc!\n");
		exit(-1);
	}

	*(void **) (&connect_ptr) = dlsym(libc, "connect");

	if (!connect_ptr)
	{
		fprintf(stderr, "Unable to locate connect function in lib\n");
		exit(-1);
	}


	/* Call real connect function */
	ret = (int)(*connect_ptr)(sockfd, (void *)serv_addr, addrlen);

	/* Clean up */
	dlclose(libc);

	return ret;	

}
