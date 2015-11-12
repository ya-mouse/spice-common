/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
   Copyright (C) 2015 Anton D. Kachalov

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
#if 0
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glib.h>
#include <string.h>
#include <byteswap.h>

#include "spice_common.h"
#include "canvas_base.h"
#include "canvas_utils.h"
#endif

#undef _ALONE
#if _ALONE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <byteswap.h>
#define DEBUG(x...) printf(x)
#else
#define DEBUG(x...)
#endif

#include <math.h>

#include "aspeed-jtables.h"

#define POWEROFTOW		17
#define RANGE_LIMIT_TABLE_SIZE	1408

#define VQ_BLOCK_START_CODE	0
#define JPEG_BLOCK_START_CODE	1
#define VQ_BLOCK_SKIP_CODE	2
#define JPEG_BLOCK_SKIP_CODE	3
#define BLOCK_START_LENGTH	2
#define BLOCK_START_MASK	3
#define BLOCK_HEADER_S_MASK	1
#define BLOCK_HEADER_MASK	15
#define VQ_HEADER_MASK	1
#define VQ_NO_UPDATE_HEADER	0
#define VQ_UPDATE_HEADER	1L
#define VQ_NO_UPDATE_LENGTH	3
#define VQ_UPDATE_LENGTH	27
#define VQ_INDEX_MASK	3L
#define VQ_COLOR_MASK	0xffffffL
#define JPEG_NO_SKIP_CODE	0
#define JPEG_NO_SKIP_PASS2_CODE	2
#define JPEG_SKIP_PASS2_CODE	10
#define LOW_JPEG_NO_SKIP_CODE	4
#define LOW_JPEG_SKIP_CODE	12
#define JPEG_SKIP_CODE	8
#define FRAME_END_CODE	9
#define VQ_NO_SKIP_1_COLOR_CODE	5
#define VQ_NO_SKIP_2_COLOR_CODE	6
#define VQ_NO_SKIP_4_COLOR_CODE	7
#define VQ_SKIP_1_COLOR_CODE	13
#define VQ_SKIP_2_COLOR_CODE	14
#define VQ_SKIP_4_COLOR_CODE	15
#define BLOCK_AST2100_START_LENGTH	4
#define BLOCK_AST2100_SKIP_LENGTH	20

#define FIX_1_082392200 277
#define FIX_1_414213562 362
#define FIX_1_847759065 473
#define FIX_2_613125930 669

#define DCTSIZE		8
#define DCTSIZE_0	0
#define DCTSIZE_1	8
#define DCTSIZE_2	16
#define DCTSIZE_3	24
#define DCTSIZE_4	32
#define DCTSIZE_5	40
#define DCTSIZE_6	48
#define DCTSIZE_7	56

struct ASTHeader
{
    short version;
    short headlen;

    short src_mode_x;
    short src_mode_y;
    short src_mode_depth;
    short src_mode_rate;
    char src_mode_index;

    short dst_mode_x;
    short dst_mode_y;
    short dst_mode_depth;
    short dst_mode_rate;
    char dst_mode_index;

    int frame_start;
    int frame_num;
    short frame_vsize;
    short frame_hsize;

    int rsvd[2];

    char compression;
    char jpeg_scale;
    char jpeg_table;
    char jpeg_yuv;
    char sharp_mode;
    char adv_table;
    char adv_scale;
    int num_of_MB;
    char rc4_en;
    char rc4_reset;

    char mode420;

    char inf_downscale;
    char inf_diff;
    short inf_analog_thr;
    short inf_dig_thr;
    char inf_ext_sig;
    char inf_auto_mode;
    char inf_vqmode;

    int comp_frame_size;
    int comp_size;
    int comp_hdebug;
    int comp_vdebug;

    char input_signal;
    short cur_xpos;
    short cur_ypos;
} __attribute__((packed));

struct HuffmanTable {
    int8_t Length[POWEROFTOW];
    short minor_code[POWEROFTOW];
    short major_code[POWEROFTOW];
    short V[65536];
    short Len[65536];
};

struct ast_decoder {
    int WIDTH;
    int HEIGHT;
    int RealWIDTH;
    int RealHEIGHT;
    int tmp_WIDTHBy16;
    int tmp_HEIGHTBy16;
    int8_t SCALEFACTOR;
    int8_t SCALEFACTORUV;
    int8_t ADVANCESCALEFACTOR;
    int8_t ADVANCESCALEFACTORUV;
    int byte_pos;
    int m_Mode420;
    int selector;
    int advance_selector;
    int Mapping;
    int32_t *buf;
    uint32_t length;
    uint32_t *m_decodeBuf;
    int _index;
    int m_newbits;
    int txb;
    int tyb;
    struct {
        int32_t Color[4];
        uint8_t Index[4];
        uint8_t BitMapBits;
    } m_VQ;
    struct HuffmanTable m_HTDC[4];
    struct HuffmanTable m_HTAC[4];
    int32_t workspace[64];
    int32_t YUVTile[768];
    short  *huff_values;
    int8_t  Y[64];
    int8_t  Cb[64];
    int8_t  Cr[64];
    int8_t  rangeLimitTable[RANGE_LIMIT_TABLE_SIZE];
    short   rangeLimitTableShort[RANGE_LIMIT_TABLE_SIZE];
    int32_t calculatedRGBofY[256];
    int32_t calculatedRGBofCrToR[256];
    int32_t calculatedRGBofCbToB[256];
    int32_t calculatedRGBofCrToG[256];
    int32_t calculatedRGBofCbToG[256];
    int32_t *YValueInTile;
    int32_t YValueInTile420[4][64];
    int32_t CbValueInTile[64];
    int32_t CrValueInTile[64];

    short DCCb[1];
    short DCCr[1];
    short m_DCY[1];
    short *min_code;
    short neg_pow2[POWEROFTOW];

    int8_t m_CbAC_nr;
    int8_t m_CbDC_nr;
    int8_t m_CrAC_nr;
    int8_t m_CrDC_nr;
    int32_t m_DCT_coeff[384];
    int8_t m_YAC_nr;
    int8_t m_YDC_nr;

    int64_t m_QT[4][64];

//    int32_t previousYUVData[0x753000];
};

#define GET_SHORT(byte0) ((short)(byte0 & 0xff))
#define WORD_hi_lo(byte0, byte1) (GET_SHORT(byte0) << 8 | GET_SHORT(byte1))
#define GET_INT(i) ((int)(i & 0xffff))

#if !_ALONE
#define GET_LONG(x) (int64_t)((int32_t)(x) & 0xffffffffL)
#else
static int64_t GET_LONG(int32_t x) {
    DEBUG("-- %x\n", x);
    return x & 0xffffffffL;
}
#endif

#define FIX_G(d) (int)((double)d * 65536 + 0.5)

#define MULTIPLY(x, y) (((x) * (y)) >> 8)

