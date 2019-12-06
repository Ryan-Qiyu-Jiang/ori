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

#include <ori/udsclient.h>
#include <ori/udsrepo.h>
#include <oriutil/runtimeexception.h>
#include <oriutil/orifile.h>

#include "fuse_cmd.h"

using namespace std;

extern UDSRepo repository;

/*
 * Export help
 */
void
usage_extract(void)
{
    cout << "ori extract <Dir Path> <Branch Name>" << endl;
    cout << endl;
    cout << "Export a subtree as a repo branch." << endl;
    cout << endl;
}

int cmd_checkout(int argc, char * const argv[]);
bool has_branch_local(const string &branchName); // defined in cmd_import.cc
int just_in_case_snapshot(const string &exportName);

/*
 * Export a subtree into a new tree with a patched history.
 *
 */
int
cmd_extract(int argc, char * const argv[])
{
    string exportName;
    string srcDirPath, srcRelPath;
    ObjectHash commitTreeClone;

    if (argc != 3) {
        cout << "Error in correct number of arguments." << endl;
        cout << "ori export <Dir Path> <Branch Name>" << endl;
        return 1;
    }
    srcDirPath = argv[1];
    exportName = argv[2];

    string srcFullPath = OriFile_RealPath(srcDirPath);
    if (srcFullPath == "") {
        cout << "Error: Source directory does not exist!" << endl;
        return 1;
    }

    string repoFullPath = OF_ControlPath(); // find dir with control file from cwd
    string controlFileName = ORI_CONTROL_FILENAME;
    repoFullPath = repoFullPath.substr(0, repoFullPath.size()-controlFileName.size()-1);
    if (repoFullPath == srcFullPath.substr(0, repoFullPath.length())) {
        srcRelPath = srcFullPath.substr(repoFullPath.length());
        cout <<"srcRelPath="<<srcRelPath<<", repoPath="<<repoFullPath<<", srcFull="<<srcFullPath<<endl;
    } else {
        cout<< "Can't find source directory in this repo."<<endl;
        return 1;
    }

    // check that exportName is not already a branch
    if(has_branch_local(exportName)) {
        cout << "Branch already exists in this repo, delete branch first?" <<endl;
        return 1;
    }

    cout << "Exporting from " << srcRelPath << " to branch " << exportName << endl;
    just_in_case_snapshot(exportName);

    commitTreeClone = repository.extractSubtree(srcRelPath, exportName);
    cout << "commits Clone Hash="<<commitTreeClone.hex()<<endl;
    if (commitTreeClone.isEmpty()) {
        printf("Error: Could not find a file or directory with this name!");
        return 1;
    }

    // checkout new branch
    char * const checkout_argv[] = {"checkout"};
    return cmd_checkout(1, checkout_argv);

    return 0;
}