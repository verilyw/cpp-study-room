# Sokect Network Programming

本节首先会回顾有关Linux/Unix上的套接字网络编程的一些关键概念。套接字编程本质上是操作系统支持的网络通信的支柱。尤其是在C/C++中，如果深入研究任何网络库，最终都会归结为一个关键的东西——Socket。

---

## 什么是套接字？

在简单的定义中，Socket是使用标准Unix文件描述符与其他程序通信的方式。Unix程序通过读/写文件描述符来执行任何类型的I/O，其底层可以是网络链接，管道，真正的磁盘文件等。

有两种类型的Internet套接字：

+ `SOCK_STREAM`，对应于保证可靠的双向字节流传输的TCP协议
+ `SOCK_DGRAM`，对应于UDP协议，速度更快但不可靠。

数据包是通过多层封装在Internet上传输的。

![avatar](https://github.com/YukunJ/Teach-Myself-CPP/raw/main/Socket-Programming/pics/data_encapsulation.png)

理论上，有一个七层的OSI网络模型，但有一个更相关的是这样的：

+ 应用层（telnet，ftp等）
+ 主机到主机的传输层（TCP, UDP）
+ 网络层（IP和路由）
+ 网络访问层（以太网，wifi等）

简而言之，我们需要做的就是为应用层构建我们想要发送的数据，并通过`send（）`将其填充到套接字当中。操作系统为我们构建了传输层和网络层，，而硬件则完成了网络访问层。



## IP地址、结构和数据处理

IP地址有两种：IPv4和IPv6。IPv4由32位组成，如`192.168.0.1`。IPv6的产生是因为IPv4不够用了，它由128位组成。

**子网**是拆分“组织指标”和“主机指标”的方式。如对于IPv4，`192.0.2.12`可能意味着前三个字节`192.0.2.00`是组织指示符，为主机留下1个字节。即该组织中可能有`2^8`个主机。因此，该主机为`12`。

**端口号**是一个16位的数字，有助于多路复用。将IP地址想象成酒店的街道地址，端口号就是酒店的房间号。它始终通过网络来区分同一台主机上运行的不同服务。

**字节序**可能因为不同机器的架构而不同，有小端和大端之分。为了保证程序的兼容性和可移植性，可以使用convert函数：

| Function  | Description           |
| --------- | --------------------- |
| `htons()` | host to network short |
| `htonl()` | host to network long  |
| `ntohs()` | network to host short |
| `ntohl()` | network to host long  |

**套接字描述符只是一个常规的`int`类型**。

`struct addrinfo`用于准备和存储套接字地址结构以供后续使用。

```c
struct addrinfo {
  int 				ai_flags;		// AI_PASSIVE, AI_CANONNAME, etc.
  int 				ai_family;		// AF_INET, AF_INET6, AF_UNSPEC
  int 				ai_socktype;	// SOCK_STREAM, SOCK_DGRAM
  int 				ai_protocol;	// use 0 for "any"
  size_t 			ai_addrlen;		// size of ai_addr in bytes
  struct sockaddr 	*ai_addr;		// struct sockaddr_in or _in6
  char 				*ai_canonname;	// full canonical hostname

  struct addrinfo 	*ai_next;		// linked list, next node
};
```

可以通过调用`getaddrinfo()`来加载它，返回此类结构的链表。

我们看到`ai_addr`是指向`struct sockaddr`的指针，它存储IPv4或IPv6套接字地址信息。

```c
struct sockaddr {
  unsigned short	sa_family;		// address family, AF_INET or AF_INET6
  char				sa_data[14];	// 14 bytes of protocol address
};
```

当我们知道一个IPv4的套接字地址时，我们可以将这个`struct sockaddr *`转换为`struct sockaddr_in*`，其16个字节布局如下：

```c
// IPv4 only
struct sockaddr_in {
  short int				sin_family;		// Address family, AF_INET
  unsigned short int 	sin_port;		// Port number
  struct in_addr		sin_addr;		// Internet address
  unsigned char			sin_zero[8];	// Padding
};
```

注意此处的`sin_port`必须使用`htons`以网络字节序排列。`struct in_addr sin_addr`只是4个字节IPv4地址的最终包装器。

```c
struct in_addr {
  uint32_t s_addr;  // 4 Bytes IPv4 address in network order  
};
```

另外一方面，如果地址是`AF_INET6`IPv6，则存在类似的结构，我们可以使用布局将`struct socket *`转换为`struct sockaddr_in6 *`。

```c
// IPv6 only
struct sockaddr_in6 {
  u_int16_t		sin6_family;	// Address family, AF_INET6
  u_int16_t 		sin6_port;	// Port number
  u_int32_t		sin6_flowinfo;	// IPv6 flow information
  struct in6_addr	sin6_addr;	// IPv6 address
  u_int32_t		sin6_scope_id;	// Scope ID
};

// the final wrapper for 16 bytes IPv6 address
struct in6_addr {
  unsigned char 	s6_addr[16];	// IPv6 address
};
```

将IP地址的字符串表示形式填写到这样的结构中会很乏味，反之亦然。因此，有实用的功能可以帮助我们这样做。

首先，`inet_pton`将字符串IP地址转换为网络形式：

```c
struct sockaddr_in sa;  // IPv4
struct sockaddr_in6 sa6; // IPv6

inet_pton(AF_INET, "10.12.112.57", &(sa.sin_addr)); // IPv4
inet_pton(AF_INET6, "2001:db8:64b3:1::3490", &(sa6.sin6_addr)) // IPv6
```

相反，`inet_ntop`则是将IP地址转换为字符串表示形式。

```c
// IPv4
char ip4[INET_ADDRSTRLEN];
struct sockaddr_in sa;

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
printf("The IPv4 address is: %s\n", ip4);

// IPv6
char ip6[INET6_ADDRSTRLEN];
struct sockaddr_in6 sa6;

inet_pton(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
printf("The IPv6 address is: %s\n", ip6);
```

## System Calls or Bust

1. `getaddrinfo()`

我们将使用此函数进行DNS查找并自动为我们填写必要的套接字结构信息。

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,	// e.g. "www.example.com" or IP
		const char *service,	// e.g. "http" or port number
		const struct addrinfo *hints,
		struct addrinfo **res);
