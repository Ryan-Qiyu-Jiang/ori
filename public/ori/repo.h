/*
 * Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __REPO_H__
#define __REPO_H__

#include <stdint.h>

#include <string>
#include <set>
#include <deque>

#include <oriutil/dag.h>
#include <oriutil/objecthash.h>
#include "tree.h"
#include "commit.h"
#include "object.h"

extern ObjectHash EMPTY_COMMIT;
extern ObjectHash EMPTYFILE_HASH;

typedef std::vector<ObjectHash> ObjectHashVec;

class LargeBlob;

class Repo
{
public:
    Repo();
    virtual ~Repo();

    // Repo information
    virtual std::string getUUID() = 0;
    virtual ObjectHash getHead() = 0;
    virtual int distance() = 0;

    // Objects
    virtual Object::sp getObject(
            const ObjectHash &id
            ) = 0;
    virtual ObjectInfo getObjectInfo(
            const ObjectHash &id
            ) = 0;
    virtual bool hasObject(const ObjectHash &id) = 0;
    virtual std::vector<bool> hasObjects(const ObjectHashVec &ids);
    virtual bytestream *getObjects(
            const ObjectHashVec &objs
            ) = 0;

    // Object queries
    virtual std::set<ObjectInfo> listObjects() = 0;
    virtual std::vector<Commit> listCommits() = 0;

    virtual int addObject(
            ObjectType type,
            const ObjectHash &hash,
            const std::string &payload
            ) = 0;

    // Wrappers
    virtual ObjectHash addBlob(ObjectType type, const std::string &blob);
    bytestream *getObjects(const std::deque<ObjectHash> &objs);

    ObjectHash addSmallFile(const std::string &path);
    std::pair<ObjectHash, ObjectHash>
        addLargeFile(const std::string &path);
    std::pair<ObjectHash, ObjectHash>
        addFile(const std::string &path);

    virtual Tree getTree(const ObjectHash &treeId);
    virtual Commit getCommit(const ObjectHash &commitId);
    virtual LargeBlob getLargeBlob(const ObjectHash &objId);

    // Lookup
    ObjectHash lookup(const Commit &c, const std::string &path);

    // Transport
    virtual void transmit(bytewstream *bs, const ObjectHashVec &objs);
    virtual void receive(bytestream *bs);

    // Extensions
    virtual std::set<std::string> listExt();
    virtual bool hasExt(const std::string &ext);
    virtual std::string callExt(const std::string &ext,
                                const std::string &data);

    // High level operations
    virtual void copyFrom(
            Object *other
            );
    virtual DAG<ObjectHash, Commit> getCommitDag();
};

#endif /* __REPO_H__ */

