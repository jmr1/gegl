static const char* bilateral_filter_fast_cl_source =
"/* This file is part of GEGL                                                  \n"
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
" * Copyright 2013 Victor Oliveira (victormatheus@gmail.com)                   \n"
" */                                                                           \n"
"                                                                              \n"
"#define GRID(x,y,z) grid[x+sw*(y + z * sh)]                                   \n"
"                                                                              \n"
"__kernel void bilateral_init(__global float8 *grid,                           \n"
"                             int sw,                                          \n"
"                             int sh,                                          \n"
"                             int depth)                                       \n"
"{                                                                             \n"
"  const int gid_x = get_global_id(0);                                         \n"
"  const int gid_y = get_global_id(1);                                         \n"
"                                                                              \n"
"  for (int d=0; d<depth; d++)                                                 \n"
"    {                                                                         \n"
"      GRID(gid_x,gid_y,d) = (float8)(0.0f);                                   \n"
"    }                                                                         \n"
"}                                                                             \n"
"                                                                              \n"
"__kernel void bilateral_downsample(__global const float4 *input,              \n"
"                                   __global       float2 *grid,               \n"
"                                   int width,                                 \n"
"                                   int height,                                \n"
"                                   int sw,                                    \n"
"                                   int sh,                                    \n"
"                                   int   s_sigma,                             \n"
"                                   float r_sigma)                             \n"
"{                                                                             \n"
"  const int gid_x = get_global_id(0);                                         \n"
"  const int gid_y = get_global_id(1);                                         \n"
"                                                                              \n"
"  for (int ry=0; ry < s_sigma; ry++)                                          \n"
"    for (int rx=0; rx < s_sigma; rx++)                                        \n"
"      {                                                                       \n"
"        const int x = clamp(gid_x * s_sigma - s_sigma/2 + rx, 0, width -1);   \n"
"        const int y = clamp(gid_y * s_sigma - s_sigma/2 + ry, 0, height-1);   \n"
"                                                                              \n"
"        const float4 val = input[y * width + x];                              \n"
"                                                                              \n"
"        const int4 z = convert_int4(val * (1.0f/r_sigma) + 0.5f);             \n"
"                                                                              \n"
"        grid[4*(gid_x+sw*(gid_y + z.x * sh))+0] += (float2)(val.x, 1.0f);     \n"
"        grid[4*(gid_x+sw*(gid_y + z.y * sh))+1] += (float2)(val.y, 1.0f);     \n"
"        grid[4*(gid_x+sw*(gid_y + z.z * sh))+2] += (float2)(val.z, 1.0f);     \n"
"        grid[4*(gid_x+sw*(gid_y + z.w * sh))+3] += (float2)(val.w, 1.0f);     \n"
"                                                                              \n"
"        barrier (CLK_GLOBAL_MEM_FENCE);                                       \n"
"      }                                                                       \n"
"}                                                                             \n"
"                                                                              \n"
"#define LOCAL_W 16                                                            \n"
"#define LOCAL_H 16                                                            \n"
"                                                                              \n"
"__attribute__((reqd_work_group_size(16, 16, 1)))                              \n"
"__kernel void bilateral_blur(__global const float8 *grid,                     \n"
"                             __global       float2 *blurz_r,                  \n"
"                             __global       float2 *blurz_g,                  \n"
"                             __global       float2 *blurz_b,                  \n"
"                             __global       float2 *blurz_a,                  \n"
"                             int sw,                                          \n"
"                             int sh,                                          \n"
"                             int depth)                                       \n"
"{                                                                             \n"
"  __local float8 img1[LOCAL_H+2][LOCAL_W+2];                                  \n"
"  __local float8 img2[LOCAL_H+2][LOCAL_W+2];                                  \n"
"                                                                              \n"
"  const int gid_x = get_global_id(0);                                         \n"
"  const int gid_y = get_global_id(1);                                         \n"
"                                                                              \n"
"  const int lx = get_local_id(0);                                             \n"
"  const int ly = get_local_id(1);                                             \n"
"                                                                              \n"
"  float8 vpp = (float8)(0.0f);                                                \n"
"  float8 vp  = (float8)(0.0f);                                                \n"
"  float8 v   = (float8)(0.0f);                                                \n"
"                                                                              \n"
"  float8 k;                                                                   \n"
"                                                                              \n"
"  int x  = clamp(gid_x, 0, sw-1);                                             \n"
"  int y  = clamp(gid_y, 0, sw-1);                                             \n"
"                                                                              \n"
"  for (int d=0; d<depth; d++)                                                 \n"
"    {                                                                         \n"
"      int xp = max(x-1, 0);                                                   \n"
"      int xn = min(x+1, sw-1);                                                \n"
"                                                                              \n"
"      int yp = max(y-1, 0);                                                   \n"
"      int yn = min(y+1, sh-1);                                                \n"
"                                                                              \n"
"      int zp = max(d-1, 0);                                                   \n"
"      int zn = min(d+1, depth-1);                                             \n"
"                                                                              \n"
"      /* the corners are not going to be used, don't bother to load them */   \n"
"                                                                              \n"
"      if (ly == 0) {                                                          \n"
"        k = GRID(x, yp, d);                                                   \n"
"        img1[0][lx+1] = k;                                                    \n"
"        img2[0][lx+1] = k;                                                    \n"
"      }                                                                       \n"
"                                                                              \n"
"      if (ly == LOCAL_H-1) {                                                  \n"
"        k = GRID(x, yn, d);                                                   \n"
"        img1[LOCAL_H+1][lx+1] = k;                                            \n"
"        img2[LOCAL_H+1][lx+1] = k;                                            \n"
"      }                                                                       \n"
"                                                                              \n"
"      if (lx == 0) {                                                          \n"
"        k = GRID(xp, y, d);                                                   \n"
"        img1[ly+1][0] = k;                                                    \n"
"        img2[ly+1][0] = k;                                                    \n"
"      }                                                                       \n"
"                                                                              \n"
"      if (lx == LOCAL_W-1) {                                                  \n"
"        k = GRID(xn, y, d);                                                   \n"
"        img1[ly+1][LOCAL_W+1] = k;                                            \n"
"        img2[ly+1][LOCAL_W+1] = k;                                            \n"
"      }                                                                       \n"
"                                                                              \n"
"      img1[ly+1][lx+1] = GRID(x, y, d);                                       \n"
"                                                                              \n"
"      barrier (CLK_LOCAL_MEM_FENCE);                                          \n"
"                                                                              \n"
"      /* blur x */                                                            \n"
"                                                                              \n"
"      img2[ly+1][lx+1] = (img1[ly+1][lx] + 2.0f * img1[ly+1][lx+1] + img1[ly+1][lx+2]) / 4.0f;\n"
"                                                                              \n"
"      barrier (CLK_LOCAL_MEM_FENCE);                                          \n"
"                                                                              \n"
"      /* blur y */                                                            \n"
"                                                                              \n"
"      v = (img2[ly][lx+1] + 2.0f * img2[ly+1][lx+1] + img2[ly+2][lx+1]) / 4.0f;\n"
"                                                                              \n"
"      /* last three v values */                                               \n"
"                                                                              \n"
"      if (d == 0)                                                             \n"
"        {                                                                     \n"
"          /* this is like CLAMP */                                            \n"
"          vpp = img1[ly+1][lx+1];                                             \n"
"          vp  = img1[ly+1][lx+1];                                             \n"
"        }                                                                     \n"
"      else                                                                    \n"
"        {                                                                     \n"
"          vpp = vp;                                                           \n"
"          vp  = v;                                                            \n"
"                                                                              \n"
"          float8 blurred = (vpp + 2.0f * vp + v) / 4.0f;                      \n"
"                                                                              \n"
"          /* XXX: OpenCL 1.1 doesn't support writes to 3D textures */         \n"
"                                                                              \n"
"          if (gid_x < sw && gid_y < sh)                                       \n"
"            {                                                                 \n"
"              blurz_r[x+sw*(y+sh*(d-1))] = blurred.s01;                       \n"
"              blurz_g[x+sw*(y+sh*(d-1))] = blurred.s23;                       \n"
"              blurz_b[x+sw*(y+sh*(d-1))] = blurred.s45;                       \n"
"              blurz_a[x+sw*(y+sh*(d-1))] = blurred.s67;                       \n"
"            }                                                                 \n"
"        }                                                                     \n"
"    }                                                                         \n"
"                                                                              \n"
"  /* last z */                                                                \n"
"                                                                              \n"
"  vpp = vp;                                                                   \n"
"  vp  = v;                                                                    \n"
"                                                                              \n"
"  float8 blurred = (vpp + 2.0f * vp + v) / 4.0f;                              \n"
"                                                                              \n"
"  if (gid_x < sw && gid_y < sh)                                               \n"
"    {                                                                         \n"
"      blurz_r[x+sw*(y+sh*(depth-1))] = blurred.s01;                           \n"
"      blurz_g[x+sw*(y+sh*(depth-1))] = blurred.s23;                           \n"
"      blurz_b[x+sw*(y+sh*(depth-1))] = blurred.s45;                           \n"
"      blurz_a[x+sw*(y+sh*(depth-1))] = blurred.s67;                           \n"
"    }                                                                         \n"
"}                                                                             \n"
"                                                                              \n"
"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;\n"
"                                                                              \n"
"__kernel void bilateral_interpolate(__global    const float4    *input,       \n"
"                                    __read_only       image3d_t  blurz_r,     \n"
"                                    __read_only       image3d_t  blurz_g,     \n"
"                                    __read_only       image3d_t  blurz_b,     \n"
"                                    __read_only       image3d_t  blurz_a,     \n"
"                                    __global          float4    *smoothed,    \n"
"                                    int   width,                              \n"
"                                    int   s_sigma,                            \n"
"                                    float r_sigma)                            \n"
"{                                                                             \n"
"  const int x = get_global_id(0);                                             \n"
"  const int y = get_global_id(1);                                             \n"
"                                                                              \n"
"  const float  xf = (float)(x) / s_sigma;                                     \n"
"  const float  yf = (float)(y) / s_sigma;                                     \n"
"  const float4 zf = input[y * width + x] / r_sigma;                           \n"
"                                                                              \n"
"  float8 val;                                                                 \n"
"                                                                              \n"
"  val.s04 = (read_imagef (blurz_r, sampler, (float4)(xf, yf, zf.x, 0.0f))).xy;\n"
"  val.s15 = (read_imagef (blurz_g, sampler, (float4)(xf, yf, zf.y, 0.0f))).xy;\n"
"  val.s26 = (read_imagef (blurz_b, sampler, (float4)(xf, yf, zf.z, 0.0f))).xy;\n"
"  val.s37 = (read_imagef (blurz_a, sampler, (float4)(xf, yf, zf.w, 0.0f))).xy;\n"
"                                                                              \n"
"  smoothed[y * width + x] = val.s0123/val.s4567;                              \n"
"}                                                                             \n"
;