```

`node`参数是连接主机名或IP地址。`service`参数可以是众所周知的服务，如`http`或`ftp`，或者只是直接端口号。`hints`参数指向一个已经填写过相关要求的`struct addrinfo`。然后该函数将使用链接的信息列表填充`res`。

例如，如果你是一台服务器，它使用端口3490来监听自己主机的IP地址：

```c
int status;
struct addrinfo hints;
struct addrinfo *servinfo;		// will point to the results

memset(&hints, 0, sizeof hints);	// make sure the struct is empty
hints.ai_family = AF_UNSPEC;		// don't care IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
  fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
  exit(1);
}

...

freeaddrinfo(servinfo);	// free the linked list
```

2. `socket()`

此函数允许你根据IPv4或IPv6，流或者数据报以及TCP或者UDP来说明你想要那种套接字。

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

domain是`PF_INET`或者`PF_INFO`，传入`AF_INET`或`AF_INET6`即可。`type`是`SOCK_STREAM`或`SOCK_DGRAM`，protocol可以设置为0来为给定的类型选择合适的协议，或者你可以调用函数`getprotobyname()`来查找你想要的协议，要么TCP或者UDP。

在大多数情况下，我们真正需要的不是硬编码，而是传入从`getaddrinfo()`函数获得的信息。

```c
int s;
struct addrinfo hints, *res;

// do the loop up
// assume we have already filled in "hints" struct
getaddrinfo("www.example.com", "http", &hints, &res);

s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

对`socket()`的调用返回一个整数作为套接字描述符，如果出错则返回-1。如果发生错误，全局变量`errno`将设置为错误值。需要在那里做更多的错误处理。

3. `bind()`

一旦我门有了socket描述符，我们可能希望将它与本地机器上的端口想关联。这是必要的，因为我们是服务器，并且将调用`listen()`以获取特定端口上的传入连接。如果我们是即将执行`connect()`的客户，则可能不需要此步骤。

```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```

`sockfd` 是 `socket()` 返回的套接字描述符。 `my_addr` 是指向 `struct sockaddr` 的指针，其中包含我们的端口和 IP 地址。 `addrlen` 是该地址的字节长度。

比如将一个套接字绑定到端口3490上的主机的例子：

```c
struct addrinfo hints, *res;
int sockfd;

// first look up address structs with getaddrinfo()
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;		// use IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;		// fill in my IP for me

getaddrinfo(NULL, "3490", &hints, &res);

// make a socket
sockfd = socket(res->ai_family, res->ai_socktyoe, res->ai_protocol);

// bind it to the port we passed in to getaddrinfo()
bind(sockfd, res->ai_addr, res->ai_addrlen);
```

调用bind时需要注意一件事是，所有低于1024的端口都是为内核保留。作为用户，可以在1024到65535之间进行选择。有时，当想要再次重新运行相同的程序以监听相同的端口是，bind可能会失败，并告诉你“地址已在使用中”，这是因为已连接的套接字可能仍然在内核中徘徊。你可以调用`setsocketopt()`以允许重复使用同一端口。并确保检查bind的返回状态，其中可能设置了`errno`。

4. `connect()`

