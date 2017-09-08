﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_COM_H__
#define __SP_COM_H__


//--------------------------------------------------------------------------------
// system
//--------------------------------------------------------------------------------

#ifndef SP_USE_DEBUG
#define SP_USE_DEBUG 0
#endif

#ifndef SP_USE_CONSOLE
#define SP_USE_CONSOLE 1
#endif

#ifndef SP_USE_ASSERT
#define SP_USE_ASSERT 1
#endif

#ifndef SP_USE_WRAPPER
#define SP_USE_WRAPPER 1
#endif

#ifndef SP_USE_OMP
#define SP_USE_OMP 1
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
// base struct
//--------------------------------------------------------------------------------

namespace sp{

	//--------------------------------------------------------------------------------
	// type
	//--------------------------------------------------------------------------------
	
	typedef unsigned char Byte;


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
	// vertex and normal
	//--------------------------------------------------------------------------------
	
	struct VecVN2{
		Vec2 vtx, nrm;
	};

	struct VecVN3{
		Vec3 vtx, nrm;
	};


	//--------------------------------------------------------------------------------
	// triangle mesh
	//--------------------------------------------------------------------------------
	
	struct Mesh{
		Vec3 vtx[3];
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
	// byte order
	//--------------------------------------------------------------------------------
	
	enum ByteOrder {
		BigEndian,
		LittleEndian,
	};

}

#endif