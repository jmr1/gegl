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
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */
#if GEGL_CHANT_PROPERTIES

#else

#define GEGL_CHANT_FILTER
#define GEGL_CHANT_NAME            nop
#define GEGL_CHANT_DESCRIPTION     "Passthrough operation. Used mostly for routing/place holder purposes internally in GEGL."
#define GEGL_CHANT_SELF            "nop.c"
#define GEGL_CHANT_CLASS_INIT
#include "gegl-chant.h"

static gboolean
process (GeglOperation *operation,
         GeglNodeContext *context,
         const GeglRectangle *process)
{
  gboolean success = FALSE;
  GeglBuffer *buffer = gegl_node_context_get_source (context, "input");
  if (buffer)
    {
      g_object_ref (buffer);
      gegl_node_context_set_object (context, "output", G_OBJECT (buffer));
      success = TRUE;
    } 
  return success;
}

static GeglNode *
detect (GeglOperation *operation,
        gint           x,
        gint           y)
{
  GeglNode *input_node;

  const gchar *name = gegl_node_get_name (operation->node);
  if (name && !strcmp (name, "proxynop-output"))
    {
      GeglNode *graph;
      graph = g_object_get_data (G_OBJECT (operation->node), "graph");
      input_node = gegl_operation_detect (graph->operation, x, y);
      if (input_node)
        return input_node;
    }
 
  input_node = gegl_operation_get_source_node (operation, "input");

  if (input_node)
    return gegl_operation_detect (input_node->operation, x, y);
  return operation->node;
}

static void class_init (GeglOperationClass *operation_class)
{
  operation_class->detect = detect;
  operation_class->no_cache = TRUE;
}

#endif
