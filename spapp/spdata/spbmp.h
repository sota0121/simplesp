﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_BMP_H__
#define __SP_BMP_H__

#include "spapp/spdata/spfile.h"
#include "spapp/spimg/spimg.h"

namespace sp{

    SP_CPUFUNC bool loadBMP(const char *path, Mem<Col3> &dst){
        if (cmpFileExt(path, "bmp, BMP") == false) return false;

        File file;
        if (file.open(path, "rb") == false) return false;

        char buf[54];
        file.read(buf, 54);

        int sizeW, sizeH;
        memcpy(&sizeW, &buf[18], sizeof(int));
        memcpy(&sizeH, &buf[22], sizeof(int));
        sizeW = getLittleEndian(sizeW);
        sizeH = getLittleEndian(sizeH);

        int bit;
        memcpy(&bit, &buf[28], sizeof(int));
        bit = getLittleEndian(bit);

        const int dsize[2] = { sizeW, sizeH };
        dst.resize(2, dsize);

        int ch = 0;
        if (bit == 8) ch = 1;
        if (bit == 24) ch = 3;
        
        if (ch == 0) return false;

        Mem1<Byte> line(sizeW * ch + (sizeW * ch) % 4);
        for (int v = 0; v < sizeH; v++) {
            file.read(line.ptr, line.size());
            for (int u = 0; u < sizeW; u++) {
                if (ch == 1) {
                    acs2(dst, u, sizeH - 1 - v) = getCol(line[u], line[u], line[u]);
                }
                if (ch == 3) {
                    acs2(dst, u, sizeH - 1 - v) = getCol(line[u * 3 + 2], line[u * 3 + 1], line[u * 3 + 0]);
                }
            }
        }

        return true;
    }

    SP_CPUFUNC bool loadBMP(const char *path, Mem<Byte> &dst){
        Mem2<Col3> col;
        if(loadBMP(path, col) == false) return false;

        cnvImg(dst, col);

        return true;
    }

    SP_CPUFUNC bool saveBMP(const char *path, const Mem<Col3> &src){
        File file;
        if (file.open(path, "wb") == false) return false;

        char buf[54] = { 0 };
        memcpy(&buf[0], "BM", sizeof(char) * 2);

        const int offset = getLittleEndian(static_cast<int>(54));
        memcpy(&buf[10], &offset, sizeof(int));

        const int header = getLittleEndian(static_cast<int>(40));
        memcpy(&buf[14], &header, sizeof(int));

        const int w = getLittleEndian(static_cast<int>(src.dsize[0]));
        const int h = getLittleEndian(static_cast<int>(src.dsize[1]));
        memcpy(&buf[18], &w, sizeof(int));
        memcpy(&buf[22], &h, sizeof(int));

        const short plane = getLittleEndian(static_cast<short>(2));
        memcpy(&buf[26], &plane, sizeof(short));

        const int bit = getLittleEndian(static_cast<int>(24));
        memcpy(&buf[28], &bit, sizeof(int));

        const int size = getLittleEndian(static_cast<int>(3 * src.size()));
        memcpy(&buf[34], &size, sizeof(int));

        file.write(buf, 54);

        Mem1<Byte> line(w * 3 + w % 4);
        for (int v = 0; v < h; v++){
            line.zero();
            for (int u = 0; u < w; u++){
                line[u * 3 + 0] = acs2(src, u, h - 1 - v).b;
                line[u * 3 + 1] = acs2(src, u, h - 1 - v).g;
                line[u * 3 + 2] = acs2(src, u, h - 1 - v).r;
            }
            file.write(line.ptr, line.size());
        }

        return true;
    }

    SP_CPUFUNC bool saveBMP(const char *path, const Mem<Byte> &src){
        Mem<Col3> col;
        cnvImg(col, src);
    
        return saveBMP(path, col);
    }


}

#endif