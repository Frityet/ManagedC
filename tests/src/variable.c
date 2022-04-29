//
// Created by Frityet on 2021-12-23.
//

#include <stdio.h>

#include <string.h>
#include <errno.h>
extern int errno;

#include "managed.h"
#include "logger.h"

int main()
{
    LOG_SUCCESS("Started!");

    auto var num = new(int);
    LOG_ASSERT(num != NULL, "Successfully allocated!", "Could not allocate! Reason: %s", strerror(errno));

    *num = 4;

    LOG_SUCCESS("Done!");
}
