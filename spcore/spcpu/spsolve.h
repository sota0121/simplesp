﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_SOLVE_H__
#define __SP_SOLVE_H__

#include "spcore/spcpu/spmop.h"
#include "spcore/spcpu/spstat.h"


namespace sp{

    //--------------------------------------------------------------------------------
    // solve util
    //--------------------------------------------------------------------------------

    class NrmData{

    public:

        int dim;

        // data matrix
        Mat V;

        // transform matrix
        Mat T;

        NrmData(const int dim){
            this->dim = dim;
        }

        NrmData(const NrmData &data){
            *this = data;
        }

        NrmData& operator = (const NrmData &data){
            dim = data.dim;
            V = data.V;
            T = data.T;
            return *this;
        }

        bool cnvData(const void *ptr, const int size) {
            if (size == 0) return false;

            V.resize(size, dim, ptr);

            const Mat mean = meanVal(V, 0);

            V -= mean;

            // calc scale
            double scale = meanSqrt(sumSq(V, 1));

            V /= scale;

            if (scale < SP_SMALL) return false;

            T = eyeMat(dim + 1, dim + 1);
            for (int c = 0; c < dim; c++) {
                T(c, c) /= scale;
                T(c, dim) -= mean(0, c) / scale;
            }

            return true;
        }

        template<typename TYPE>
        bool cnvData(const Mem<TYPE> &mem){
            return cnvData(mem.ptr, mem.size());
        }

    };

    SP_CPUFUNC Mat calcAtWeight(const Mat &A, const Mem<double> errs = Mem<double>(), const double minErr = 1.0){
        Mat AtW = trnMat(A);

        if (errs.size() > 0){
            const int num = AtW.cols() / errs.size();

            const double median = medianVal(errs);
            const double thresh = 5.0 * maxVal(median, minErr);

            for (int r = 0; r < AtW.rows(); r++){
                for (int c = 0; c < AtW.cols(); c++){
                    AtW(r, c) *= funcTukey(errs[c / num], thresh);
                }
            }
        }
        return AtW;
    }

    // solve equation (A * X = B)
    SP_CPUFUNC bool solveEq(Mat &result, const Mat &A, const Mat &B, const Mem<double> errs = Mem<double>(), const double minErr = 1.0){
        
        const Mat AtW = calcAtWeight(A, errs, minErr);

        result = invMat(AtW * A) * AtW * B;
        return (result.size() > 0) ? true : false;
    }

    // solve equation (A * X = 0)
    SP_CPUFUNC bool solveEqZero(Mat &result, const Mat &A, const Mem<double> errs = Mem<double>(), const double minErr = 1.0){
        
        const Mat AtW = calcAtWeight(A, errs, minErr);

        Mat eigVec, eigVal;
        if (eigMat(eigVec, eigVal, AtW * A, true) == false) return false;

        result.resize(eigVec.rows(), 1);
        for (int i = 0; i < eigVec.rows(); i++) {
            result[i] = eigVec(i, 0);
        }
        return true;
    }

    //--------------------------------------------------------------------------------
    // ransac util
    //--------------------------------------------------------------------------------

    // ransac sampling max
#define SP_RANSAC_ITMAX 1000
#define SP_RANSAC_NUM 5
#define SP_RANSAC_RATE 0.1

    // ransac adaptive stop
    SP_CPUFUNC int adaptiveStop(const double rate, const int num, const double n = 0.99){
        const double e = maxVal(rate, 0.1);
        const int k = round(1.0 + log(1.0 - n) / log(1.0 - pow(e, num)));
        return minVal(k, SP_RANSAC_ITMAX);
    }




}
#endif