static void initColorTable(struct ast_decoder *dec)
{
    int l = 0x10000;
    int i1 = l >> 1;
    int i = 0;
    for (int j = -128; i < 256; j++) {
        dec->calculatedRGBofCrToR[i] = (FIX_G(1.597656) * j + i1) >> 16;
        dec->calculatedRGBofCbToB[i] = (FIX_G(2.015625) * j + i1) >> 16;
        dec->calculatedRGBofCrToG[i] = (-FIX_G(0.8125) * j + i1) >> 16;
        dec->calculatedRGBofCbToG[i] = (-FIX_G(0.390625) * j + i1) >> 16;
        i++;
    }

    i = 0;
    for (int k = -16; i < 256; k++) {
        dec->calculatedRGBofY[i] = (FIX_G(1.1639999999999999) * k + i1) >> 16;
        i++;
    }
}

static void initRangeLimitTable(struct ast_decoder *dec)
{
    memset(dec->rangeLimitTable, 0, 255);
//    memset(dec->rangeLimitTableShort, 0, 255*sizeof(short));
    for (short word0 = 0; word0 < 256; word0++)
    {
        dec->rangeLimitTable[256 + word0] = (int8_t)word0;
        dec->rangeLimitTableShort[256 + word0] = word0;
    }

    memset(dec->rangeLimitTable+512, -1, 896-512);
    for (int i=512; i<896; i++) {
        dec->rangeLimitTableShort[i] = 255;
//+512, 255, (896-512)*sizeof(short));
    }
    memset(dec->rangeLimitTable+896, 0, 1280-896);
    memset(dec->rangeLimitTableShort+896, 0, (1280-896)*sizeof(short));
    for (short word1 = 1280; word1 < 1408; word1++) {
        dec->rangeLimitTable[word1] = (int8_t)word1;
        dec->rangeLimitTableShort[word1] = (short)(word1 & 0xff);
    }
}

static void loadHuffmanTable(struct HuffmanTable *huffmantable, int8_t *abyte0, short *aword0, int *ai)
{
    DEBUG("--> loadHuffmanTable()\n");
    for (int8_t byte2 = 1; byte2 <= 16; byte2++)
        huffmantable->Length[byte2] = abyte0[byte2];

    int i = 0;
    for (int8_t byte0 = 1; byte0 <= 16; byte0++)
    {
        for (int8_t byte3 = 0; byte3 < GET_SHORT(huffmantable->Length[byte0]); byte3++)
        {
            huffmantable->V[GET_INT(WORD_hi_lo(byte0, byte3))] = aword0[i];
            i++;
        }
    }

    int j = 0;
    for (int8_t byte1 = 1; byte1 <= 16; byte1++)
    {
        huffmantable->minor_code[byte1] = (short)j;
        for (int8_t byte4 = 1; byte4 <= GET_SHORT(huffmantable->Length[byte1]); byte4++)
            j++;

        huffmantable->major_code[byte1] = (short)(j - 1);
        j *= 2;
        if (GET_SHORT(huffmantable->Length[byte1]) == 0)
        {
            huffmantable->minor_code[byte1] = -1;
            huffmantable->major_code[byte1] = 0;
        }
    }

    huffmantable->Len[0] = 2;
    i = 2;
    for (int k = 1; k < 65535; k++) {
        if(k < ai[i])
        {
            huffmantable->Len[k] = (int8_t)(ai[i + 1] & 0xff);
        } else
        {
            i += 2;
            huffmantable->Len[k] = (int8_t)(ai[i + 1] & 0xff);
        }
    }
}

static void initHuffmanTable(struct ast_decoder *dec)
{
    loadHuffmanTable(&dec->m_HTDC[0],
                     std_dc_luminance_nrcodes,
                     std_dc_luminance_values,
                     DC_LUMINANCE_HUFFMANCODE);
    loadHuffmanTable(&dec->m_HTAC[0],
                     std_ac_luminance_nrcodes,
                     std_ac_luminance_values,
                     AC_LUMINANCE_HUFFMANCODE);
    loadHuffmanTable(&dec->m_HTDC[1],
                     std_dc_chrominance_nrcodes,
                     std_dc_chrominance_values,
                     DC_CHROMINANCE_HUFFMANCODE);
    loadHuffmanTable(&dec->m_HTAC[1],
                     std_ac_chrominance_nrcodes,
                     std_ac_chrominance_values,
                     AC_CHROMINANCE_HUFFMANCODE);
}

