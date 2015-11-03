/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
   Copyright (C) 2015 Anton D. Kachalov <mouse@yandex-team.ru>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pt_canvas.h"
#include "quic.h"
#include "rop3.h"
#include "region.h"

#define PT_CANVAS_REGION
#include "canvas_base.c"

typedef struct PTCanvas PTCanvas;

struct PTCanvas {
    CanvasBase base;
    void *frame;
    uint32_t frame_size;
};

static void pt_canvas_draw_fill(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceFill *fill)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_dump(uint8_t *buf, int len)
{
    for (int i=0; i<len; i++) {
        printf("%02x ", buf[i]);
        if (i>0 && !(i % 16)) printf("\n");
    }
    printf("\n");
}

static void pt_canvas_draw_copy(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceCopy *copy)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    SpiceImage *image = copy->src_bitmap;
    printf("=== %s(%p): new=%p\n", __func__, canvas->frame, copy->src_bitmap);

    if (canvas->frame)
        free(canvas->frame);

    if (image->descriptor.type == SPICE_IMAGE_TYPE_AST) {
        canvas->frame = malloc(image->u.ast.data_size);
        canvas->frame_size = image->u.ast.data_size;
        memcpy(canvas->frame, image->u.ast.data->chunk[0].data, canvas->frame_size);
        printf("-- %p [%x] << %p\n", canvas->frame, canvas->frame_size, image->u.ast.data->chunk[0].data);
        pt_dump(canvas->frame, 128);
    } else {
        canvas->frame = NULL;
        canvas->frame_size = NULL;
    }
}

static void pt_canvas_draw_opaque(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceOpaque *opaque)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_alpha_blend(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceAlphaBlend *alpha_blend)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    SpiceImage *image = alpha_blend->src_bitmap;
    printf("=== %s(%p): new=%p\n", __func__, canvas->frame, alpha_blend->src_bitmap);

    if (canvas->frame)
        free(canvas->frame);

    if (image->descriptor.type == SPICE_IMAGE_TYPE_AST) {
        canvas->frame = malloc(image->u.ast.data_size);
        canvas->frame_size = image->u.ast.data_size;
        memcpy(canvas->frame, image->u.ast.data->chunk[0].data, canvas->frame_size);
//        printf("-- %p [%x] << %p\n", canvas->frame, canvas->frame_size, image->u.ast.data->chunk[0].data);
//        ast_dump(canvas->frame, 128);
    } else {
        canvas->frame = NULL;
        canvas->frame_size = NULL;
    }
}

static void pt_canvas_draw_blend(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceBlend *blend)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_transparent(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceTransparent *transparent)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_whiteness(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceWhiteness *whiteness)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_blackness(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceBlackness *blackness)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_invers(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceInvers *invers)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_rop3(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceRop3 *rop3)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_stroke(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceStroke *stroke)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_draw_text(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpiceText *text)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_clear(SpiceCanvas *spice_canvas)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_copy_bits(SpiceCanvas *spice_canvas, SpiceRect *bbox, SpiceClip *clip, SpicePoint *src_pos)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_read_bits(SpiceCanvas *spice_canvas, uint8_t *dest, int dest_stride, const SpiceRect *area)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;

    spice_return_if_fail(dest_stride > 0);

    printf("=== %s: %dx%d @ %p\n", __func__, (area->bottom - area->top), (area->right - area->left), dest);
    if (canvas->frame)
        memcpy(dest, canvas->frame, canvas->frame_size);
}

static void pt_canvas_group_start(SpiceCanvas *spice_canvas, QRegion *region)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;

    canvas_base_group_start(spice_canvas, region);
}

static void pt_canvas_put_image(SpiceCanvas *spice_canvas, const SpiceRect *dest, const uint8_t *src_data,
                         uint32_t src_width, uint32_t src_height, int src_stride,
                         const QRegion *clip)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;
    printf("=== %s: \n", __func__);
}

static void pt_canvas_group_end(SpiceCanvas *spice_canvas)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;

    canvas_base_group_end(spice_canvas);
}

static int need_init = 1;
static SpiceCanvasOps pt_canvas_ops;

SpiceCanvas *pt_canvas_create(int width, int height, uint32_t format
                               , SpiceImageCache *bits_cache
#ifdef SW_CANVAS_CACHE
                               , SpicePaletteCache *palette_cache
#endif
                               , SpiceImageSurfaces *surfaces
                               , SpiceGlzDecoder *glz_decoder
                               , SpiceJpegDecoder *jpeg_decoder
                               , SpiceZlibDecoder *zlib_decoder
                            )
{
    PTCanvas *canvas;
    int init_ok;

    if (need_init) {
        return NULL;
    }
    canvas = spice_new0(PTCanvas, 1);

    canvas->frame = NULL;
    init_ok = canvas_base_init(&canvas->base, &pt_canvas_ops,
                               width, height, format
                               , bits_cache
#ifdef SW_CANVAS_CACHE
                               , palette_cache
#endif
                               , surfaces
                               , glz_decoder
                               , jpeg_decoder
                               , zlib_decoder
                               );
    if (!init_ok) {
        goto error_1;
    }

    return (SpiceCanvas *)canvas;

error_1:
    free(canvas);

    return NULL;
}

static void pt_canvas_destroy(SpiceCanvas *spice_canvas)
{
    PTCanvas *canvas = (PTCanvas *)spice_canvas;

    if (!canvas) {
        return;
    }
    canvas_base_destroy(&canvas->base);
    if (canvas->frame)
        free(canvas->frame);
    free(canvas);
}

void pt_canvas_init(void) //unsafe global function
{
    if (!need_init) {
        return;
    }
    need_init = 0;

    canvas_base_init_ops(&pt_canvas_ops);
    pt_canvas_ops.draw_fill = pt_canvas_draw_fill;
    pt_canvas_ops.draw_copy = pt_canvas_draw_copy;
    pt_canvas_ops.draw_opaque = pt_canvas_draw_opaque;
    pt_canvas_ops.copy_bits = pt_canvas_copy_bits;
    pt_canvas_ops.draw_text = pt_canvas_draw_text;
    pt_canvas_ops.draw_stroke = pt_canvas_draw_stroke;
    pt_canvas_ops.draw_rop3 = pt_canvas_draw_rop3;
    pt_canvas_ops.draw_blend = pt_canvas_draw_blend;
    pt_canvas_ops.draw_blackness = pt_canvas_draw_blackness;
    pt_canvas_ops.draw_whiteness = pt_canvas_draw_whiteness;
    pt_canvas_ops.draw_invers = pt_canvas_draw_invers;
    pt_canvas_ops.draw_transparent = pt_canvas_draw_transparent;
    pt_canvas_ops.draw_alpha_blend = pt_canvas_draw_alpha_blend;
    pt_canvas_ops.put_image = pt_canvas_put_image;
    pt_canvas_ops.clear = pt_canvas_clear;
    pt_canvas_ops.read_bits = pt_canvas_read_bits;
    pt_canvas_ops.group_start = pt_canvas_group_start;
    pt_canvas_ops.group_end = pt_canvas_group_end;
    pt_canvas_ops.destroy = pt_canvas_destroy;
}
