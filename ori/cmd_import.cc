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

#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#include <ori/udsclient.h>
#include <ori/udsrepo.h>
#include <oriutil/runtimeexception.h>

#include <oriutil/debug.h>
#include <oriutil/orifile.h>
#include <ori/localrepo.h>
#include <dirent.h>

#include "fuse_cmd.h"

using namespace std;

extern UDSRepo repository;

/*
 * Import help
 */
void
usage_import(void)
{
    cout << "ori import <Ori FS Name> <Branch Name> <Local Dir Path>" << endl;
    cout << endl;
    cout << "Import repository as a subtree." << endl;
    cout << endl;
}

int cmd_checkout(int argc, char * const argv[]);
int just_in_case_snapshot(string exportName);
/*
 * Import a subtree with history.
 *
 *
 *   ori import <Ori FS Name> <Branch Name> <Local Dir Path>
 *       import to new branch
 *       import from branch to dir
 *       checkout new head
 */
int
cmd_import(int argc, char * const argv[])
{
    string dstDirPath, dstRelPath;
    string srcFSName, branchName;

    if (argc != 4) {
        cout << "Error in correct number of arguments." << endl;
        cout << "ori import <Ori FS Name> <Branch Name> <Local Dir Path>" << endl;
        return 1;
    }
    srcFSName = argv[1];
    branchName = argv[2];
    dstDirPath = argv[3];

    string dstFullPath = OriFile_RealPath(dstDirPath);
    if (dstFullPath == "") {
        cout << "Error: destination directory does not exist!" << endl;
        return 1;
    }
    string srcRepoRootPath = OF_RepoPath(srcFSName); // .ori/fs_name.ori/
    if (srcRepoRootPath == "") {
        cout << "No src repository found..." << endl;
        return 1;
    }

    cout << "importing from " << srcFSName <<
         " branch=" << branchName <<
         " to local dir=" << dstFullPath << endl;


    dstRelPath = dstDirPath;
    // check srcFSName is a real thing
    // check branch is a real thing
    // check branchName isn't taken

    // make sure repo is snapshotted so import dir is found
    just_in_case_snapshot(branchName);

    ObjectHash importedBranch = repository.import(srcFSName, branchName, dstRelPath);
    if (importedBranch.isEmpty()) {
        printf("Error: Could not find a file or directory with this name!");
        return 1;
    }

    // checkout new head to see changes
    char * const checkout_argv[] = {"checkout"};
    return cmd_checkout(1, checkout_argv);
}

int just_in_case_snapshot(string exportName) {

    strwstream req;
    req.writePStr("snapshot");
    req.writeUInt8(1);
    req.writeUInt8(1);
    req.writeLPStr("Just in case snapshot to prepare for import."); // msg
    req.writeLPStr(exportName+"_jic_snapshot"); // name

    strstream resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "status failed with an unknown error!" << endl;
        return 1;
    }
}