static void convertYUVtoRGB(struct ast_decoder *dec, int i, int j)
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    int32_t end = 0; //dec->RealWIDTH * dec->RealHEIGHT - 1;
    DEBUG("--> convertYUVtoRGB(%d, %d)\n", i, j);
        if(dec->m_Mode420 == 0)
        {
            dec->YValueInTile = dec->YUVTile;
            for(int k = 0; k < 64; k++)
            {
                dec->CbValueInTile[k] = dec->YUVTile[64 + k];
                dec->CrValueInTile[k] = dec->YUVTile[128 + k];
            }

            int k5 = i * 8;
            int i6 = j * 8;
            int k2 = i6 * dec->RealWIDTH + k5;
            int j8 = dec->RealWIDTH - k5;
            if(j8 == 0 || j8 > 8)
                j8 = 8;
            for(int k1 = 0; k1 < 8; k1++)
            {
                for(int l = 0; l < j8; l++)
                {
                    int i3 = (k1 << 3) + l;
                    int k3 = (k2 + l) * 3;
                    int i4 = dec->YValueInTile[i3];
                    int k4 = dec->CbValueInTile[i3];
                    int i5 = dec->CrValueInTile[i3];
//                    dec->previousYUVData[k3] = i4;
//                    dec->previousYUVData[k3 + 1] = k4;
//                    dec->previousYUVData[k3 + 2] = i5;
                    int l6 = dec->calculatedRGBofY[i4] + dec->calculatedRGBofCbToB[k4];
                    int j7 = dec->calculatedRGBofY[i4] + (dec->calculatedRGBofCbToG[k4] + dec->calculatedRGBofCrToG[i5]);
                    int l7 = dec->calculatedRGBofY[i4] + dec->calculatedRGBofCrToR[i5];
                    if(l6 >= 0)
                        l6 += 256;
                    else
                        l6 = 0;
                    if(j7 >= 0)
                        j7 += 256;
                    else
                        j7 = 0;
                    if(l7 >= 0)
                        l7 += 256;
                    else
                        l7 = 0;
                    if(k3 < dec->RealWIDTH * dec->RealHEIGHT * 3)
                    {
                        b = dec->rangeLimitTable[l6];
                        g = dec->rangeLimitTable[j7];
                        r = dec->rangeLimitTable[l7];
                        dec->m_decodeBuf[end-(i6 + k1)*dec->RealWIDTH + k5 + l] = b | g << 8 | r << 16 | 0xff << 24;
                        DEBUG("444 k3=%d %d %d, %d %d %d  %d %d %d %08X\n", k3, k5 + l, i6 + k1, (int8_t)r, (int8_t)g, (int8_t)b, k1, k2, i3, dec->m_decodeBuf[end-(i6 + k1)*dec->RealWIDTH + k5 + l]);
                    }
                }

                k2 += dec->RealWIDTH;
            }

        } else
        {
            int k6 = 0;
            for(int i1 = 0; i1 < 4; i1++)
            {
                for(int l1 = 0; l1 < 64; l1++)
                {
                    dec->YValueInTile420[i1][l1] = dec->YUVTile[k6];
                    k6++;
                }

            }

            for(int i2 = 0; i2 < 64; i2++)
            {
                dec->CbValueInTile[i2] = dec->YUVTile[k6];
                dec->CrValueInTile[i2] = dec->YUVTile[k6 + 64];
                k6++;
            }

            int l5 = i * 16;
            int j6 = j * 16;
            int l2 = j6 * dec->WIDTH + l5;
            int l8 = 0;
            int i9 = 0;
            int j9 = 0;
            int k9 = 0;
            int8_t byte1 = 16;
            if(dec->HEIGHT == 608 && j == 37)
                byte1 = 8;
            for(int j2 = 0; j2 < byte1; j2++)
            {
                int i10 = (j2 >> 3) * 2;
                int j10 = (j2 >> 1) << 3;
                for(int j1 = 0; j1 < 16; j1++)
                {
                    int l9 = i10 + (j1 >> 3);
                    int k8;
                    switch(l9)
                    {
                    case 0: // '\0'
                        k8 = l8++;
                        break;

                    case 1: // '\001'
                        k8 = i9++;
                        break;

                    case 2: // '\002'
                        k8 = j9++;
                        break;

                    default:
                        k8 = k9++;
                        break;
                    }
//                    int l3 = (l2 + j1) * 3;
                    int j3 = j10 + (j1 >> 1);
                    int j4 = dec->YValueInTile420[l9][k8];
                    int l4 = dec->CbValueInTile[j3];
                    int j5 = dec->CrValueInTile[j3];
                    int B = dec->calculatedRGBofY[j4] + dec->calculatedRGBofCbToB[l4];
                    int G = dec->calculatedRGBofY[j4] + (dec->calculatedRGBofCbToG[l4] + dec->calculatedRGBofCrToG[j5]);
                    int R = dec->calculatedRGBofY[j4] + dec->calculatedRGBofCrToR[j5];
                    if(B >= 0)
                        b = dec->rangeLimitTable[B + 256];
                    else
                        b = 0;
                    if(G >= 0)
                        g = dec->rangeLimitTable[G + 256];
                    else
                        g = 0;
                    if(R >= 0)
                        r = dec->rangeLimitTable[R + 256];
                    else
                        r = 0;
//                    dec->m_decodeBuf[end-(j6 + j2)*dec->RealWIDTH + l5 + j1] = b | g << 8 | r << 16;
//                    DEBUG("420 %d %d, %d %d %d  %08X\n", l5 + j1, j6 + j2, b, g, r, dec->m_decodeBuf[(j6 + j2)*dec->RealWIDTH + l5 + j1]);
                }

                l2 += dec->RealWIDTH;
            }
        }
}

static void setQuantizationTable(int8_t *abyte0, int8_t byte0, int8_t *abyte1)
{
    for(int8_t byte1 = 0; byte1 < 64; byte1++)
    {
        int i = (abyte0[byte1] * 16) / byte0;
        if (i <= 0)
            i = 1;
        if (i > 255)
            i = 255;
        abyte1[zigzag[byte1]] = (int8_t)i;
    }
}

static void loadLuminanceQuantizationTable(struct ast_decoder *dec, int64_t *al)
{
    float af[] = {
        1.0F, 1.38704F, 1.306563F, 1.175876F, 1.0F, 0.785695F, 0.5411961F, 0.2758994F
    };
    int8_t abyte0[64];
    int8_t *std_luminance_qt;

    switch (dec->selector)
    {
    case 0: // '\0'
        std_luminance_qt = Tbl_000Y;
        break;

    case 1: // '\001'
        std_luminance_qt = Tbl_014Y;
        break;

    case 2: // '\002'
        std_luminance_qt = Tbl_029Y;
        break;

    case 3: // '\003'
        std_luminance_qt = Tbl_043Y;
        break;

    case 4: // '\004'
        std_luminance_qt = Tbl_057Y;
        break;

    case 5: // '\005'
        std_luminance_qt = Tbl_071Y;
        break;

    case 6: // '\006'
        std_luminance_qt = Tbl_086Y;
        break;

    case 7: // '\007'
        std_luminance_qt = Tbl_100Y;
        break;

    default:
        return;
    }
    setQuantizationTable(std_luminance_qt, dec->SCALEFACTOR, abyte0);
    for (int8_t byte0 = 0; byte0 <= 63; byte0++)
        al[byte0] = GET_SHORT(abyte0[zigzag[byte0]]);

    int8_t byte1 = 0;
    for(int8_t byte2 = 0; byte2 <= 7; byte2++)
    {
        for(int8_t byte3 = 0; byte3 <= 7; byte3++)
        {
            int i = (int)((float)al[byte1] * (af[byte2] * af[byte3]));
            al[byte1] = i * 0x10000;
            byte1++;
        }

    }

    dec->byte_pos += 64;
}

static void loadChrominanceQuantizationTable(struct ast_decoder *dec, int64_t *al)
{
    float af[] = {
        1.0F, 1.38704F, 1.306563F, 1.175876F, 1.0F, 0.785695F, 0.5411961F, 0.2758994F
    };
    int8_t abyte0[64];
    int8_t *std_chrominance_qt;

    if (dec->Mapping == 1)
        switch(dec->selector)
        {
        case 0: // '\0'
            std_chrominance_qt = Tbl_000Y;
            break;

        case 1: // '\001'
            std_chrominance_qt = Tbl_014Y;
            break;

        case 2: // '\002'
            std_chrominance_qt = Tbl_029Y;
            break;

        case 3: // '\003'
            std_chrominance_qt = Tbl_043Y;
            break;

        case 4: // '\004'
            std_chrominance_qt = Tbl_057Y;
            break;

        case 5: // '\005'
            std_chrominance_qt = Tbl_071Y;
            break;

        case 6: // '\006'
            std_chrominance_qt = Tbl_086Y;
            break;

        case 7: // '\007'
            std_chrominance_qt = Tbl_100Y;
            break;

        default:
            return;
        }
    else
        switch(dec->selector)
        {
        case 0: // '\0'
            std_chrominance_qt = Tbl_000UV;
            break;

        case 1: // '\001'
            std_chrominance_qt = Tbl_014UV;
            break;

        case 2: // '\002'
            std_chrominance_qt = Tbl_029UV;
            break;

        case 3: // '\003'
            std_chrominance_qt = Tbl_043UV;
            break;

        case 4: // '\004'
            std_chrominance_qt = Tbl_057UV;
            break;

        case 5: // '\005'
            std_chrominance_qt = Tbl_071UV;
            break;

        case 6: // '\006'
            std_chrominance_qt = Tbl_086UV;
            break;

        case 7: // '\007'
            std_chrominance_qt = Tbl_100UV;
            break;

        default:
            return;
        }
    setQuantizationTable(std_chrominance_qt, dec->SCALEFACTORUV, abyte0);
    for(int8_t byte0 = 0; byte0 <= 63; byte0++)
        al[byte0] = GET_SHORT(abyte0[zigzag[byte0]]);

    int8_t byte1 = 0;
    for(int8_t byte2 = 0; byte2 <= 7; byte2++)
    {
        for(int8_t byte3 = 0; byte3 <= 7; byte3++)
        {
            int i = (int)((float)al[byte1] * (af[byte2] * af[byte3]));
            al[byte1] = i * 0x10000;
            byte1++;
        }
    }

    dec->byte_pos += 64;
}

