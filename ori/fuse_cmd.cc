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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>

#include <ori/oriutil.h>

#include "fuse_cmd.h"

std::string OF_ControlPath()
{
    char *cwdbuf = getcwd(NULL, 0);
    std::string path = cwdbuf;
    free(cwdbuf);

    while (path.size() > 0) {
        std::string control_path = path + "/" + ORI_CONTROL_FILENAME;
        if (Util_FileExists(control_path)) {
            return control_path;
        }
        path = StrUtil_Dirname(path);
    }

    return "";
}

bool OF_RunCommand(const char *cmd)
{
    std::string controlPath = OF_ControlPath();
    if (controlPath.size() == 0)
        return false;

    int fd = open(controlPath.c_str(), O_RDWR | O_TRUNC);
    if (fd < 0) {
        perror("OF_RunCommand open");
        return false;
    }

    write(fd, cmd, strlen(cmd));
    fsync(fd);
    lseek(fd, 0, SEEK_SET);

    // Read output
    struct stat sb;
    fstat(fd, &sb);
    size_t left = sb.st_size;
    while (left > 0) {
        size_t to_read = left;
        if (to_read > 1024)
            to_read = 1024;

        char buf[1024];
        read(fd, buf, to_read);
        fwrite(buf, to_read, 1, stdout);
        left -= to_read;
    }

    return true;
}
