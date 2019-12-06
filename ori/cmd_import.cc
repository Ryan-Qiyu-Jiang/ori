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
#include <sys/param.h>

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
#include <ori/repostore.h>

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
int just_in_case_snapshot(const string &exportName);
bool has_srcrepo(const string &srcName);
bool has_branch_local(const string &branchName);
bool has_branch_src(const string &srcFSName, const string &branchName);

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
    LocalRepo srcRepo;

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

    string repoFullPath = OF_ControlPath(); // find dir with control file from cwd
    string controlFileName = ORI_CONTROL_FILENAME;
    repoFullPath = repoFullPath.substr(0, repoFullPath.size()-controlFileName.size()-1);
    if (repoFullPath == dstFullPath.substr(0, repoFullPath.length())) {
        dstRelPath = dstFullPath.substr(repoFullPath.length());
        cout <<"dstRelPath="<<dstRelPath<<endl;
    } else {
        cout<< "Can't find destination directory in this repo."<<endl;
        return 1;
    }

    if(!has_srcrepo(srcFSName)) {
        cout << "Source repo not found." <<endl;
        return 1;
    }
    if(!has_branch_src(srcFSName, branchName)) {
        cout << "Source branch not found." <<endl;
        return 1;
    }
    if(has_branch_local(branchName)) {
        cout << "Branch already exists in this repo, try ori import branch instead?" <<endl;
        return 1;
    }

    cout << "Importing from " << srcFSName <<
         " branch=" << branchName <<
         " to local dir=" << dstFullPath << endl;

    just_in_case_snapshot(branchName);

    ObjectHash importedBranch = repository.import(srcFSName, branchName, dstRelPath);
    if (importedBranch.isEmpty()) {
        printf("Error: Import failed!");
        return 1;
    }

    // checkout new head to see changes
    char * const checkout_argv[] = {"checkout"};
    return cmd_checkout(1, checkout_argv);
}

// makes a snapshot if there are any unsaved changes
// why: if import directory was added but not snapshotted it won't be found when grafting new commit tree
int just_in_case_snapshot(const string &exportName) {

    strwstream req;
    req.writePStr("snapshot");
    req.writeUInt8(1);
    req.writeUInt8(1);
    req.writeLPStr("Just in case snapshot to prepare for import."); // msg
    req.writeLPStr(exportName+"_jic_snapshot"); // name

    strstream resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "Status failed with an unknown error!" << endl;
        return 1;
    }
    return 0;
}

bool has_srcrepo(const string &srcFSName) {
    set<string> repos = RepoStore_GetRepos();
    return repos.count(srcFSName);
}

bool has_branch_local(const string &branchName) {
    uint32_t len;
    strwstream req;
    string currentBranch;

    req = strwstream();
    req.writePStr("branch");
    req.writePStr("list");
    strstream resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "branches failed with an unknown error!" << endl;
        return 0;
    }

    len = resp.readUInt32();
    for (uint32_t i = 0; i < len; i++)
    {
        string branch;

        resp.readLPStr(branch);
        if (branchName == branch) {
            return 1;
        }
    }
    return 0;
}

bool has_branch_src(const string &srcFSName, const string &branchName) {
    LocalRepo srcRepo;
    string dstFullPath;
    string currentRepoRoot = OF_RepoPath();
    string srcRepoRoot = currentRepoRoot.substr(0, currentRepoRoot.rfind("/")) + "/" + srcFSName + ".ori";

    DLOG("rootPath=%s", srcRepoRoot.c_str());

    srcRepo.open(srcRepoRoot);
    set<string> branches = srcRepo.listBranches();

    return branches.count(branchName);
}