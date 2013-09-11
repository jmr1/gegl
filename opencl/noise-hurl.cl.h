/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 Carlos Zubieta <czubieta.dev@gmail.com>
 */

static const char* noise_hurl_cl_source =
"__kernel void cl_noise_hurl(__global       float4 *src,                       \n"
"                            __global const int    *random_data,               \n"
"                            __global const long   *random_primes,             \n"
"                                           int     x_offset,                  \n"
"                                           int     y_offset,                  \n"
"                                           int     roi_width,                 \n"
"                                           int     whole_region_width,        \n"
"                                           int     seed,                      \n"
"                                           float   pct_random,                \n"
"                                           int     offset)                    \n"
"{                                                                             \n"
"  int gid  = get_global_id(0);                                                \n"
"  int gidy = gid / roi_width;                                                 \n"
"  int gidx = gid - gidy*roi_width;                                            \n"
"                                                                              \n"
"  int x = gidx + x_offset;                                                    \n"
"  int y = gidy + y_offset;                                                    \n"
"  int n = 4 * (x + whole_region_width * y + offset);                          \n"
"                                                                              \n"
"  float4 src_v = src[gid];                                                    \n"
"                                                                              \n"
"  float pc          = gegl_cl_random_float_range (random_data, random_primes, \n"
"                                                  seed, x, y, 0, n, 0, 100);  \n"
"  float red_noise   = gegl_cl_random_float (random_data, random_primes,       \n"
"                                            seed, x, y, 0, n+1);              \n"
"  float green_noise = gegl_cl_random_float (random_data, random_primes,       \n"
"                                            seed, x, y, 0, n+2);              \n"
"  float blue_noise  = gegl_cl_random_float (random_data, random_primes,       \n"
"                                            seed, x, y, 0, n+3);              \n"
"                                                                              \n"
"  if(pc <= pct_random)                                                        \n"
"    {                                                                         \n"
"      src_v.x = red_noise;                                                    \n"
"      src_v.y = green_noise;                                                  \n"
"      src_v.z = blue_noise;                                                   \n"
"    }                                                                         \n"
"  src[gid] = src_v;                                                           \n"
"}                                                                             \n"
;
