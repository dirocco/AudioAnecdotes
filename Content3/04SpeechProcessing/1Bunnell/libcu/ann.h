#ifndef __ANN_HEADER__
#define __ANN_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct network {
    int nlayers; 	/* Number of layers in network */
    int *sz;		/* Number of nodes in each layer */
    float *w;		/* connection weights for each node */
    float **act;	/* activation levels at each node */
} ANN;

DLL_PREFIX ANN *init_net(char *weightfile);
void DLL_PREFIX free_net(ANN *net);
void DLL_PREFIX run_net (ANN *net, float *inact, float *outact);
#ifdef __cplusplus
}
#endif
#endif