`connect()`提供了将套接字描述符关联到远程监听器的功能，假设我们是客户端。

```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
```

`sockfd` 是我们想要与之关联的套接字描述符。 `serv_addr` 是指向存储目标 IP 和端口号的 `struct sockaddr` 的指针， `addrlen` 是目标地址结构的字节长度。

我们在端口 3490 上建立到“www.example.com”的套接字连接：

```c
struct addrinfo hints, *res;
int sockfd;

// first, look up destination address struct
memeset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

getaddrinfo("www.example.com", "3490", &hints, &res);

// make a socket
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// connnect
connect(sockfd, res->ai_addr, res->ai_addrlen);
```

作为client端，一旦我们拿到了socket描述符，我们就直接跳转到 `connect()` ，而不是先调用 `bind()` 。因为我们只关心我们要去哪里（远程端口）。内核会为我们选择一个本地端口。

5. `listen()`

如果我们是等待传入连接的服务器，那么我们需要执行两个步骤： `listen()` 和 `accept()` 。 `listen()` 调用非常简单：

```c
int listen(int sockfd, int backlog);
```

特别是 `backlog` 参数。它是传入队列上允许的连接数，因为传入连接将等到您调用 `accept()` ，这是对可以排队的数量的限制。默认情况下，该值在大多数系统上通常为 20。

简而言之，作为服务器，我们需要完成的整个步骤序列是：

+ `getaddrinfo()`
+ `socket()`
+ `bind()`
+ `listen()`
+ `accept()`

6. `accept()`

`accept()` 有点奇怪，它会在每次建立与传入的连接时为服务器提供一个新的 socker 描述符，即同时我们将有两个套接字描述符：一个用于侦听更多传入连接，一个用于与我们刚刚接受的客户。

```c
#inlcude <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

`sockfd` 是 `listen()` ing 套接字描述符。 `addr` 将是指向本地 `struct sockaddr_storage` 的指针，其中将填充有关传入连接的信息。 `addrlen` 是应设置为 `sizeof(struct sockaddr_storage)` 的整数变量。

```c
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "3490"		// the port users will be connecting to
#define BACKLOG 10		// how many pending connections queue will hold

int main() {
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_flags = AI_PASSIVE;	// fill in my IP for me
  
  getaddrinfo(NULL, MYPORT, &hints, &res);

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  bind(sockfd, res->ai_addr, res->ai_addrlen);
  listen(sockfd, BACKLOG);

  addr_size = sizeof their_addr;
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

  // ready to communicate on socket descriptor new_fd
  ...
}
```

7. `send()`和`recv()`

这两个函数让我们通过流套接字或连接的数据报套接字进行通信。 （对于未连接的数据报套接字，请参阅 `sendto()` 和 `recvfrom()` )

```c
int send(int sockfd, const void *msg, int len, int flags);
```

`sockfd` 是我们从作为客户端的 `socket()` 或作为服务器的 `accept()` 获得的套接字描述符。 `msg` 是指向我们要发送的消息的指针， `len` 是以字节为单位的数据长度。在大多数情况下， `flag` 可以设置为 0。

类似地， `recv()` 返回它实际读取并放入 `buf` 中的字节数。

8. `sendto()`和`recvfrom()`

这两个函数是上述两个函数的对等版本，没有连接的套接字。作为没有连接套接字的代价，我们需要在每次调用 `sendto()` 和 `recvfrom()` 时提供/接收此类信息。

9. `close()`和`shutdown()`

我们可以只使用常规的 Unix 文件描述符 `close()` 函数：

``` c
close(sockfd);
```

这将阻止对套接字的任何进一步读取和写入。此类尝试将收到错误。

如果我们想要对套接字的关闭模式进行更细粒度的控制，可以使用shutdown函数：

```c
int shutdown(int sockfd, int how);
```

其中 `sockfd` 是我们要关闭的 socker 文件描述符， `how` 控制如何完成。

| how  | Effect                 |
| ---- | ---------------------- |
| 0    | 不允许进一步接受       |
| 1    | 不允许进一步发送       |
| 2    | 不允许进一步接受和发送 |

10. `getpeername()`

这个简单的函数告诉您有关连接流套接字另一端的信息

```c
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, int *addrlen);
```

`sockfd` 是连接流套接字的描述符， `addr` 是指向 `struct sockaddr` 的指针，将由连接另一端的信息填充， `addrlen` 应该指向已初始化的 `int` 成为 `sizeof(struct sockaddr)` 。一旦我们获得了对方的信息，我们就可以调用`inet_ntop()`，`getnameinfo()`，`gethostbyaddr()`来获取更多信息。

11. `gethostname()`

要检索运行程序的计算机的本地主机名，调用`gethostname()`。结果进一步传递给`gethostbyaddr()`以检索本地机器的本地IP地址。

```c
#include <unistd.h>

