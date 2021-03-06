﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_PLY_H__
#define __SP_PLY_H__

#include "spapp/spdata/spfile.h"
#include "spapp/spdata/spstr.h"

namespace sp{

    //--------------------------------------------------------------------------------
    // local method
    //--------------------------------------------------------------------------------
    
    using namespace std;

    namespace _ply{
        struct Element {
            int num;
            string name;
            Mem1<string> prop;

            Element() {
                num = 0;
            }
            Element(const Element &elem) {
                *this = elem;
            }

            void operator = (const Element &elem) {
                num = elem.num;
                name = elem.name;
                prop = elem.prop;
            }
        };

        struct Vertex {
            Vec3 pos;
            Col3 col;

            Vertex() {
                pos = getVec(0.0, 0.0, 0.0);
                col = getCol(0, 0, 0);
            }
            Vertex(const Vertex &vtx) {
                *this = vtx;
            }

            void operator = (const Vertex &vtx) {
                pos = vtx.pos;
                col = vtx.col;
            }
        };

        SP_CPUFUNC bool loadHeader(File &file, Mem1<Element> &elems){
            elems.clear();

            Element *pElem = NULL;

            char line[SP_STRMAX];
            while (file.gets(line)){
                Mem1<string> words = strSplit(line);
                
                if (words[0] == "end_header") break;

                if (words[0] == "element"){
                    Element elem;

                    elem.name = words[1];
                    elem.num = atoi(words[2].c_str());

                    elems.push(elem);
                    pElem = &elems[elems.size() - 1];
                }

                if (words[0] == "property" && pElem){
                    pElem->prop.push(words[words.size() - 1]);
                }
            }

            for (int i = 0; i < elems.size(); i++){
                const Element &elem = elems[i];
                if (elem.num == 0 || elem.prop.size() == 0) return false;

                string prop;
                for (int p = 0; p < elem.prop.size(); p++){
                    prop += elem.prop[p] + "";
                }

                SP_PRINTD("elem %d %s\n", elem.num, elem.name.c_str());
                SP_PRINTD("prop %s\n", prop.c_str());
            }

            return true;
        }

        SP_CPUFUNC Vertex getVtx(const char *line, const Element &elem){
            Vertex vtx;

            const Mem1<string> words = strSplit(line);

            for (int p = 0; p < elem.prop.size(); p++){
                const double val = atof(words[p].c_str());

                if (elem.prop[p] == "x") vtx.pos.x = val;
                if (elem.prop[p] == "y") vtx.pos.y = val;
                if (elem.prop[p] == "z") vtx.pos.z = val;

                if (elem.prop[p] == "red") cnvVal(vtx.col.r, val);
                if (elem.prop[p] == "green") cnvVal(vtx.col.g, val);
                if (elem.prop[p] == "blue") cnvVal(vtx.col.b, val);
            }

            return vtx;
        }

        SP_CPUFUNC Mem1<int> getIdx(const char *line, const Element &elem){
            Mem1<int> idx;

            const Mem1<string> words = strSplit(line);

            for (int i = 0; i < atoi(words[0].c_str()); i++){
                idx.push(atoi(words[i + 1].c_str()));
            }

            return idx;
        }

        SP_CPUFUNC bool loadVtxs(File &file, Mem1<Vertex> &vtxs) {
            file.seek(SEEK_SET);

            Mem1<Element> elems;
            if (loadHeader(file, elems) == false) return false;

            for (int i = 0; i < elems.size(); i++) {
                const Element &elem = elems[i];

                char line[SP_STRMAX];
                if (elem.name == "vertex") {
                    for (int j = 0; j < elem.num; j++) {
                        file.gets(line);
                        vtxs.push(getVtx(line, elem));
                    }
                    continue;
                }
                else {
                    for (int j = 0; j < elem.num; j++) {
                        file.gets(line);
                    }
                }
            }
            return true;
        }

        SP_CPUFUNC bool loadIdxs(File &file, Mem1<Mem1<int>> &idxs) {
            file.seek(SEEK_SET);

            Mem1<Element> elems;
            if (loadHeader(file, elems) == false) return false;

            for (int i = 0; i < elems.size(); i++) {
                const Element &elem = elems[i];

                char line[SP_STRMAX];
                if (elem.name == "face") {
                    for (int j = 0; j < elem.num; j++) {
                        file.gets(line);
                        idxs.push(getIdx(line, elem));
                    }
                }
                else{
                    for (int j = 0; j < elem.num; j++) {
                        file.gets(line);
                    }
                }
            }
            return true;
        }
    }
    using namespace _ply;