static void loadPass2LuminanceQuantizationTable(struct ast_decoder *dec, int64_t *al)
{
    float af[] = {
        1.0F, 1.38704F, 1.306563F, 1.175876F, 1.0F, 0.785695F, 0.5411961F, 0.2758994F
    };
    int8_t abyte0[64];
    int8_t *std_luminance_qt;

    switch(dec->advance_selector)
    {
    case 0: // '\0'
        std_luminance_qt = Tbl_000Y;
        break;

    case 1: // '\001'
        std_luminance_qt = Tbl_014Y;
        break;

    case 2: // '\002'
        std_luminance_qt = Tbl_029Y;
        break;

    case 3: // '\003'
        std_luminance_qt = Tbl_043Y;
        break;

    case 4: // '\004'
        std_luminance_qt = Tbl_057Y;
        break;

    case 5: // '\005'
        std_luminance_qt = Tbl_071Y;
        break;

    case 6: // '\006'
        std_luminance_qt = Tbl_086Y;
        break;

    case 7: // '\007'
        std_luminance_qt = Tbl_100Y;
        break;

    default:
        return;
    }
    setQuantizationTable(std_luminance_qt, dec->ADVANCESCALEFACTOR, abyte0);
    for(int8_t byte0 = 0; byte0 <= 63; byte0++)
        al[byte0] = GET_SHORT(abyte0[zigzag[byte0]]);

    int8_t byte1 = 0;
    for(int8_t byte2 = 0; byte2 <= 7; byte2++)
    {
        for(int8_t byte3 = 0; byte3 <= 7; byte3++)
        {
            int i = (int)((float)al[byte1] * (af[byte2] * af[byte3]));
            al[byte1] = i * 0x10000;
            byte1++;
        }

    }

    dec->byte_pos += 64;
}

static void loadPass2ChrominanceQuantizationTable(struct ast_decoder *dec, int64_t *al)
{
    float af[] = {
        1.0F, 1.38704F, 1.306563F, 1.175876F, 1.0F, 0.785695F, 0.5411961F, 0.2758994F
    };
    int8_t abyte0[64];
    int8_t *std_chrominance_qt;

    if(dec->Mapping == 1)
        switch(dec->advance_selector)
        {
        case 0: // '\0'
            std_chrominance_qt = Tbl_000Y;
            break;

        case 1: // '\001'
            std_chrominance_qt = Tbl_014Y;
            break;

        case 2: // '\002'
            std_chrominance_qt = Tbl_029Y;
            break;

        case 3: // '\003'
            std_chrominance_qt = Tbl_043Y;
            break;

        case 4: // '\004'
            std_chrominance_qt = Tbl_057Y;
            break;

        case 5: // '\005'
            std_chrominance_qt = Tbl_071Y;
            break;

        case 6: // '\006'
            std_chrominance_qt = Tbl_086Y;
            break;

        case 7: // '\007'
            std_chrominance_qt = Tbl_100Y;
            break;

        default:
            return;
        }
    else
        switch(dec->advance_selector)
        {
        case 0: // '\0'
            std_chrominance_qt = Tbl_000UV;
            break;

        case 1: // '\001'
            std_chrominance_qt = Tbl_014UV;
            break;

        case 2: // '\002'
            std_chrominance_qt = Tbl_029UV;
            break;

        case 3: // '\003'
            std_chrominance_qt = Tbl_043UV;
            break;

        case 4: // '\004'
            std_chrominance_qt = Tbl_057UV;
            break;

        case 5: // '\005'
            std_chrominance_qt = Tbl_071UV;
            break;

        case 6: // '\006'
            std_chrominance_qt = Tbl_086UV;
            break;

        case 7: // '\007'
            std_chrominance_qt = Tbl_100UV;
            break;

        default:
            return;
        }
    setQuantizationTable(std_chrominance_qt, dec->ADVANCESCALEFACTORUV, abyte0);
    for(int8_t byte0 = 0; byte0 <= 63; byte0++)
        al[byte0] = GET_SHORT(abyte0[zigzag[byte0]]);

    int8_t byte1 = 0;
    for(int8_t byte2 = 0; byte2 <= 7; byte2++)
    {
        for(int8_t byte3 = 0; byte3 <= 7; byte3++)
        {
            int i = (int)((float)al[byte1] * (af[byte2] * af[byte3]));
            al[byte1] = i * 0x10000;
            byte1++;
        }
    }

    dec->byte_pos += 64;
}


static void updateReadBuf(struct ast_decoder *dec, int i)
{
    int64_t uprdbuf_readbuf;
    DEBUG("--> updateReadBuf(%d)\n", i);
    if (dec->m_newbits - i <= 0) {
        uprdbuf_readbuf = GET_LONG(dec->buf[dec->_index]);
        dec->_index++;
        dec->buf[0] = (int32_t)(GET_LONG(dec->buf[0]) << i) | (int32_t)((GET_LONG(dec->buf[1]) | uprdbuf_readbuf >> dec->m_newbits) >> (32 - i));
        dec->buf[1] = (int32_t)(uprdbuf_readbuf << (i - dec->m_newbits));
        dec->m_newbits += 32 - i;
    } else {
        dec->buf[0] = (int32_t)(GET_LONG(dec->buf[0]) << i) | (int32_t)(GET_LONG(dec->buf[1]) >> (32 - i));
        dec->buf[1] = (int32_t)(GET_LONG(dec->buf[1]) << i);
        dec->m_newbits -= i;
    }
}

static short lookKbits(struct ast_decoder *dec, uint8_t byte0)
{
    return (short)(GET_LONG(dec->buf[0]) >> (32 - byte0));
}

static void skipKbits(struct ast_decoder *dec, uint8_t byte0)
{
    if (dec->m_newbits - byte0 <= 0) {
        if (dec->_index > dec->length - 1)
            dec->_index = dec->length - 1;
        dec->buf[0] = (int32_t)(GET_LONG(dec->buf[0]) << byte0) | (int32_t)((GET_LONG(dec->buf[1]) | GET_LONG(dec->buf[dec->_index]) >> dec->m_newbits) >> (32 - byte0));
        dec->buf[1] = (int32_t)(GET_LONG(dec->buf[dec->_index]) << (byte0 - dec->m_newbits));
        dec->m_newbits += 32 - byte0;
        dec->_index++;
    } else {
        dec->buf[0] = (dec->buf[0] << byte0) | (int32_t)(GET_LONG(dec->buf[1]) >> (32 - byte0));
        dec->buf[1] = (dec->buf[1] << byte0);
        dec->m_newbits -= byte0;
    }
}