int gethostname(char *hostname, size_t size);
```

`hostname` 是指向将用主机名填充的字符数组的指针， `size` 是此字符数组的最大大小。

## 服务端与客户端完整示例

![avatar](https://github.com/YukunJ/Teach-Myself-CPP/raw/main/Socket-Programming/pics/client_server.png)



## 一些先进的技术

1. Blocking

默认情况下，许多套接字函数调用是阻塞的，如`accept()`、`recv()`等，如果我们不希望套接字阻塞，则需要调用`fcntl()`

```c
#include <unistd.h>
#include <fcntl.h>

#include <unistd.h>
#include <fcntl.h>
...
sockfd = socket(AF_INET, SOCK_STREAM, 0);
fcntl(sockfd, F_SETFL, O_NONBLOCK);
...
```

通过将套接字设置为非阻塞，您可以实现轮询功能，因为如果你尝试从没有数据的非阻塞套接字中读取数据，它将返回 `-1` ，并将 `errno` 设置为 `EAGAIN`或者`EWOULDBLOCK`

但是，一般来说，这不是实现轮询的好办法。如果以忙等待的方式编写代码来检查到达的数据，会浪费CPU时间。

2. `poll()`

为了避免轮询使用过多的CPU时间，我们需要调用`poll()`函数。这是一个系统调用，可以让程序休眠以解决CPU时间，并且仅在超时或满足某些预先指定的条件时才唤醒。

一般的策略是保留一个`struct pollfd`数组，其中包含有关我们要监视哪些套接字描述符以及我们需要监视那种事件的信息。特别是当有新的传入连接准备好被`accept()`时，`listen()`ing socket将返回“ready to read"。

```c
#include <poll.h>

struct pollfd {
  int fd;		// the socket descriptor
  short events;		// bitmap of event we want to monitor for
  short revents;	// bitmap of event that actually occurred
};

int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```

`poll` 需要监控的是 `struct pollfd` 的数组， `nfds` 是指监控数组中有多少个描述符， `timeout` 的单位是毫秒。返回时，它返回数组中已发生事件的元素数。

对于 `struct pollfd` ， `fd` 是我们要监视的文件描述符。对于 `events` 和 `revents` ，它们由以下宏描述：

| Macro   | Description                                        |
| ------- | -------------------------------------------------- |
| POLLIN  | 当数据准备好发送到此套接字上的 `recv()` 时提醒我   |
| POLLOUT | 当我可以 `send()` 数据到这个套接字而不阻塞时提醒我 |

3. `select()`

`select()` 实际上与上面的 `poll()` 函数非常相似，它使您能够在不阻塞的情况下监视多个套接字描述符。

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

sturct timeval {
  int tv_sec;	// seconds
  int tv_usec;	// microseconds
};

int select(int numfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout);
```

此函数监视文件描述符集，特别是 `readfds` 、 `writefds` 和 `exceptfds` 。参数 `numfds` 应设置为最高文件描述符的值加一。 `struct timeval` 是超时设置。

要对`fd_set`数据结构进行操作，可以使用以下几个宏：

| Function                        | Description                       |
| ------------------------------- | --------------------------------- |
| `FD_SET(int fd, fd_set *set)`   | Add `fd` to the set               |
| `FD_CLR(int fd, fd_set *set)`   | Remove `fd` from the set          |
| `FD_ISSET(int fd, fd_set *set)` | Return true if `fd` is in the set |
| `FD_ZERO(fd_set *set)`          | Clear all entries from the set    |

有几点需要注意。如果你不关心`select()`中的集合，只需要将其设置为`NULL`即可忽略。可以将`timeout`设置为NULL，这意味着永远不会超时，或者将其设置为0，将有效地轮询集合中的所有文件描述符。

4. 处理部分`send()`s

在 `send()` 部分，我们说过操作系统可能无法发送您需要的所有字节。例如，您要求发送 1000 字节的缓冲区， `send()` 函数返回 500，表示它只发送 500 字节。缓冲区中剩余 500 个字节。我们需要确保发出剩余的：

```c
#include <sys/types.h>
#include <sys/socket.h>

int sendall(int s, char *buf, int *len) {
  int total = 0;		// how many bytes we've sent
  int bytesleft = *len;		// how many we have left to send
  int n;

  while (total < *len) {
    n = send(s, buf + total, bytesleft, 0);
    if (n == -1) break;
    total += n;
    bytesleft -= n;
  }

  *len = total;			// record number actually sent here
  
  return (n == -1) ? -1 : 0;	// return -1 on failure, 0 on success 
}
```