    SP_CPUFUNC bool loadPLY(const char *path, Mem1<Mesh3> &meshes){
        File file;
        if (file.open(path, "r") == false) return false;

        Mem1<Vertex> vtxs;
        Mem1<Mem1<int>> idxs;

        if (loadVtxs(file, vtxs) == false) return false;
        if (loadIdxs(file, idxs) == false) return false;

        meshes.clear();
        for (int i = 0; i < idxs.size(); i++){
            const Mem1<int> &idx = idxs[i];
            if (idx.size() == 3) {
                meshes.push(getMesh(vtxs[idx[0]].pos, vtxs[idx[1]].pos, vtxs[idx[2]].pos));
            }
            if (idx.size() == 4) {
                meshes.push(getMesh(vtxs[idx[0]].pos, vtxs[idx[1]].pos, vtxs[idx[2]].pos));
                meshes.push(getMesh(vtxs[idx[0]].pos, vtxs[idx[2]].pos, vtxs[idx[3]].pos));
            }
        }
        return true;
    }

    SP_CPUFUNC bool loadPLY(const char *path, Mem1<Vec3> &pnts) {
        File file;
        if (file.open(path, "r") == false) return false;

        Mem1<Vertex> vtxs;
        if (loadVtxs(file, vtxs) == false) return false;


        pnts.clear();
        for (int i = 0; i < vtxs.size(); i++) {
            pnts.push(vtxs[i].pos);
        }
        return true;
    }

    SP_CPUFUNC bool loadPLY(const char *path, Mem1<Vec3> &pnts, Mem1<Col3> &cols) {
        File file;
        if (file.open(path, "r") == false) return false;

        Mem1<Vertex> vtxs;
        if (loadVtxs(file, vtxs) == false) return false;

        pnts.clear();
        cols.clear();
        for (int i = 0; i < vtxs.size(); i++) {
            pnts.push(vtxs[i].pos);
            cols.push(vtxs[i].col);
        }
        return true;
    }

    SP_CPUFUNC bool savePLY(const char *path, const Mem1<Mesh3> &meshes) {
        File file;
        if (file.open(path, "w") == false) return false;

        file.printf("ply\n");
        file.printf("format ascii 1.0\n");
        file.printf("element vertex %d\n", meshes.size() * 3);
        file.printf("property float x\n");
        file.printf("property float y\n");
        file.printf("property float z\n");
        file.printf("element face %d\n", meshes.size());
        file.printf("property list uchar int vertex_indices\n");
        file.printf("end_header\n");

        for (int i = 0; i < meshes.size(); i++) {
            for (int j = 0; j < 3; j++) {
                file.printf("%lf %lf %lf ", meshes[i].pos[j].x, meshes[i].pos[j].y, meshes[i].pos[j].z);
                file.printf("\n");
            }
        }
        for (int i = 0; i < meshes.size(); i++) {
            file.printf("3 %d %d %d ", 3 * i + 0, 3 * i + 1, 3 * i + 2);
            file.printf("\n");
        }
        return true;
    }

    SP_CPUFUNC bool savePLY(const char *path, const Mem1<Vec3> &pnts){
        File file;
        if (file.open(path, "w") == false) return false;

        file.printf("ply\n");
        file.printf("format ascii 1.0\n");
        file.printf("element vertex %d\n", pnts.size());
        file.printf("property float x\n");
        file.printf("property float y\n");
        file.printf("property float z\n");
        file.printf("end_header\n");

        for (int i = 0; i < pnts.size(); i++){
            file.printf("%lf %lf %lf ", pnts[i].x, pnts[i].y, pnts[i].z);
            file.printf("\n");
        }

        return true;
    }

    SP_CPUFUNC bool savePLY(const char *path, const Mem1<Vec3> &pnts, const Mem1<Col3> &cols){
        File file;
        if (file.open(path, "w") == false) return false;
        if (cmpSize(pnts, cols) == false) return false;

        file.printf("ply\n");
        file.printf("format ascii 1.0\n");
        file.printf("element vertex %d\n", pnts.size());
        file.printf("property float x\n");
        file.printf("property float y\n");
        file.printf("property float z\n");
        file.printf("property uchar red\n");
        file.printf("property uchar green\n");
        file.printf("property uchar blue\n");
        file.printf("end_header\n");

        for (int i = 0; i < pnts.size(); i++){
            file.printf("%lf %lf %lf ", pnts[i].x, pnts[i].y, pnts[i].z);
            file.printf("%d %d %d ", cols[i].r, cols[i].g, cols[i].b);
            file.printf("\n");
        }

        return true;
    }
}

#endif