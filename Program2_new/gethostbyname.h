// Hugh Smith - April 2017 

// Replacement code for gethostbyname 
// Gives either IPv4 address, IPv6 address or IPv4 mapped IPv6 address
// Works well with sockets of type family AF_INET6                        

#ifndef GETHOSTBYNAME_H
#define GETHOSTBYNAME_H


unsigned char * gethostbyname6(const char * hostName, struct sockaddr_in6 * aSockaddr6);
unsigned char * gethostbyname4(const char * hostName, struct sockaddr_in * aSockaddr);
char * getIPAddressString4(unsigned char * ipAddress);
char * getIPAddressString6(unsigned char * ipAddress);

// Testing functions
void gethostbyname_test();
void gethostbyname_test_lookup(char * hostname);

// Just for printout out address info
void printIPInfo(struct sockaddr_in6 * ipAddressStruct);
char * ipAddressToString(struct sockaddr_in6 * ipAddressStruct);
#endif