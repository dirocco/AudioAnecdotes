#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <math.h>

#include "libcu.h"

#define EMAX 70.	/* cut-off for exp-function */
#define EMIN -70.	/* cut-off for exp-function */

DLL_PREFIX ANN *init_net(char *weightfile)
{
  float  ferr;
  int    il, szmax, itemp, nconn;
  int	nlayers, nl1, nhl;
  FILE   *wp;
  ANN    *net;

  net = (ANN *) malloc(sizeof(ANN));

  /* Initialize weights from file*/
  if ((wp = fopen(weightfile, "r")) == (FILE *) NULL) {
      free(net);
      return (ANN *) NULL;
  }

  fread (&itemp, sizeof (int), 1, wp);
  fread (&nlayers, sizeof (int), 1, wp);
  net->nlayers = nlayers;
  net->sz = (int *) malloc(nlayers * sizeof(int));
  nl1 = nlayers - 1;
  nhl = nl1 - 1;

  fread (net->sz, sizeof (int), nlayers, wp);
  szmax = 0;
  nconn = 0;
  for (il = 0; il < nl1; il++) {
      if ((il > 0) && (net->sz[il] > szmax))
	szmax = net->sz[il];
      nconn += net->sz[il] * net->sz[il+1];
  }

  /*
   * act = matrix of neural activities
   * w = weight matrix
   */

  if ((net->act = (float **) alloc2d (nhl, szmax, sizeof(float))) == 
      (float **) NULL) {
      free(net->sz);
      free(net);
      return (ANN *) NULL;
  }

  if ((net->w = (float *) malloc (nconn * sizeof(float))) == (float *) NULL) {
      free2d((void **) (net->act));
      free(net->sz);
      free(net);
      return (ANN *) NULL;
  }

  fread (net->w, sizeof(float), nconn, wp);
  fread (&ferr, sizeof(float), 1, wp);
  fclose (wp);

  return(net);
}

void DLL_PREFIX free_net(ANN *net)
{
  free (net->sz);
  free (net->w);
  free2d ((void **) (net->act));
  free (net);
}


float DLL_PREFIX f(float s)
{
    float  x;

    x = s;
    if (x > EMAX)
        x = (float) EMAX;
    else if (x < EMIN)
        x = (float) EMIN;
    return (float) (1.0 / (1.0 + exp ((double) -x)));
}

/* pass input thru the network and return output activation levels */

void DLL_PREFIX run_net (ANN *net, float *inact, float *outact)
{
    int    i, j, k, nl1, nhl, nconn = 0;
    float  s;

    nl1 = net->nlayers - 1;
    nhl = nl1 - 1;

/*
 * Compute activation for first hidden layer from input
 */
    for (i=0; i<net->sz[1]; i++) {
        s = 0.0f;
        for (j=0; j<(net->sz[0])-1; j++)
            s += net->w[nconn++] * inact[j];
	s += net->w[nconn++];	/* implicit Unit activation on extra input */
        net->act[0][i] = f(s);

    }
/*
 * Compute activation for any additional hidden layers
 */
    for (k=1; k<nhl; k++)
        for (i=0; i<net->sz[k+1]; i++) {
            s = 0.0f;
            for (j=0; j<net->sz[k]; j++)
                s += net->w[nconn++] * net->act[k-1][j];
            net->act[k][i] = f(s);
        }
/*
 * Compute activation at output from last hidden layer
 */
    for (k=0; k<net->sz[nl1]; k++) {
	s = 0.0f;
	for (j=0; j<net->sz[nhl]; j++)
	  s += net->w[nconn++] * net->act[nhl-1][j];
	*outact++ = f(s);
    }
}

 
/*
 *  Test program for net_subs
 */
#ifdef COMPILE_TEST
main ()
{
    ANN *net;
    FILE *cf;
    float inputs[13], outputs[2];
    int actual, estimate, ncases;
    int i, j;

    if ((net = init_net("testnet.wts")) == (ANN *) NULL) {
	printf("error initializing network from testnet.wts\n");
	exit(1);
    }

    printf("Network returned is %d layers of size:", net->nlayers);
    for (i=0; i<net->nlayers; i++)
      printf(" %d", net->sz[i]);
    printf("\n");

    if ((cf = fopen("testnet.cls", "r")) == (FILE *) NULL) {
	printf("error opening class file testnet.cls\n");
	exit(1);
    }
	
    fscanf(cf, "%d", &ncases);
    printf("Number of cases = %d\n", ncases);

    for (i=0; i<ncases; i++) {
	fscanf(cf, "%i", &actual);
	for (j=0; j<13; j++)
	  fscanf(cf, "%f", &inputs[j]);

	run_net(net, inputs, outputs);

	if (outputs[0] >= outputs[1])
	  estimate = 1;
	else
	  estimate = 2;

	printf("Case %d: activation out = %6f %6f; expected %d, got %d\n",
	       i, outputs[0], outputs[1], actual, estimate);
    }

    free_net(net);
    exit(0);
}
#endif
