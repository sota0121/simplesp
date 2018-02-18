﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_BINALIZATION_H__
#define __SP_BINALIZATION_H__

#include "spcore/spcore.h"
#include "spapp/spimg/spfilter.h"

namespace sp{

    SP_CPUFUNC void binalize(Mem2<Byte> &dst, const Mem2<Byte> &src, const int thresh, const bool inv = false){
        dst.resize(src.dsize);
        const Mem2<Byte> &tmp = (&dst != &src) ? src : clone(src);

        if (inv == false) {
            for (int i = 0; i < dst.size(); i++) {
                dst[i] = (tmp[i] >= thresh) ? 255 : 0;
            }
        }
        else {
            for (int i = 0; i < dst.size(); i++) {
                dst[i] = (tmp[i] < thresh) ? 255 : 0;
            }
        }

    }

    SP_CPUFUNC void binalizeAdapt(Mem2<Byte> &dst, const Mem2<Byte> &src, const bool inv = false){

        int hist[256] = { 0 };
        for (int i = 0; i < src.size(); i++) {
            hist[src[i]]++;
        }

        int thresh = 0;
        double maxEval = 0.0;
        for (int t = 1; t < 256; t++) {
            double cnt0 = 0.0;
            double sum0 = 0.0;
            for (int i = 0; i < t; i++) {
                cnt0 += hist[i];
                sum0 += i * hist[i];
            }
            if (cnt0 == 0) continue;

            double cnt1 = 0.0;
            double sum1 = 0.0;
            for (int i = t; i < 256; i++) {
                cnt1 += hist[i];
                sum1 += i * hist[i];
            }
            if (cnt1 == 0) continue;

            const double mean0 = sum0 / cnt0;
            const double mean1 = sum1 / cnt1;

            const double eval = cnt0 * cnt1 * (mean0 - mean1) * (mean0 - mean1);
            if (eval > maxEval) {
                maxEval = eval;
                thresh = t;
            }
        }
        SP_PRINTD("binalizeAdapt thresh %d\n", thresh);
        binalize(dst, src, thresh, inv);
    }

    SP_CPUFUNC void binalizeBlock(Mem2<Byte> &dst, const Mem2<Byte> &src, const int blockSize, const bool inv = false){
        dst.resize(src.dsize);
        const Mem2<Byte> &tmp = (&dst != &src) ? src : clone(src);

        const int step = src.dsize[0];

        const Byte *pSrc = tmp.ptr;
        Byte *pDst = dst.ptr;

        for (int v = 0; v < dst.dsize[1]; v += blockSize){
            for (int u = 0; u < dst.dsize[0]; u += blockSize){
                const int sizeX = minVal(u + blockSize, src.dsize[0]);
                const int sizeY = minVal(v + blockSize, src.dsize[1]);

                Byte maxv = 0;
                Byte minv = SP_BYTEMAX;

                const int margin = blockSize / 2;
                for (int y = v - margin; y < sizeY + margin; y++){
                    for (int x = u - margin; x < sizeX + margin; x++){
                        const Byte val = pSrc[y * step + x];
                        maxv = maxVal(maxv, val);
                        minv = minVal(minv, val);
                    }
                }

                const int thresh = (maxv + minv) / 2;
                if (inv == false) {
                    for (int y = v; y < sizeY; y++) {
                        for (int x = u; x < sizeX; x++) {
                            pDst[y * step + x] = (pSrc[y * step + x] >= thresh) ? 255 : 0;
                        }
                    }
                }
                else {
                    for (int y = v; y < sizeY; y++) {
                        for (int x = u; x < sizeX; x++) {
                            pDst[y * step + x] = (pSrc[y * step + x] < thresh) ? 255 : 0;
                        }
                    }
                }
            }
        }
    }


    //--------------------------------------------------------------------------------
    // labeling
    //--------------------------------------------------------------------------------