static short getKbits(struct ast_decoder *dec, int8_t byte0)
{
    short signed_wordvalue = lookKbits(dec, byte0);
    if(((1L << (byte0 - 1)) & (long)signed_wordvalue) == 0L)
        signed_wordvalue = (short)(signed_wordvalue + dec->neg_pow2[byte0]);
    skipKbits(dec, byte0);
    return signed_wordvalue;
}

static void moveBlockIndex(struct ast_decoder *dec)
{
    dec->txb++;
    DEBUG("--> moveBlockIndex()\n");
    if (dec->txb == dec->tmp_WIDTHBy16) {
        // m_view.repaint((dec->txb - 1) * 16, dec->tyb * 16, 16, 16);
//        printf("repaint\n");
    }
    if (dec->m_Mode420 == 0)
    {
        if (dec->txb >= dec->tmp_WIDTHBy16 / 8)
        {
            dec->tyb++;
            if (dec->tyb >= dec->tmp_HEIGHTBy16 / 8)
                dec->tyb = 0;
            dec->txb = 0;
        }
    } else if (dec->txb >= dec->tmp_WIDTHBy16 / 16)
    {
        dec->tyb++;
        if (dec->tyb >= dec->tmp_HEIGHTBy16 / 16)
            dec->tyb = 0;
        dec->txb = 0;
    }
//    SOCIVTPPktHdr.gTxb = dec->txb;
//    SOCIVTPPktHdr.gTyb = dec->tyb;
//    pixels += 256L;
}


static void decompressVQ(struct ast_decoder *dec, int i, int j, char byte0)
{
    int k = 0;
    DEBUG("--> decompressVQ(%d, %d, %d)\n", i, j, byte0);
    if (dec->m_VQ.BitMapBits == 0) {
        for (int l = 0; l < 64; l++) {
            dec->YUVTile[k + 0] = (dec->m_VQ.Color[dec->m_VQ.Index[0]] & 0xff0000L) >> 16;
            dec->YUVTile[k + 64] = (dec->m_VQ.Color[dec->m_VQ.Index[0]] & 65280L) >> 8;
            dec->YUVTile[k + 128] = (dec->m_VQ.Color[dec->m_VQ.Index[0]] & 255L);
            k++;
        }
    } else {
        for (int i1 = 0; i1 < 64; i1++) {
            short word0 = lookKbits(dec, dec->m_VQ.BitMapBits);
            dec->YUVTile[k + 0] = (dec->m_VQ.Color[dec->m_VQ.Index[word0]] & 0xff0000L) >> 16;
            dec->YUVTile[k + 64] = (dec->m_VQ.Color[dec->m_VQ.Index[word0]] & 65280L) >> 8;
            dec->YUVTile[k + 128] = (dec->m_VQ.Color[dec->m_VQ.Index[word0]] & 255L);
            k++;
            skipKbits(dec, dec->m_VQ.BitMapBits);
        }
    }

    convertYUVtoRGB(dec, i, j);
}


static void inverseDCT(struct ast_decoder *dec, int i, int8_t byte0)
{
        int j = 0;
        int k = 0;
        int l = 0;
        l = i;
        DEBUG("--> inverseDCT(%d,%d)\n", i, byte0);
        for(int k9 = DCTSIZE; k9 > 0; k9--)
            if((dec->m_DCT_coeff[l + DCTSIZE_1] | dec->m_DCT_coeff[l + DCTSIZE_2] | dec->m_DCT_coeff[l + DCTSIZE_3] | dec->m_DCT_coeff[l + DCTSIZE_4] | dec->m_DCT_coeff[l + DCTSIZE_5] | dec->m_DCT_coeff[l + DCTSIZE_6] | dec->m_DCT_coeff[l + DCTSIZE_7]) == 0)
            {
                int i10 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_0] * dec->m_QT[byte0][j + DCTSIZE_0]) >> 16;
                dec->workspace[k + DCTSIZE_0] = i10;
                dec->workspace[k + DCTSIZE_1] = i10;
                dec->workspace[k + DCTSIZE_2] = i10;
                dec->workspace[k + DCTSIZE_3] = i10;
                dec->workspace[k + DCTSIZE_4] = i10;
                dec->workspace[k + DCTSIZE_5] = i10;
                dec->workspace[k + DCTSIZE_6] = i10;
                dec->workspace[k + DCTSIZE_7] = i10;
                l++;
                j++;
                k++;
                //nZeroACTerms++;
            } else
            {
                int i1 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_0] * dec->m_QT[byte0][j + DCTSIZE_0]) >> 16;
                int k1 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_2] * dec->m_QT[byte0][j + DCTSIZE_2]) >> 16;
                int i2 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_4] * dec->m_QT[byte0][j + DCTSIZE_4]) >> 16;
                int k2 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_6] * dec->m_QT[byte0][j + DCTSIZE_6]) >> 16;
                int i5 = i1 + i2;
                int k5 = i1 - i2;
                int k6 = k1 + k2;
                int i6 = MULTIPLY(k1 - k2, FIX_1_414213562) - k6;
                DEBUG("%d,%d %d  %d\n", k1, k2, MULTIPLY(k1 - k2, FIX_1_414213562), FIX_1_414213562);
                i1 = i5 + k6;
                k2 = i5 - k6;
                k1 = k5 + i6;
                i2 = k5 - i6;
                int i3 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_1] * dec->m_QT[byte0][j + DCTSIZE_1]) >> 16;
                int k3 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_3] * dec->m_QT[byte0][j + DCTSIZE_3]) >> 16;
                int i4 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_5] * dec->m_QT[byte0][j + DCTSIZE_5]) >> 16;
                int k4 = (int32_t)((int64_t)dec->m_DCT_coeff[l + DCTSIZE_7] * dec->m_QT[byte0][j + DCTSIZE_7]) >> 16;
                DEBUG("%d %d %d %d\n", i5, k5, i6, k6);
                int i9 = i4 + k3;
                int k7 = i4 - k3;
                int i8 = i3 + k4;
                int k8 = i3 - k4;
                k4 = i8 + i9;
                k5 = MULTIPLY(i8 - i9, FIX_1_414213562);
                int i7 = MULTIPLY(k7 + k8, FIX_1_847759065);
                i5 = MULTIPLY(k8, FIX_1_082392200) - i7;
                i6 = MULTIPLY(k7, -FIX_2_613125930) + i7;
                i4 = i6 - k4;
                k3 = k5 - i4;
                i3 = i5 + k3;
                dec->workspace[k + DCTSIZE_0] = i1 + k4;
                dec->workspace[k + DCTSIZE_7] = i1 - k4;
                dec->workspace[k + DCTSIZE_1] = k1 + i4;
                dec->workspace[k + DCTSIZE_6] = k1 - i4;
                dec->workspace[k + DCTSIZE_2] = i2 + k3;
                dec->workspace[k + DCTSIZE_5] = i2 - k3;
                dec->workspace[k + DCTSIZE_4] = k2 + i3;
                dec->workspace[k + DCTSIZE_3] = k2 - i3;
                l++;
                j++;
                k++;
            }

        k = 0;
        for(int l9 = 0; l9 < DCTSIZE; l9++)
        {
            int j10 = i + l9 * DCTSIZE;
            int j5 = dec->workspace[k + 0] + dec->workspace[k + 4];
            int l5 = dec->workspace[k + 0] - dec->workspace[k + 4];
            int l6 = dec->workspace[k + 2] + dec->workspace[k + 6];
            int j6 = MULTIPLY(dec->workspace[k + 2] - dec->workspace[k + 6], FIX_1_414213562) - l6;
            int j1 = j5 + l6;
            int l2 = j5 - l6;
            int l1 = l5 + j6;
            int j2 = l5 - j6;
            int j9 = dec->workspace[k + 5] + dec->workspace[k + 3];
            int l7 = dec->workspace[k + 5] - dec->workspace[k + 3];
            int j8 = dec->workspace[k + 1] + dec->workspace[k + 7];
            int l8 = dec->workspace[k + 1] - dec->workspace[k + 7];
            int l4 = j8 + j9;
            l5 = MULTIPLY(j8 - j9, FIX_1_414213562);
            int j7 = MULTIPLY(l7 + l8, FIX_1_847759065);
            j5 = MULTIPLY(l8, FIX_1_082392200) - j7;
            j6 = MULTIPLY(l7, -FIX_2_613125930) + j7;
            int j4 = j6 - l4;
            int l3 = l5 - j4;
            int j3 = j5 + l3;
            int k10 = 0;
            DEBUG("%d: %d %d %d\n", l9, l5, j5, j6);
            k10 = 128 + (((j1 + l4) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 0] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((j1 - l4) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 7] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((l1 + j4) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 1] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((l1 - j4) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 6] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((j2 + l3) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 2] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((j2 - l3) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 5] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((l2 + j3) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 4] = dec->rangeLimitTableShort[k10 + 256];
            k10 = 128 + (((l2 - j3) >> 3) & 0x3ff);
            dec->YUVTile[j10 + 3] = dec->rangeLimitTableShort[k10 + 256];
            k += DCTSIZE;
        }

}


