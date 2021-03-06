﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_COM_H__
#define __SP_COM_H__


//--------------------------------------------------------------------------------
// system
//--------------------------------------------------------------------------------

#ifndef SP_USE_ASSERT
#define SP_USE_ASSERT 1
#endif

#ifndef SP_USE_OMP
#define SP_USE_OMP 1
#endif

#ifndef SP_USE_THREAD
#define SP_USE_THREAD 0
#endif

//--------------------------------------------------------------------------------
// call
//--------------------------------------------------------------------------------

// cpu function
#ifndef SP_CPUFUNC
#define SP_CPUFUNC inline
#endif

// cpu/gpu function
#ifndef SP_GENFUNC
#define SP_GENFUNC inline
#endif


//--------------------------------------------------------------------------------
// const value
//--------------------------------------------------------------------------------

// pi
#define SP_PI 3.14159265358979323846

// limit value regarded as zero
#define SP_SMALL (1.0e-20)

// limit value regarded as infinity
#define SP_INFINITY (1.0e+20)

// maximal value (unsigned char)
#define SP_BYTEMAX 255

// maximal value (int)
#define SP_INTMAX ((int)(((unsigned)-1)/2))

// maximal string
#define SP_STRMAX 512

// maximal dimension
#define SP_DIMMAX 3


//--------------------------------------------------------------------------------
// once 
//--------------------------------------------------------------------------------

#define SP_ONCE(func) { static bool once = true; if(once){ once = false; func; } }


//--------------------------------------------------------------------------------
// shader
//--------------------------------------------------------------------------------

#define SP_SHADER(...)  #__VA_ARGS__



//--------------------------------------------------------------------------------
// base struct
//--------------------------------------------------------------------------------

namespace sp{

    //--------------------------------------------------------------------------------
    // type
    //--------------------------------------------------------------------------------
    
    typedef unsigned char Byte;


    //--------------------------------------------------------------------------------
    // complex
    //--------------------------------------------------------------------------------

    struct Cmp {
        double re, im;
    };


    //--------------------------------------------------------------------------------
    // memory ptr
    //--------------------------------------------------------------------------------

    template<typename TYPE> struct ExPtr{
        // pointer
        TYPE *ptr;

        // dimension
        int dim;

        // dimension size
        int dsize[SP_DIMMAX];
    };


    //--------------------------------------------------------------------------------
    // rectangle
    //--------------------------------------------------------------------------------

    struct Rect{
        // dimension
        int dim;

        // dimension base
        int dbase[SP_DIMMAX];

        // dimension size
        int dsize[SP_DIMMAX];
    };


    //--------------------------------------------------------------------------------
    // vector
    //--------------------------------------------------------------------------------

    struct Vec2{
        double x, y;
    };

    struct Vec3{
        double x, y, z;
    };


    //--------------------------------------------------------------------------------
    // position and normal
    //--------------------------------------------------------------------------------
    
    struct VecPN2{
        Vec2 pos, nrm;
    };

    struct VecPN3{
        Vec3 pos, nrm;
    };


    //--------------------------------------------------------------------------------
    // triangle mesh
    //--------------------------------------------------------------------------------
    
    struct Mesh2{
        Vec2 pos[3];
    };

    struct Mesh3 {
        Vec3 pos[3];
    };

    //--------------------------------------------------------------------------------
    // 3d transform
    //--------------------------------------------------------------------------------

    struct Rot{
        // quaternion
        double qx, qy, qz, qw;
    };

    struct Pose{
        // rotation
        Rot rot;

        // translation
        Vec3 trn;
    };


    //--------------------------------------------------------------------------------
    // camera parameter
    //--------------------------------------------------------------------------------

    struct CamParam{
        int dsize[2];
        double fx, fy;
        double cx, cy;
        double k1, k2, k3, p1, p2;
    };


    //--------------------------------------------------------------------------------
    // color
    //--------------------------------------------------------------------------------

    struct Col3{
        Byte r, g, b;
    };

    struct Col4 {
        Byte r, g, b, a;
    };


    //--------------------------------------------------------------------------------
    // disparity data
    //--------------------------------------------------------------------------------

    struct Disp {
        float disp;
        float eval;
    };


    //--------------------------------------------------------------------------------
    // byte order
    //--------------------------------------------------------------------------------
    
    enum ByteOrder {
        BigEndian,
        LittleEndian,
    };

}

#endif