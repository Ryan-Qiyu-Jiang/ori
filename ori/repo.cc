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

#include <string>
#include <set>
#include <queue>
#include <iostream>

#include "debug.h"
#include "util.h"
#include "object.h"
#include "largeblob.h"

#include "repo.h"
#include "localrepo.h"

using namespace std;


/*
 * Repo
 */

Repo::Repo() {
}

Repo::~Repo() {
}

int Repo::addObject(const ObjectInfo &info,
        const std::string &payload)
{
    assert(false);
    return -1;
}

int Repo::getData(ObjectInfo *info,
        std::string &data) {
    assert(false);
    return -1;
}

/*
 * Add a blob to the repository. This is a low-level interface.
 */
string
Repo::addBlob(Object::Type type, const string &blob)
{
    string hash = Util_HashString(blob);
    ObjectInfo info(hash.c_str());
    info.type = type;
    //info.flags = ...;
    addObject(info, blob);
    return hash;
}

/*
 * Get an object length.
 */
size_t
Repo::getObjectLength(const string &objId)
{
    auto_ptr<BaseObject> o(getObject(objId));
    // XXX: Add better error handling
    if (!o.get())
        return 0;
    return o->getInfo().payload_size;
}

/*
 * Get the object type.
 */
Object::Type
Repo::getObjectType(const string &objId)
{
    auto_ptr<BaseObject> o(getObject(objId));
    if (!o.get()) {
        printf("Couldn't get object %s\n", objId.c_str());
        return Object::Null;
    }
    return o->getInfo().type;
}

Tree
Repo::getTree(const std::string &treeId)
{
    ObjectInfo info(treeId.c_str());
    string blob;
    getData(&info, blob);

    assert(info.type == Object::Tree);

    Tree t;
    t.fromBlob(blob);

    return t;
}

/*
 * High Level Operations
 */

/*
 * Pull changes from the source repository.
 */
void
Repo::pull(Repo *r)
{
    set<string> objects = r->listObjects();
    set<string>::iterator it;

    for (it = objects.begin(); it != objects.end(); it++)
    {
	if (!hasObject(*it)) {
	    // XXX: Copy object without loading it all into memory!
	    //addBlob(r->getPayload(*it), r->getObjectType(*it));
            BaseObject::ap o(r->getObject((*it)));
            if (!o.get()) {
                printf("Error getting object %s\n", (*it).c_str());
                continue;
            }

            std::string raw_data = o->getStoredPayloadStream()->readAll();
            addObjectRaw(o->getInfo(), raw_data);
	}
    }
}

