static const char* divide_cl_source =
"                                                                              \n"
"/* !!!! AUTOGENERATED FILE generated by math_cl.rb !!!!!                      \n"
" *                                                                            \n"
" * This file is an image processing operation for GEGL                        \n"
" *                                                                            \n"
" * GEGL is free software; you can redistribute it and/or                      \n"
" * modify it under the terms of the GNU Lesser General Public                 \n"
" * License as published by the Free Software Foundation; either               \n"
" * version 3 of the License, or (at your option) any later version.           \n"
" *                                                                            \n"
" * GEGL is distributed in the hope that it will be useful,                    \n"
" * but WITHOUT ANY WARRANTY; without even the implied warranty of             \n"
" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          \n"
" * Lesser General Public License for more details.                            \n"
" *                                                                            \n"
" * You should have received a copy of the GNU Lesser General Public           \n"
" * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.       \n"
" *                                                                            \n"
" * Copyright 2017 Janusz Rupar                                                \n"
" *                                                                            \n"
" * !!!! AUTOGENERATED FILE !!!!!                                              \n"
" */                                                                           \n"
"                                                                              \n"
"#define CHECK(VALUE)(VALUE == 0.0f ? 0.0f : (1.0f / VALUE))                   \n"
"#define FORMULA(INPUT,VALUE)(INPUT * VALUE)                                   \n"
"                                                                              \n"
"__kernel void kernel_divide (__global const float4     *in,                   \n"
"                              __global       float4     *out,                 \n"
"                                             float       value)               \n"
"{                                                                             \n"
"  int id = get_global_id(0);                                                  \n"
"  float val = CHECK(value);                                                   \n"
"  float4 v = (float4)(val, val, val, 1.0f);                                   \n"
"  out[id] = FORMULA(in[id],v);                                                \n"
"}                                                                             \n"
"                                                                              \n"
"__kernel void kernel_divide_aux (__global const float4     *in,               \n"
"                                  __global       float4     *out,             \n"
"                                  __global const float      *aux)             \n"
"{                                                                             \n"
"  int id = get_global_id(0);                                                  \n"
"  float4 v = (float4)(CHECK(aux[id * 3]),                                     \n"
"                      CHECK(aux[id * 3 + 1]),                                 \n"
"                      CHECK(aux[id * 3 + 2]),                                 \n"
"                      1.0f);                                                  \n"
"  out[id] = FORMULA(in[id],v);                                                \n"
"}                                                                             \n"
;
