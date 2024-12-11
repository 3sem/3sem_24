#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <pthread.h>

#include "monte.h"


typedef struct Chunk
{
	double (*func)(const double x);

	double x_min;
	double x_range;
	double y_max;
	
	uint32_t n_points;
	uint32_t n_internal_points;
} Chunk;


static const uint32_t N_POINTS = 100000000;


void init_thread_seed( unsigned short xsubi[3])
{
	xsubi[0] = rand();
	xsubi[1] = rand();
	xsubi[2] = rand();
}

static void calc_chunk_internal_points( Chunk *const chunk)
{ 
	assert( chunk != NULL);


	unsigned short xsubi[3] = { 0 }; // external storage for random function
	init_thread_seed( xsubi);

	double (*const func)( const double x) = chunk->func;
	const double x_min                    = chunk->x_min;
	const double x_range                  = chunk->x_range;
	const double y_max                    = chunk->y_max;	
	const uint32_t n_points               = chunk->n_points;

	uint32_t n_internal_points = 0;	

	for ( uint32_t i = 0; i < n_points; i++ )
	{
		double x_rand = erand48( xsubi) * x_range + x_min;
		double y_rand = erand48( xsubi) * y_max;

		double y_func = func( x_rand);

		if ( y_rand < y_func )
		{
			n_internal_points += 1;
		}
	}

	chunk->n_internal_points = n_internal_points;
}

static void *wrapper_calc_chunk_internal_points( void *const args)
{
	assert( args != NULL);


	Chunk *const chunk = (Chunk *)args;

	calc_chunk_internal_points( chunk);
	
	
	pthread_exit( 0);
}

static void cancel_threads( const pthread_t tids[], const uint32_t n_threads)
{
	for ( uint32_t i = 0; i < n_threads; i++ )
	{
		pthread_cancel( tids[i]);
	}
}

static void join_threads( const pthread_t tids[], const uint32_t n_threads)
{
	for ( uint32_t i = 0; i < n_threads; i++ )
	{
		pthread_join( tids[i], NULL);
	}
}

static uint32_t calc_internal_points( const Chunk chunks[], const uint32_t n_threads)
{
	uint32_t n_internal_points = 0;

	for ( uint32_t i = 0; i < n_threads; i++ )
	{
		n_internal_points += chunks[i].n_internal_points;
	}

	return n_internal_points;
}


double calc_integral( double (*const func)( const double x), 
			const double x_min, const double x_max, 
			const uint32_t n_threads)
{
	assert( func != NULL);

	
	srand( time( NULL));


	pthread_t tids[n_threads]; // variable-length array
	Chunk   chunks[n_threads]; // variable-length array

	memset( tids,   0, n_threads * sizeof( pthread_t));
	memset( chunks, 0, n_threads * sizeof( Chunk));
	

	const double   x_range        = (x_max - x_min);
	const double   x_chunk_range  = x_range / n_threads;
	const double   y_max          = func( x_max);
	const uint32_t n_chunk_points = N_POINTS / n_threads;


	int create_res = 0;

	for ( uint32_t i = 0; i < n_threads; i++ )
	{
		const double x_chunk_min = x_min + x_chunk_range * i;

		pthread_t *const tid   = tids   + i;
		Chunk     *const chunk = chunks + i;

		chunk->func              = func;
		chunk->x_min             = x_chunk_min;
		chunk->x_range           = x_chunk_range;	
		chunk->y_max             = y_max;
		chunk->n_points          = n_chunk_points;
		chunk->n_internal_points = 0;
		
		// read() block
		// read() non-block
		create_res = pthread_create( tid, NULL, wrapper_calc_chunk_internal_points, chunk);

		if ( create_res != 0 )
		{
			cancel_threads( tids, i);			
			break;
		}
	}

	if ( create_res != 0)
	{
		return -1;
	}


	join_threads( tids, n_threads);

	const double square   = x_range * y_max;
    const double coverage = (double)calc_internal_points( chunks, n_threads) / N_POINTS;


	return square * coverage;
}
