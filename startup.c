/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This routine does all the required initialization and startup,
 * including memory allocation and prefilling of vectors.  It is
 * COMPLETELY outside the timing loops.
 *========================================================================
 */

#include "dieharder.h"

void startup()
{

 int i,imax,j,k;

 /*
  * The very first thing we do is see if any simple help options
  * were requested, as we exit immediately if they were and can
  * save all sorts of work.
  */
 if(list == YES) {

printf("\n\
                     DieHarder Test Suite\n\
========================================================================\n\
The following tests are available and will be run when diehard -a is\n\
invoked.  Special options or suggested parameters are indicated if\n\
they are needed to get a satisfactory result (such as completion in a\n\
reasonable amount of time).\n\
\n\
            Diehard Tests\n\
   -d 1 Diehard Runs test\n\
   -d 2 Diehard Birthdays test (-t 100, or less than 200) \n\
   -d 3 Diehard Minimum Distance (2D Spheres) test\n\
   -d 4 Diehard 3D Spheres (minimum distance) test\n\
\n\
             RGB Tests\n\
   -r 1 Bit Persist test\n\
   -r 2 Bit Ntuple Distribution test suite (-n ntuple for 1-8)\n\
\n\
      Statistical Test Suite (STS)\n\
   -s 1 STS Monobit test\n\
   -s 2 STS Runs test\n\
\n\
            User Tests\n\
No user-developed test are installed at this time.\n\
\n");

   exit(0);
 }

 /*
  * Count and optionally list the available, built in gsl generators
  */
 types = gsl_rng_types_setup ();
 i = 0;
 while(types[i] != NULL){
   i++;
 }
 num_gsl_rngs = i;

 /*
  * Now add my own types and count THEM.
  */
 add_my_types();
 while(types[i] != NULL){
   i++;
 }

 num_rngs = i;
 num_my_rngs = num_rngs - num_gsl_rngs;

 if(generator > num_rngs-1){
   fprintf(stderr,"Error:  rng %d (> %d) does not exist!\n",generator,num_rngs-1);
   list_rngs();
   exit(0);
 }

 /*
  * Initialize the selected gsl rng.  random_seed() seeds from
  * /dev/random.  Note that any locally defined rng's were "added"
  * to the gsl set above and can now be called with the gsl
  * wrapper!  So we either initialize the selected generator or list
  * the generators and exit.  We had to wait until now for the latter
  * or we'd miss our own additions!
  */
 if(generator >= 0){
   rng = gsl_rng_alloc (types[generator]);
   random_max = gsl_rng_max(rng);
   seed = random_seed();
   gsl_rng_set(rng,seed);
 } else {
   list_rngs();
   exit(0);
 }


 

 /*
  * Simultaneously count the number of significant bits in the rng
  * AND create a mask (which we need in e.g. rgb_persist and possibly
  * elsewhere).
  */
 rmax = random_max;
 rmax_bits = 0;
 rmax_mask = 0;
 while(rmax){
   rmax >>= 1;
   rmax_mask = rmax_mask << 1;
   rmax_mask++;
   rmax_bits++;
 }


 /*
  * Allocate the global vector that will hold random integers
  * loaded from gsl generators (usually) or from a file (sometimes).
  * One day (soon) we'll need to figure out file I/O.  Not exactly
  * trivial, as many of the tests determine how many rands they
  * need depending strongly on the test parameters -- to be able
  * to run arbitrary tests, the file will need to be quite large.
  * We may need to write a routine that fills rand_int on demand
  * from either the file or a generator.
  *
  * This size is possibly overkill, but otherwise we'd have to
  * figure out how big it is per test.  This is big enough
  * for all of them, I'm pretty sure.
  */
 rand_int = (int *) malloc((size_t) (tsamples*sizeof(unsigned int)));

 /*
  * This is the global vector of p-values generated by each test
  * run many times.  It has to be oversized because resizing it
  * to just right is more of a hassle then just spending the memory
  * in an era where systems with less than a GB of active memory
  * will be increasingly rare.  These p-values are ONLY used in
  * the end-stage e.g. KS tests that globally validate the distribution
  * of p-values returned by the test.  Set the kspi index to point to
  * the first element of the vector.
  */
 ks_pvalue = (double *)malloc((size_t) KS_SAMPLES_PER_TEST_MAX*psamples*sizeof(double));
 kspi = 0;

}
