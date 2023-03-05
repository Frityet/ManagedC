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

#if !defined(MANAGEDC_MARRAY)
#define MANAGEDC_MARRAY

#include "managed.h"

#if MC_ANSI
#   error "Managed array requires C99 + Extensions!"
#endif

#undef mc_array
#define mc_array(free, ...) (mc_typeof((__VA_ARGS__)[0]) (*)[(sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))])managed_allocate(sizeof((__VA_ARGS__)[0]), (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0])), free, __VA_ARGS__)


#endif
