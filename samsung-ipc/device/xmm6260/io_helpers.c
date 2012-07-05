/*
 * io_helpers.c - I/O helper functions for the firmware loader
 * This file is part of:
 *
 * Firmware loader for Samsung I9100 and I9250
 * Copyright (C) 2012 Alexander Tarasikov <alexander.tarasikov@gmail.com>
 *
 * based on the incomplete C++ implementation which is
 * Copyright (C) 2012 Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "io_helpers.h"

#define DEFAULT_TIMEOUT 50

int expect(int fd, unsigned timeout) {
    int ret = 0;
    
    struct timeval tv = {
        tv.tv_sec = timeout / 1000,
        tv.tv_usec = 1000 * (timeout % 1000),
    };

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(fd, &read_set);

    ret = select(fd + 1, &read_set, 0, 0, &tv);

    if (ret < 0) {
        // _e("failed to select the fd %d ret=%d: %s", fd, ret, strerror(errno));
        goto fail;
    }

    if (ret < 1 || !FD_ISSET(fd, &read_set)) {
        // _d("fd %d not in fd set", fd);
        goto fail;
    }

fail:
    return ret;
}

int expect_read(int fd, void *buf, size_t size) {
    int ret;
    if ((ret = expect(fd, DEFAULT_TIMEOUT)) < 1) {
        // _e("failed to select the fd %d", fd);
        return ret;
    }

    return read(fd, buf, size);
}

int expect_data(int fd, void *data, size_t size) {
    int ret;
    char buf[size];
    if ((ret = expect_read(fd, buf, size)) != size) {
        ret = -1;
        // _e("failed to receive data");
        return ret;
    }
    ret = memcmp(buf, data, size);

    return ret;
}

// vim:ts=4:sw=4:expandtab
