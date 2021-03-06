﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

// [reference]
// C. Harris, and M. Stephens,
// "A combined corner and edge detector",
// In Alvey Vision Conference, 1988


#ifndef __SP_CORNER_H__
#define __SP_CORNER_H__

#include "spcore/spcore.h"
#include "spapp/spimg/spfilter.h"
#include "spapp/spimgex/spfeature.h"

namespace sp{

    SP_CPUFUNC void harris(Mem1<Vec2> &pixs, const Mem2<Byte> &src, const int block = 4) {

        const double RESP_RATE = 0.9;

        pixs.clear();

        // calc response
        Mem2<float> rmap(src.dsize);
        {
            Mem2<float> sobelX, sobelY;
            sobelFilter3x3(sobelX, sobelY, src);

            struct Gnn{
                float gxx, gxy, gyy;
            };

            Mem2<Gnn> dmap(src.dsize);
            for (int i = 0; i < dmap.size(); i++){
                const float gx = sobelX[i];
                const float gy = sobelY[i];

                dmap[i].gxx = gx * gx;
                dmap[i].gxy = gx * gy;
                dmap[i].gyy = gy * gy;
            }

            boxFilter3x3<Gnn, float>(dmap, dmap);

            const double k = 0.04;
            for (int i = 0; i < rmap.size(); i++){
                const float gxx = dmap[i].gxx;
                const float gxy = dmap[i].gxy;
                const float gyy = dmap[i].gyy;

                rmap[i] = static_cast<float>((gxx * gyy - gxy * gxy) - k * (gxx + gyy) * (gxx + gyy));
            }
        }

        // non-maximal suppression
        {
            // macro block
            Mem2<double> bimg((rmap.dsize[0] + block - 1) / block, (rmap.dsize[1] + block - 1) / block);
            bimg.zero();

            for (int y = 0; y < bimg.dsize[1]; y++){
                for (int x = 0; x < bimg.dsize[0]; x++){
                    double &maxv = bimg(x, y);

                    const int eu = minVal(rmap.dsize[0], (x + 1) * block);
                    const int ev = minVal(rmap.dsize[1], (y + 1) * block);
                
                    for (int v = y * block; v < ev; v++){
                        for (int u = x * block; u < eu; u++){
                            maxv = maxVal(maxv, rmap(u, v));
                        }
                    }
                }
            }

            Mem<double> list = bimg;
            sort(list);
            const double thresh = list[round(RESP_RATE * (list.size() - 1))];

            maxFilter(bimg, bimg, 3);

            for (int y = 0; y < bimg.dsize[1]; y++){
                for (int x = 0; x < bimg.dsize[0]; x++){
                    const double maxv = bimg(x, y);
                    if (maxv < thresh) continue;

                    const int eu = minVal(rmap.dsize[0], (x + 1) * block);
                    const int ev = minVal(rmap.dsize[1], (y + 1) * block);

                    for (int v = y * block; v < ev; v++){
                        for (int u = x * block; u < eu; u++){
                            if (rmap(u, v) < maxv) continue;
                        
                            pixs.push(getVec(u, v));
                            goto _exit;
                        }
                    }
                _exit:;
                }
            }
        }
    }

    SP_CPUFUNC void harris(Mem1<Vec2> &pixs, const Mem2<Col3> &src, const int block = 4) {
        Mem2<Byte> gry;
        cnvImg(gry, src);

        harris(pixs, gry, block);
    }
}

#endif