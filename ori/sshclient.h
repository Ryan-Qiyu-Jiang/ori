#ifndef __SSHCLIENT_H__
#define __SSHCLIENT_H__

#include "repo.h"

class SshClient
{
public:
    SshClient(const std::string &remotePath);
    ~SshClient();

    int connect();
    void disconnect();
    bool connected();

    // At the moment the protocol is synchronous
    void sendCommand(const std::string &command);
    void recvResponse(std::string &out);

private:
    std::string remoteHost, remoteRepo;

    int fdFromChild, fdToChild;
    int childPid;
};

class SshRepo : public BasicRepo
{
public:
    SshRepo(SshClient *client);
    ~SshRepo();

    std::string getHead();

    int getObjectRaw(Object::ObjectInfo *info, std::string &raw_data);
    std::set<std::string> listObjects();
    Object addObjectRaw(const Object::ObjectInfo &info,
            const std::string &raw_data);
    Object addObjectRaw(Object::ObjectInfo info, bytestream *bs);
    std::string addObject(Object::ObjectInfo info, const std::string &data);

private:
    SshClient *client;
};

#endif
