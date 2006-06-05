/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2003 Calvin Williamson
 *           2005, 2006 Øyvind Kolås
 */

#include "config.h"

#include <glib-object.h>
#include <string.h>

#include "gegl-types.h"
#include "gegl-operation.h"
#include "gegl-node.h"
#include "gegl-pad.h"


enum
{
  PROP_0
};

static void      gegl_operation_class_init (GeglOperationClass    *klass);
static void      gegl_operation_init       (GeglOperation         *self);
static GObject * constructor               (GType                  type,
                                            guint                  n_props,
                                            GObjectConstructParam *props);
static void      finalize                  (GObject               *gobject);
static void      set_property              (GObject               *gobject,
                                            guint                  prop_id,
                                            const GValue          *value,
                                            GParamSpec            *pspec);
static void      get_property              (GObject               *gobject,
                                            guint                  prop_id,
                                            GValue                *value,
                                            GParamSpec            *pspec);
static void      associate                 (GeglOperation         *self);

static gboolean calc_have_rect (GeglOperation *self);
static gboolean calc_need_rect (GeglOperation *self);
static gboolean calc_comp_rect (GeglOperation *self);
static gboolean calc_result_rect (GeglOperation *self);

G_DEFINE_TYPE (GeglOperation, gegl_operation, G_TYPE_OBJECT)

static void
gegl_operation_class_init (GeglOperationClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->constructor  = constructor;
  gobject_class->finalize     = finalize;
  gobject_class->set_property = set_property;
  gobject_class->get_property = get_property;
  
  klass->associate = associate;
  klass->calc_have_rect = calc_have_rect;
  klass->calc_need_rect = calc_need_rect;
  klass->calc_comp_rect = calc_comp_rect;
  klass->calc_result_rect = calc_result_rect;
}

static void
gegl_operation_init (GeglOperation *self)
{
  self->name = NULL;
}

static void
finalize (GObject *gobject)
{
  GeglOperation *self = GEGL_OPERATION (gobject);

  if (self->name)
    g_free (self->name);

  G_OBJECT_CLASS (gegl_operation_parent_class)->finalize (gobject);
}

static GObject*
constructor (GType                  type,
             guint                  n_props,
             GObjectConstructParam *props)
{
  GObject    *object;
  GeglOperation *self;

  object = G_OBJECT_CLASS (gegl_operation_parent_class)->constructor (type,
                                                                   n_props,
                                                                   props);

  self = GEGL_OPERATION (object);

  self->constructed = TRUE;

  return object;
}

