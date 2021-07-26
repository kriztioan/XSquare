/**
 *  @file   Network.h
 *  @brief  Network Class Definition
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <strings.h>

#include <netdb.h>

#include <sys/errno.h>

#include <ctype.h>

#include <sys/time.h>
#include <time.h>

class Interface;

class Network {

public:
  Network();
  ~Network();

  void networkStart();
  void networkStop();

  void setNetworkPort(unsigned int port);
  void setNetworkIp(const char *ip);

  void networkSync();

protected:
  Interface *interface;

  char *ip;

  unsigned int port;

  int socketfd;

  bool networkOn;

private:
};

#include "Interface.h"

#endif // End of NETWORK_H_
