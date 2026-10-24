/*
 *  Inlines.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef ININES_H
#define ININES_H

#include <math.h>
#include <Accelerate/Accelerate.h>

#ifdef __SSE3__
	#include <pmmintrin.h>
#endif

// -----------------------------------------------------------------------------

#define M_2PI	M_PI * 2.0
#define M_NPI_2	M_PI_2 * -1.0
#define M_LN2_2	M_LN2 * 0.5

// -----------------------------------------------------------------------------

static inline Float32 linear(Float32 fract, Float32 x0, Float32 x1)
{
	return x0 + (x1 - x0) * fract;
}

// -----------------------------------------------------------------------------

static inline Float32 cubic(Float32 fract, Float32 xm1, Float32 x0, Float32 x1, Float32 x2)
{
	return x0 + 0.5 *	fract * (x1 - xm1 +
						fract * (4.0 * x1 + 2.0 * xm1 - 5.0 * x0 - x2 +
						fract * (3.0 * (x0 - x1) - xm1 + x2)));
}

// -----------------------------------------------------------------------------

//	0.0 <= x <= 1.0

//	quadUpNegative
static inline Float32 pow2(Float32 x)
{
	return x * x;
}

static inline Float32 pow3(Float32 x)
{
	return x * x * x;
}

static inline Float32 pow5(Float32 x)
{
	Float32	x2 = x * x;
	return	x2 * x2 * x;
}

// -----------------------------------------------------------------------------

static inline Float32 quadDownPositive(Float32 x)
{
	return 1.0 - pow2(x);
}

static inline Float32 quadDownNegative(Float32 x)
{
	Float32 invX = 1.0 - x;
	return  pow2(invX);
}

static inline Float32 quadUpPositive(Float32 x)
{
	Float32 invX = 1.0 - x;
	return 1.0 - pow2(invX);
}

static inline Float32 quadInOut(Float32 x)
{
	x *= 2.0;
	
	if (x >= 1.0)
		x = 0.5 + (1.0 - pow2(2.0 - x)) * 0.5;
	else
		x = pow2(x) * 0.5;
		
	return x;
}

static inline Float32 quadMix(Float32 x, Float32 y, Float32 m)
{
	Float32 left, right;
	
	left	= quadDownPositive(m);
	right	= quadUpPositive(m);
	
	return x * left + y * right;
}

// -----------------------------------------------------------------------------

//	0.5 - 1.0 - 2.0

static inline Float32 halfDouble(Float32 x)
{
	return pow(2.0, (x - 0.5) * 2.0);
}

// -----------------------------------------------------------------------------

//	-1.0 <= m <= 1.0

static inline Float32 hardClip(Float32 x)
{
	Float32 x1 = fabsf(x + 1.0);
	Float32 x2 = fabsf(x - 1.0);
	
	return 0.5 * (x1 - x2);
}

static inline Float32 sineClip(Float32 x)
{
	if (x >= 0.0)
		x = x * (2.0 - x);
	else
		x = x * (2.0 + x);
	
	return x;
}

// -----------------------------------------------------------------------------

static inline Float32 fastClamp_f(Float32 x, Float32 min, Float32 max)
{
	Float32 x1 = fabsf(x - min);
	Float32 x2 = fabsf(x - max);
	
	return (x1 + min + max - x2) * 0.5;
}

static inline Float32 clamp_f(Float32 x, Float32 min, Float32 max)
{
	return (x < min ? min : (x > max ? max : x));
}

static inline UInt32 clamp_i(UInt32 x, UInt32 min, UInt32 max)
{
	return (x < min ? min : (x > max ? max : x));
}

// -----------------------------------------------------------------------------

static inline Float32 sineShaper(Float32 x)
{
	return (1.5 * x) - (0.5 * x * x * x);
}

static inline Float32 absFastTanh(Float32 x)
{
	x *= 2.1;
	
    Float32 ax	= fabsf(x);
    Float32 t	= (6.0 + ax * (3.0 + ax));
	
    return x * t / (12.0 + ax * t);
}

static inline Float32 pow2FastTanh(Float32 x)
{
	Float32 p2 = pow2(x);
	return x * (27.0 + p2) / (27.0 + 9.0 * p2);
}

static inline Float32 expTanhShaper(Float32 x)
{
	return 1.0 - 2.0 / (exp(2.0 * x) + 1.0);
}

static inline Float32 sqrTanhShaper(Float32 x)
{
	return x / sqrt(1.0 + x * x);
}

// -----------------------------------------------------------------------------

//	y = -1	-> negative shape
//	y =  0	-> linear	shape
//	y =  1	-> positive shape

static inline Float32 shaperGain(Float32 x)
{
	return 2.0 * x / (1.0 - x);
}

//	function for a single call with build-in shaperGain.
static inline Float32 shaperNormalized(Float32 x, Float32 y)
{
	y = shaperGain(y);
	return (1.0 + y) * x / (1.0 + y * fabsf(x));
}

//	function for multiple (loop) calls with precalculated shaperGain.
static inline Float32 shaper(Float32 x, Float32 y)
{
	return (1.0 + y) * x / (1.0 + y * fabsf(x));
}

// -----------------------------------------------------------------------------

typedef struct sinOsc
{
	Float32	sin;
	Float32	cos;
} sinOsc;

typedef struct triMod
{
	Float32	left;
	Float32	center;
	Float32	right;
} triMod;

inline static Float32 so_run(sinOsc* s, Float32 x)
{
	x *= M_2PI;

	s->sin	+= x * s->cos;
	s->cos  -= x * s->sin;

	return s->sin;
}

inline static Float32 so_normalized(sinOsc *s, Float32 x)
{
	return so_run(s, x) * 0.5 + 0.5;
}

inline static void so_triMod(sinOsc* s1, Float32 x1, sinOsc* s2, Float32 x2, triMod* m)
{
	so_run(s1, x1);
	so_run(s2, x2);

	Float32 sin	 = s1->sin + s2->sin;
	Float32 cos	 = s1->cos + s2->cos;
	
	m->center	 = 0.5 + sin * 0.25;
	
	sin			*= -0.5;
	cos			*= 0.8666;
	
	m->left		 = 0.5 + (sin + cos) * 0.25;
	m->right	 = 0.5 + (sin - cos) * 0.25;
}

inline static void so_clear(sinOsc *s)
{
	s->sin	= 0.0;
	s->cos	= 1.0;
}

inline static void so_set(sinOsc *s, Float32 p)
{
	p *= M_2PI;
	
	s->sin  = sinf(p);
	s->cos  = cosf(p);
}

// -----------------------------------------------------------------------------

inline static Float32 fastSin(Float32 x)
{
	if (x > M_PI_2)
		x = M_PI_2 - (x - M_PI_2);
	else if (x < M_NPI_2)
		x = M_NPI_2 + (M_NPI_2 - x);

    Float32 sqr = x * x;
    Float32 sin = -2.39e-08;
	
    sin *= sqr;
    sin += 2.7526e-06;
    sin *= sqr;
    sin -= 1.98409e-04;
    sin *= sqr;
    sin += 8.3333315e-03;
    sin *= sqr;
    sin -= 1.666666664e-01;
    sin *= sqr;
    sin += 1.0f;
    sin *= x;
	
    return sin;
}

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