static void decodeHuffmanDataUnit(struct ast_decoder *dec, int8_t byte0, int8_t byte1, short *aword0, short word0)
{
        int8_t byte5 = 0;
        DEBUG("--> decodeHuffmanDataUnit(%d,%d)\n", byte0, byte1);
        memset(dec->m_DCT_coeff, 0, sizeof(dec->m_DCT_coeff));
        dec->min_code = dec->m_HTDC[byte0].minor_code;
        dec->huff_values = dec->m_HTDC[byte0].V;
        int8_t byte2 = 0;
        int8_t byte3 = dec->m_HTDC[byte0].Len[dec->buf[0] >> 16 & 0xffff];
        short word1 = lookKbits(dec, byte3);
        skipKbits(dec, byte3);
        byte5 = dec->huff_values[WORD_hi_lo(byte3, (int8_t)(word1 - dec->min_code[byte3]))];
        if(byte5 == 0)
        {
            dec->m_DCT_coeff[word0 + 0] = aword0[0];
        } else
        {
            int i = aword0[0] + getKbits(dec, byte5);
            dec->m_DCT_coeff[word0] = i;
            aword0[0] = (short)dec->m_DCT_coeff[word0];
        }
        dec->min_code = dec->m_HTAC[byte1].minor_code;
        dec->huff_values = dec->m_HTAC[byte1].V;
        byte2 = 1;
        do
        {
            int8_t byte4 = dec->m_HTAC[byte1].Len[dec->buf[0] >> 16 & 0xffff];
            short word2 = lookKbits(dec, byte4);
            skipKbits(dec, byte4);
            int8_t byte8 = dec->huff_values[WORD_hi_lo(byte4, (int8_t)(word2 - dec->min_code[byte4]))];
            int8_t byte6 = (byte8 & 0xf);
            int8_t byte7 = (byte8 >> 4 & 0xf);
            if(byte6 == 0)
            {
                if(byte7 != 15)
                    break;
                byte2 += 16;
            } else
            {
                byte2 += byte7;
                dec->m_DCT_coeff[word0 + dezigzag[byte2]] = getKbits(dec, byte6);
                byte2++;
            }
        } while(byte2 < 64);
}

#if 0
    final void precalculateCrCbTables()
    {
        System.out.println("--> precalculateCrCbTables()");
        for(short word0 = 0; word0 <= 255; word0++)
            m_Cr_tab[word0] = (short)(int)(((double)word0 - 128D) * 1.4019999999999999D);

        for(short word1 = 0; word1 <= 255; word1++)
            m_Cb_tab[word1] = (short)(int)(((double)word1 - 128D) * 1.772D);

        for(short word2 = 0; word2 <= 255; word2++)
        {
            for(short word3 = 0; word3 <= 255; word3++)
                m_Cr_Cb_green_tab[(word2 << 8) + word3] = (short)(int)(-0.34414D * ((double)word3 - 128D) - 0.71414D * ((double)word2 - 128D));

        }

    }
#endif

static void decompressJPEG(struct ast_decoder *dec, int i, int j, int8_t byte0)
{
        DEBUG("--> decompressJPEGPass(%d,%d,%d)\n", i, j, byte0);
        decodeHuffmanDataUnit(dec, dec->m_YDC_nr, dec->m_YAC_nr, dec->m_DCY, (short)0);
        inverseDCT(dec, 0, byte0);
        if(dec->m_Mode420 == 1)
        {
            decodeHuffmanDataUnit(dec, dec->m_YDC_nr, dec->m_YAC_nr, dec->m_DCY, (short)64);
            inverseDCT(dec, 64, byte0);
            decodeHuffmanDataUnit(dec, dec->m_YDC_nr, dec->m_YAC_nr, dec->m_DCY, (short)128);
            inverseDCT(dec, 128, byte0);
//            if(SOCIVTPPktHdr.gTxb == 0 && SOCIVTPPktHdr.gTyb == 1)
//                SOCIVTPPktHdr.gCheck = 100;
            decodeHuffmanDataUnit(dec, dec->m_YDC_nr, dec->m_YAC_nr, dec->m_DCY, (short)192);
            inverseDCT(dec, 192, byte0);
//            if(SOCIVTPPktHdr.gTxb == 0 && SOCIVTPPktHdr.gTyb == 1)
//                SOCIVTPPktHdr.gCheck = 101;
            decodeHuffmanDataUnit(dec, dec->m_CbDC_nr, dec->m_CbAC_nr, dec->DCCb, (short)256);
            inverseDCT(dec, 256, (byte0 + 1));
            decodeHuffmanDataUnit(dec, dec->m_CrDC_nr, dec->m_CrAC_nr, dec->DCCr, (short)320);
            inverseDCT(dec, 320, (byte0 + 1));
        } else
        {
            decodeHuffmanDataUnit(dec, dec->m_CbDC_nr, dec->m_CbAC_nr, dec->DCCb, (short)64);
            inverseDCT(dec, 64, (byte0 + 1));
            decodeHuffmanDataUnit(dec, dec->m_CrDC_nr, dec->m_CrAC_nr, dec->DCCr, (short)128);
            inverseDCT(dec, 128, (byte0 + 1));
        }
        convertYUVtoRGB(dec, i, j);
}

