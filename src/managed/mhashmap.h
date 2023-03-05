/**
 * Copyright (C) 2023 Amrit Bhogal
 *
 * This file is part of ManagedC.
 *
 * ManagedC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ManagedC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ManagedC.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(MANAGEDC_MHASHMAP)
#define MANAGEDC_MHASHMAP

#include "managed.h"

#if !defined(MC_HASH)
static unsigned long int managed_hash(unsigned char *data, size_t size)
{
    unsigned long int hash = 0;
    size_t i = 0;

    for (i = 0; i < size; i++)
        hash = (hash << 5) - hash + data[i];

    return hash;
}
#   define MC_HASH(data, size) managed_hash((unsigned char *)data, (size_t)size)
#endif

#define mhashmap_t(K, V) struct { K *const key; V *const value; }

static void *managed_hashmap(size_t keysize, size_t valsize,
                             size_t count, managed_Free_f *keyfree,
                             managed_Free_f *valfree, const void *data)
{

    return NULL;
}


#endif
