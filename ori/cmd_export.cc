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

#include <string>
#include <iostream>
#include <iomanip>

#include <oriutil/debug.h>
#include <oriutil/orifile.h>
#include <ori/localrepo.h>

#include "fuse_cmd.h"

using namespace std;

extern LocalRepo repository;

/*
 * Export help
 */
void
usage_export(void)
{
    cout << "ori export <Dir Path> <Export Name>" << endl;
    cout << endl;
    cout << "Export a subtree from a repository." << endl;
    cout << endl;
}

/*
 * Export a subtree into a new tree with a patched history.
 *
 * It just does extract rn
 */

int cmd_extract(int argc, char * const argv[]);

int
cmd_export(int argc, char * const argv[])
{
    string dest_fs_name;
    string srcRoot, dstRoot, srcRelPath;
    string tmpDirPath;

    if (argc != 3) {
        cout << "Error in correct number of arguments." << endl;
        cout << "ori export <Dir Path> <New Ori FS Name>" << endl;
        return 1;
    }

    return cmd_extract(argc, argv);
}
