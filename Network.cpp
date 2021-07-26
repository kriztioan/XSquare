/**
 *  @file   Network.cpp
 *  @brief  Network Class Implementation
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-26
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "Network.h"

Network::Network() {

  Network::interface = NULL;
  Network::networkOn = false;
  Network::ip = NULL;
  Network::port = -1;
}

Network::~Network() {

  if (Network::networkOn)
    Network::networkStop();

  if (Network::ip != NULL)
    delete Network::ip;
}

void Network::networkStart() {

  if (Network::interface->messagesOn)
    fprintf(Network::interface->messageFile, "Network.........starting\n");

  struct sockaddr_in remote, local;

  local.sin_family = AF_INET;
  local.sin_port = htons(0);
  local.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(local.sin_zero), '\0', 8);

  remote.sin_family = AF_INET;
  remote.sin_port = htons(Network::port);
  if (NULL == Network::ip || inet_aton(Network::ip, &(remote.sin_addr)) == 0) {
    if (Network::interface->messagesOn)
      fprintf(Network::interface->messageFile, "    Connection..invalid IP\n");
    return;
  }
  memset(&(remote.sin_zero), '\0', 8);

  if ((Network::socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    if (Network::interface->messagesOn)
      fprintf(Network::interface->messageFile,
              "    Connection..failed to open socket\n");
    return;
  }

  int yes = 1;
  if (setsockopt(Network::socketfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                 sizeof(int)) == -1) {
    if (Network::interface->messagesOn)
      fprintf(Network::interface->messageFile,
              "    Connection..failed to set socketopt\n");
    return;
  }

  if (bind(Network::socketfd, (struct sockaddr *)&local,
           sizeof(struct sockaddr)) == -1) {
    if (Network::interface->messagesOn)
      fprintf(Network::interface->messageFile,
              "    Connection..failed to bind port\n");
    return;
  }

  if (connect(Network::socketfd, (struct sockaddr *)&remote,
              sizeof(struct sockaddr)) == -1) {
    if (Network::interface->messagesOn)
      fprintf(Network::interface->messageFile,
              "    Connection..failed to connect\n");
    return;
  }

  if (Network::interface->messagesOn)
    fprintf(Network::interface->messageFile,
            "    Connection..connection to %s on port %d opened\n",
            inet_ntoa(remote.sin_addr), ntohl(remote.sin_port));

  Network::networkOn = true;
}

void Network::networkStop() {

  close(Network::socketfd);

  Network::networkOn = false;

  if (Network::interface->messagesOn)
    fprintf(Network::interface->messageFile, "Network.........stopped\n");
}

void Network::setNetworkPort(unsigned int port) {

  if (Network::interface->messagesOn)
    fprintf(Network::interface->messageFile,
            "    Connection..setting port to %d\n", port);

  Network::port = port;
}

void Network::setNetworkIp(const char *ip) {

  if (Network::interface->messagesOn)
    fprintf(Network::interface->messageFile,
            "    Connection..setting ip to %s\n", ip);

  if (Network::ip != NULL)
    delete Network::ip;

  Network::ip = strcpy(new char[strlen(ip) + 1], ip);
}

void Network::networkSync(void) {}
