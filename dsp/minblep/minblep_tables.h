/*
 *  minblep_tables.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef MINBLEP_TABLES_H
#define MINBLEP_TABLES_H

// -----------------------------------------------------------------------------

#define MINBLEP_PHASES          64
#define MINBLEP_PHASE_MASK      63
#define STEP_DD_PULSE_LENGTH    72
#define SLOPE_DD_PULSE_LENGTH   71
#define LONGEST_DD_PULSE_LENGTH	STEP_DD_PULSE_LENGTH
#define DD_SAMPLE_DELAY			4
#define FILL_LENGTH				256

typedef struct Float32Delta
{
	Float32	value;
	Float32	delta;
}
Float32Delta;

extern	Float32Delta	step_dd_table[];
extern	Float32			slope_dd_table[];

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
