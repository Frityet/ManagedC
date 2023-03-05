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

#include "managed/marray.h"
#include "test.h"

#define $(...) mc_typeof(mc_typeof(__VA_ARGS__) *)

#define k

declaretest(marray)
{
    int i = 0;
    scanf("%d", &i);
    $(int[10]) o = mc_array(NULL, (int[i]) {});

    ASSERT((*o)[0] == 1, "Value did not set!");

    return success;
}