static void
set_property (GObject      *gobject,
              guint         property_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  GeglOperation *object = GEGL_OPERATION (gobject);

  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
get_property (GObject      *gobject,
              guint         property_id,
              GValue       *value,
              GParamSpec   *pspec)
{
  GeglOperation *object = GEGL_OPERATION (gobject);

  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

/**
 * gegl_operation_create_pad:
 * @self: a #GeglOperation.
 * @param_spec:
 *
 * Create a property.
 **/
void
gegl_operation_create_pad (GeglOperation *self,
                           GParamSpec    *param_spec)
{
  GeglPad *pad;

  g_return_if_fail (GEGL_IS_OPERATION (self));
  g_return_if_fail (param_spec);

  if (!self->node)
    {
      g_warning ("gegl_operation_create_pad aborting, no associated node, this"
                 " should be called after the operation is associated with a specific node.");
      return;
    }

  pad = g_object_new (GEGL_TYPE_PAD, NULL);
  gegl_pad_set_param_spec (pad, param_spec);
  gegl_pad_set_node (pad, self->node);
  gegl_node_add_pad (self->node, pad);
} 

gboolean
gegl_operation_evaluate (GeglOperation *self,
                         const gchar   *output_pad)
{
  GeglOperationClass *klass;

  g_return_val_if_fail (GEGL_IS_OPERATION (self), FALSE);

  klass = GEGL_OPERATION_GET_CLASS (self);

  return klass->evaluate (self, output_pad);
}

gboolean
gegl_operation_calc_have_rect (GeglOperation *self)
{
  GeglOperationClass *klass;

  klass = GEGL_OPERATION_GET_CLASS (self);
  if (klass->calc_have_rect)
    return klass->calc_have_rect (self);
  return FALSE;
}

gboolean
gegl_operation_calc_need_rect (GeglOperation *self)
{
  GeglOperationClass *klass;

  klass = GEGL_OPERATION_GET_CLASS (self);

  if (klass->calc_need_rect)
    return klass->calc_need_rect (self);
  return FALSE;
}

gboolean
gegl_operation_calc_comp_rect (GeglOperation *self)
{
  GeglOperationClass *klass;

  klass = GEGL_OPERATION_GET_CLASS (self);

  if (klass->calc_comp_rect)
    return klass->calc_comp_rect (self);
  return FALSE;
}

gboolean
gegl_operation_calc_result_rect (GeglOperation *self)
{
  GeglOperationClass *klass;

  klass = GEGL_OPERATION_GET_CLASS (self);

  if (klass->calc_result_rect)
    return klass->calc_result_rect (self);
  return FALSE;
}

#include <stdio.h>

static void
associate (GeglOperation *self)
{
  fprintf (stderr, "kilroy was at What The Hack (%p)\n", (void*)self);
  return;
}

void
gegl_operation_associate (GeglOperation *self,
                          GeglNode      *node)
{
  GeglOperationClass *klass;

  g_return_if_fail (GEGL_IS_OPERATION (self));
  g_return_if_fail (GEGL_IS_NODE (node));

  klass = GEGL_OPERATION_GET_CLASS (self);

  g_assert (klass->associate);
  self->node = node;
  klass->associate (self);
}

void
gegl_operation_set_have_rect (GeglOperation *operation,
                              gint           x,
                              gint           y,
                              gint           width,
                              gint           height)
{
  g_assert (operation);
  g_assert (operation->node);
  gegl_node_set_have_rect (operation->node, x, y, width, height);
}

GeglRect *
gegl_operation_get_have_rect (GeglOperation *operation,
                              const gchar   *input_pad_name)
{
  GeglPad *pad;
  g_assert (operation && 
            operation->node &&
            input_pad_name);
  pad = gegl_node_get_pad (operation->node, input_pad_name);
  g_assert (pad);
  pad = gegl_pad_get_connected_to (pad);
  if (!pad)
    return NULL;
  g_assert (gegl_pad_get_node (pad));
  
  return gegl_node_get_have_rect (gegl_pad_get_node (pad));
}

void
gegl_operation_set_need_rect (GeglOperation *operation,
                              gint           x,
                              gint           y,
                              gint           width,
                              gint           height)
{
  g_assert (operation);
  g_assert (operation->node);
  gegl_node_set_need_rect (operation->node, x, y, width, height);
}

void
gegl_operation_set_comp_rect (GeglOperation *operation,
                              gint           x,
                              gint           y,
                              gint           width,
                              gint           height)
{
  g_assert (operation);
  g_assert (operation->node);
  gegl_node_set_comp_rect (operation->node, x, y, width, height);
}

gboolean
gegl_operation_get_requested_rect (GeglOperation *operation,
                                   const gchar   *output_pad_name,
                                   GeglRect      *rect)
{
  g_assert (operation && 
            operation->node &&
            output_pad_name);
  return gegl_node_get_requested_rect (operation->node, output_pad_name, rect);
}


static gboolean
calc_have_rect (GeglOperation *self)
{
  g_warning ("Op '%s' has no proper have_rect function",
     G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS(self)));
  return FALSE;
}

static gboolean
calc_need_rect (GeglOperation *self)
{
  g_warning ("Op '%s' has no proper need_rect function",
     G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS(self)));
  return FALSE;
}

static gboolean
calc_comp_rect (GeglOperation *self)
{

  GeglRect *result = gegl_operation_result_rect (self);
  if (!result)
    return FALSE;
  gegl_operation_set_comp_rect (self,
     result->x, result->y,
     result->w, result->h);

  return TRUE;

  g_warning ("Op '%s' has no proper comp_rect function",
     G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS(self)));
}

#include "gegl/gegl-utils.h"

static gboolean
calc_result_rect (GeglOperation *self)
{
  GeglNode *node = self->node;
  g_assert (node);

  gegl_rect_intersect (&node->result_rect, &node->have_rect, &node->need_rect);
  /*g_warning ("Op '%s' has no proper result_rect function",
     G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS(self)));*/
  if(0)g_warning ("%i,%i %ix%i  "
" %i,%i %ix%i  "
" %i,%i %ix%i",
   node->result_rect.x, 
   node->result_rect.y, 
   node->result_rect.w, 
   node->result_rect.h,
   node->have_rect.x, 
   node->have_rect.y, 
   node->have_rect.w, 
   node->have_rect.h,
   node->need_rect.x, 
   node->need_rect.y, 
   node->need_rect.w, 
   node->need_rect.h);
  return FALSE;
}

GeglRect *
gegl_operation_need_rect     (GeglOperation *operation)
{
  g_assert (operation);
  g_assert (operation->node);
  return &operation->node->need_rect;
}
GeglRect *
gegl_operation_have_rect     (GeglOperation *operation)
{
  g_assert (operation);
  g_assert (operation->node);
  return &operation->node->have_rect;
}
GeglRect *
gegl_operation_result_rect   (GeglOperation *operation)
{
  g_assert (operation);
  g_assert (operation->node);
  return &operation->node->result_rect;
}
GeglRect *
gegl_operation_comp_rect     (GeglOperation *operation)
{
  g_assert (operation);
  g_assert (operation->node);
  return &operation->node->comp_rect;
}