static void decompressJPEGPass2(struct ast_decoder *dec, int i, int j, int8_t byte0)
{
        DEBUG("--> decompressJPEGPass2(%d,%d,%d)\n", i, j, byte0);
        decodeHuffmanDataUnit(dec, dec->m_YDC_nr, dec->m_YAC_nr, dec->m_DCY, (short)0);
        inverseDCT(dec, 0, byte0);
        decodeHuffmanDataUnit(dec, dec->m_CbDC_nr, dec->m_CbAC_nr, dec->DCCb, (short)64);
        inverseDCT(dec, 64, (byte0 + 1));
        decodeHuffmanDataUnit(dec, dec->m_CrDC_nr, dec->m_CrAC_nr, dec->DCCr, (short)128);
        inverseDCT(dec, 128, (byte0 + 1));
//        convertYUVToRGBPass2(dec, i, j);
}


static void dump(uint8_t *buf, int len, int tofile)
{
    FILE *fp;
    static int i = 0;
    char s[64];

  if (tofile) {
    sprintf(s, "/tmp/aspeed.%04d.%03d.log", len, i++);
    fp = fopen(s, "w+");
    if (!fp) return;
    printf("dump %p\n", buf);
    fwrite(buf, len, 1, fp);
    fclose(fp);
  } else {
    for (int i=0; i<len; i++) {
        printf("%02x ", buf[i]);
        if (i>0 && !(i % 16)) printf("\n");
    }
    printf("\n");
  }
}

