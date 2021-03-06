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

#ifndef _H__PT_CANVAS
#define _H__PT_CANVAS

#include <spice/macros.h>

#include "canvas_base.h"
#include "region.h"

SPICE_BEGIN_DECLS

SpiceCanvas *pt_canvas_create(int width, int height, uint32_t format
                            , SpiceImageCache *bits_cache
#ifdef SW_CANVAS_CACHE
                            , SpicePaletteCache *palette_cache
#endif
                            , SpiceImageSurfaces *surfaces
                            , SpiceGlzDecoder *glz_decoder
                            , SpiceJpegDecoder *jpeg_decoder
                            , SpiceZlibDecoder *zlib_decoder
                            );
void pt_canvas_init(void);

SPICE_END_DECLS

#endif