    SP_CPUFUNC int labeling(Mem2<int> &map, const Mem2<Byte> &bin, const bool near8 = false){
        
        const int step = bin.dsize[0];

        map.resize(bin.dsize);

        Mem1<int> table;
        table.reserve(bin.size());

        const Rect rect = getRect2(bin.dsize);

        const int linkNum = (near8 == true) ? 4 : 2;
        const int link[][2] = { { -1, 0 }, { 0, -1 }, { -1, -1 }, { +1, -1 } };

        const Byte *pBin = bin.ptr;
        int *pMap = map.ptr;
        int *pTable = table.ptr;

        for (int v = 0; v < bin.dsize[1]; v++){
            for (int u = 0; u < bin.dsize[0]; u++){
                pMap[v * step + u] = -1;
                if (pBin[v * step + u] == 0) continue;

                int crntLabel = table.size();

                int *pTmp[4] = { 0 };

                // check min label
                for (int i = 0; i < linkNum; i++){
                    const int ru = u + link[i][0];
                    const int rv = v + link[i][1];

                    if (isInRect2(rect, ru, rv) == false) continue;
                    if (pBin[rv * step + ru] == 0) continue;

                    const int refLabel = table[pMap[rv * step + ru]];
                    pTmp[i] = &pMap[rv * step + ru];
                    if (refLabel < crntLabel){
                        crntLabel = refLabel;
                    }
                }

                if (crntLabel == table.size()){
                    table.push(crntLabel);
                }
                else{
                    for (int i = 0; i < linkNum; i++){
                        if (pTmp[i] != NULL) {
                            pTable[*pTmp[i]] = crntLabel;
                        }
                    }
                }

                pMap[v * step + u] = crntLabel;
            }
        }

        const int tableNum = table.size();

        // update table
        for (int i = 0; i < tableNum; i++){
            int p = i;
            while (pTable[p] != p){
                p = pTable[p];
            }
            pTable[i] = p;
        }

        int labelNum = 0;

        // update label
        int maxv = -1;
        for (int i = 0; i < tableNum; i++){
            if (pTable[i] > maxv){
                maxv = pTable[i];

                for (int j = i; j < tableNum; j++){
                    if (pTable[j] == maxv){
                        pTable[j] = labelNum;
                    }
                }
                labelNum++;
            }
        }

        // update map
        for (int i = 0; i < bin.size(); i++){
            int &id = pMap[i];
            if (id < 0) continue;

            id = pTable[id];
        }

        return labelNum;
    }

    SP_CPUFUNC Mem1<Rect> getLabelRect(const Mem2<int> &map){
        const int labelNum = round(maxVal(map) + 1);
        const int step = map.dsize[0];

        Mem1<Rect> dst(labelNum);
        dst.zero();

        const int *pMap = map.ptr;
        Rect *pDst = dst.ptr;

        for (int v = 0; v < map.dsize[1]; v++){
            for (int u = 0; u < map.dsize[0]; u++){
                const int id = *pMap++;
                if (id < 0) continue;
                Rect &rect = pDst[id];
                if (rect.dim == 0) {
                    rect = getRect2(u, v, 1, 1);
                }
                else {
                    const int ru = rect.dbase[0];
                    const int rv = rect.dbase[1];
                    const int rw = rect.dsize[0];
                    const int rh = rect.dsize[1];

                    rect.dbase[0] = minVal(u, ru);
                    rect.dsize[0] = maxVal(u + 1, ru + rw) - rect.dbase[0];

                    rect.dbase[1] = minVal(v, rv);
                    rect.dsize[1] = maxVal(v + 1, rv + rh) - rect.dbase[1];
                }
            }
        }
        return dst;
    }

    SP_CPUFUNC Mem1< Mem1<Vec2> > getLabelContour(const Mem2<int> &map, const bool useImgFrame = false){

        Mem1<Rect> rects = getLabelRect(map);

        // 8 nears clockwise search
        const int order[8][2] = {
            { -1, -1 }, { 0, -1 }, { +1, -1 }, { +1, 0 },
            { +1, +1 }, { 0, +1 }, { -1, +1 }, { -1, 0 }
        };
        const int start[3][3] = {
            { 0, 1, 2 },
            { 7, 0, 3 },
            { 6, 5, 4 }
        };

        Mem1< Mem1<Vec2> > dst(rects.size());

        for (int i = 0; i < dst.size(); i++){
            const Rect rect = rects[i];

            if (useImgFrame == false){
                if (isInRect(getRect2(map.dsize) - 1, rect) == false) continue;
            }

            int sx = 0;
            int sy = 0;
            for (int y = rect.dbase[1]; y < rect.dbase[1] + rect.dsize[1]; y++){
                for (int x = rect.dbase[0]; x < rect.dbase[0] + rect.dsize[0]; x++){
                    if (map(x, y) == i){
                        sx = x;
                        sy = y;
                        goto _exit;
                    }
                }
            }
        _exit:;

            int vec[2] = { +1, 0 };
            int cx = sx;
            int cy = sy;

            while (1){
                const int s = (start[vec[1] + 1][vec[0] + 1] + 8 - 2) % 8;

                for (int j = 0; j < 8; j++){
                    const int x = cx + order[(s + j) % 8][0];
                    const int y = cy + order[(s + j) % 8][1];
                    if (isInRect2(rect, x, y) == true && map(x, y) == i){
                        vec[0] = x - cx;
                        vec[1] = y - cy;

                        cx = x;
                        cy = y;
                        break;
                    }
                }
                dst[i].push(getVec(cx, cy));

                if (cx == sx && cy == sy){
                    break;
                }
            }
        }

        return dst;
    }


}

#endif