#if !_ALONE
static pixman_image_t *canvas_get_aspeed(CanvasBase *canvas, SpiceImage *image)
#else
int main(int argc, char **argv)
#endif
{
    int width;
    int height;
    uint8_t *dest, *orig;
    size_t len;
    int i, j;
    struct ASTHeader *hdr;
#if _ALONE
    FILE *fp;
#else
    pixman_image_t *surface = NULL;
#endif
    struct ast_decoder *dec = calloc(sizeof(struct ast_decoder), 1);

    initColorTable(dec);
    initRangeLimitTable(dec);
    initHuffmanTable(dec);

//    printf("get: %p %p\n", canvas, image);
//    stride = pixman_image_get_stride(surface);

//    printf("--> get_aspeed=%p\n", image->u.ast.data);//, image->u.ast.data->chunk[0].data);
#if !_ALONE
    dec->buf = image->u.ast.data->chunk[0].data;
    len = image->u.ast.data_size;
    if (len <= 88) {
        free(dec);
        return NULL;
    }
    orig = malloc(len);
    memcpy(orig, image->u.ast.data->chunk[0].data, len);
//    dump((void *)dec->buf, 96, 0);

    hdr = (struct ASTHeader *)dec->buf;
    width = hdr->src_mode_x;
    height = hdr->src_mode_y;
    printf("### decode_frame(%zd, %x): %dx%d (%dx%d) p=%p\n", len, hdr->comp_size, width, height, hdr->src_mode_x, hdr->src_mode_y, orig);

    surface = surface_create(
#ifdef WIN32
                             canvas->dc,
#endif
                             PIXMAN_a8r8g8b8,
                             width, height, FALSE);
    if (surface == NULL) {
        spice_warning("create surface failed");
        return NULL;
    }

    dest = (uint8_t *)pixman_image_get_data(surface);
    memset(dest - width*(height-1)*4, 0, width*(height-1)*4);
#else
    fp = fopen("videocap.data", "r");
    if (!fp) return 2;

    len = 552;
    dec->buf = malloc(len);

    fread(dec->buf, len, 1, fp);
    fclose(fp);

    if (len <= 88) {
        free(dec);
        return 1;
    }
    orig = malloc(len);
    memcpy(orig, dec->buf, len);
//    dump((void *)dec->buf, 96, 0);

    hdr = (struct ASTHeader *)dec->buf;
    width = hdr->src_mode_x;
    height = hdr->src_mode_y;
    printf("### decode_frame(%zd, %x): %dx%d (%dx%d) p=%p dec=%p\n", len, hdr->comp_size, width, height, hdr->src_mode_x, hdr->src_mode_y, orig, dec);

    dest = calloc(width*(height)*4, 1) + width*(height)*4;
    memset(dest - width*(height-1)*4, 0, width*(height-1)*4);
    printf("get dest=%p\n", dest);
#endif

    dec->buf += 88 >> 2;

    j = hdr->comp_size >> 2;

    dec->m_decodeBuf = (void *)dest;
    dec->length = j;
    dec->_index = BLOCK_START_LENGTH;
    dec->m_newbits = 32;
    dec->txb = dec->tyb = 0;
    dec->byte_pos = 0;
    dec->selector = hdr->jpeg_table;
    dec->advance_selector = hdr->adv_table;
    dec->Mapping = hdr->jpeg_yuv;
    dec->m_YDC_nr = 0;
    dec->m_CbDC_nr = 1;
    dec->m_CrDC_nr = 1;
    dec->m_YAC_nr = 0;
    dec->m_CbAC_nr = 1;
    dec->m_CrAC_nr = 1;

    for (i = 1; i < POWEROFTOW; i++) {
        dec->neg_pow2[i] = (short)(1.0 - pow(2, i));
    }

    dec->m_DCY[0] = dec->DCCb[0] = dec->DCCr[0];

    dec->SCALEFACTOR = dec->SCALEFACTORUV = 16;
    dec->ADVANCESCALEFACTOR = dec->ADVANCESCALEFACTORUV = 16;

    for (int l = 0; l < 4; l++)
        dec->m_VQ.Index[l] = l;

    dec->m_VQ.Color[0] = 32896L;
    dec->m_VQ.Color[1] = 0xff8080L;
    dec->m_VQ.Color[2] = 0x808080L;
    dec->m_VQ.Color[3] = 0xc08080L;

    dec->WIDTH = hdr->src_mode_x;
    dec->HEIGHT = hdr->src_mode_y;
    dec->RealWIDTH = hdr->src_mode_x;
    dec->RealHEIGHT = hdr->src_mode_y;
    dec->m_Mode420 = hdr->mode420;
    if (dec->m_Mode420 == 1) {
        if (dec->WIDTH % 16 != 0)
            dec->WIDTH = (dec->WIDTH + 16) - dec->WIDTH % 16;
        if (dec->HEIGHT % 16 != 0)
            dec->HEIGHT = (dec->HEIGHT + 16) - dec->HEIGHT % 16;
    } else {
        if (dec->WIDTH % 8 != 0)
            dec->WIDTH = (dec->WIDTH + 8) - dec->WIDTH % 8;
        if (dec->HEIGHT % 8 != 0)
            dec->HEIGHT = (dec->HEIGHT + 8) - dec->HEIGHT % 8;
    }

    dec->tmp_WIDTHBy16 = hdr->dst_mode_x;
    dec->tmp_HEIGHTBy16 = hdr->dst_mode_y;
    if (dec->m_Mode420 == 1) {
        if (dec->tmp_WIDTHBy16 % 16 != 0)
            dec->tmp_WIDTHBy16 = (dec->tmp_WIDTHBy16 + 16) - dec->tmp_WIDTHBy16 % 16;
        if (dec->tmp_HEIGHTBy16 % 16 != 0)
            dec->tmp_HEIGHTBy16 = (dec->tmp_HEIGHTBy16 + 16) - dec->tmp_HEIGHTBy16 % 16;
    } else {
        if (dec->tmp_WIDTHBy16 % 8 != 0)
            dec->tmp_WIDTHBy16 = (dec->tmp_WIDTHBy16 + 8) - dec->tmp_WIDTHBy16 % 8;
        if (dec->tmp_HEIGHTBy16 % 8 != 0)
            dec->tmp_HEIGHTBy16 = (dec->tmp_HEIGHTBy16 + 8) - dec->tmp_HEIGHTBy16 % 8;
    }

    loadLuminanceQuantizationTable(dec, dec->m_QT[0]);
    loadChrominanceQuantizationTable(dec, dec->m_QT[1]);
    loadPass2LuminanceQuantizationTable(dec, dec->m_QT[2]);
    loadPass2ChrominanceQuantizationTable(dec, dec->m_QT[3]);

    int k = 0;
    do {
        switch ((GET_LONG(dec->buf[0]) >> 28) & BLOCK_HEADER_MASK) {
        case JPEG_NO_SKIP_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            decompressJPEG(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case JPEG_SKIP_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            decompressJPEG(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case JPEG_NO_SKIP_PASS2_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            decompressJPEGPass2(dec, dec->txb, dec->tyb, 2);
            moveBlockIndex(dec);
            break;
        case JPEG_SKIP_PASS2_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            decompressJPEGPass2(dec, dec->txb, dec->tyb, 2);
            moveBlockIndex(dec);
            break;
        case VQ_NO_SKIP_1_COLOR_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            dec->m_VQ.BitMapBits = 0;
            for (int i1 = 0; i1 < 1; i1++) {
                dec->m_VQ.Index[i1] = (GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK;
                if (((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER) {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else {
                    dec->m_VQ.Color[dec->m_VQ.Index[i1]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }

                decompressVQ(dec, dec->txb, dec->tyb, 0);
                moveBlockIndex(dec);
            }
            break;
        case VQ_SKIP_1_COLOR_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            dec->m_VQ.BitMapBits = 0;
            for(int j1 = 0; j1 < 1; j1++)
            {
                dec->m_VQ.Index[j1] = (int32_t)((GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK);
                if(((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER)
                {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else
                {
                    dec->m_VQ.Color[dec->m_VQ.Index[j1]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }
            }

            decompressVQ(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case VQ_NO_SKIP_2_COLOR_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            dec->m_VQ.BitMapBits = 1;
            for(int k1 = 0; k1 < 2; k1++)
            {
                dec->m_VQ.Index[k1] = (int32_t)((GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK);
                if(((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER)
                {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else
                {
                    dec->m_VQ.Color[dec->m_VQ.Index[k1]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }
            }
            decompressVQ(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case VQ_SKIP_2_COLOR_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            dec->m_VQ.BitMapBits = 1;
            for(int l1 = 0; l1 < 2; l1++)
            {
                dec->m_VQ.Index[l1] = (int32_t)((GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK);
                if(((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER)
                {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else
                {
                    dec->m_VQ.Color[dec->m_VQ.Index[l1]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }
            }

            decompressVQ(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case VQ_NO_SKIP_4_COLOR_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            dec->m_VQ.BitMapBits = 2;
            for(int i2 = 0; i2 < 4; i2++)
            {
                dec->m_VQ.Index[i2] = (int32_t)((GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK);
                if(((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER)
                {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else
                {
                    dec->m_VQ.Color[dec->m_VQ.Index[i2]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }
            }

            decompressVQ(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case VQ_SKIP_4_COLOR_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            dec->m_VQ.BitMapBits = 2;
            for(int j2 = 0; j2 < 4; j2++)
            {
                dec->m_VQ.Index[j2] = (int32_t)((GET_LONG(dec->buf[0]) >> 29) & VQ_INDEX_MASK);
                if(((GET_LONG(dec->buf[0]) >> 31) & VQ_HEADER_MASK) == VQ_NO_UPDATE_HEADER)
                {
                    updateReadBuf(dec, VQ_NO_UPDATE_LENGTH);
                } else
                {
                    dec->m_VQ.Color[dec->m_VQ.Index[j2]] = (GET_LONG(dec->buf[0]) >> 5) & VQ_COLOR_MASK;
                    updateReadBuf(dec, VQ_UPDATE_LENGTH);
                }
            }

            decompressVQ(dec, dec->txb, dec->tyb, 0);
            moveBlockIndex(dec);
            break;
        case LOW_JPEG_NO_SKIP_CODE:
            updateReadBuf(dec, BLOCK_AST2100_START_LENGTH);
            decompressJPEG(dec, dec->txb, dec->tyb, 2);
            moveBlockIndex(dec);
            break;
        case LOW_JPEG_SKIP_CODE:
            dec->txb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff00000L) >> 20);
            dec->tyb = (int32_t)((GET_LONG(dec->buf[0]) & 0xff000L) >> 12);
            updateReadBuf(dec, BLOCK_AST2100_SKIP_LENGTH);
            decompressJPEG(dec, dec->txb, dec->tyb, 2);
            moveBlockIndex(dec);
            break;
        case FRAME_END_CODE:
            goto done;

        default:
            printf("Unknow Marco Block type %08lx (%08lx) k=%d\n", GET_LONG(dec->buf[0]) >> 28, GET_LONG(dec->buf[0]), k);
            moveBlockIndex(dec);
            dump(orig, len, 1);
            goto done;
        }
        // m_view.repaint((txb - 1) * 16, tyb * 16, 16, 16);
        k++;
//        if (k == 5) exit(1);
    } while (dec->_index < j);

done:
    free(orig);
    free(dec);
#if _ALONE
    return 0;
#else
    return surface;
#endif
}
