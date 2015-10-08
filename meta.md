===================================================
struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};

get's filled by getaddrinfo : a linked list of addinfos

====================================================
// IP address informations
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET(IPV4)
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr -> should be set to zero by memset
};

settings sin_addr : inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4 ( -1 error 0 messed up so should be greater than 0)
old alternatives : inet_addr() and inet_aton()

sin_addr ip address to string:

	 char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
	 struct sockaddr_in sa;      // pretend this is loaded with something
	 inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	 printf("The IPv4 address is: %s\n", ip4);

======================================================
struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
=====================================================

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints, // that addinfo that we should have filled previously
                struct addrinfo **res);
