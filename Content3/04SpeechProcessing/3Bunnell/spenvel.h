#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagENVPT {
	int np;
	float *freq;
	float *ampl;
}ENVPT;

double spenvel(double fmax, int npt, float *s, ENVPT *e);
double combsrch(double flo, double fhi, double finc,  double fmax, int npt, float *s);
double cepitch(double flo, double fhi, double fmax, int npt, float *s);
double combvda(double fspect, int npt, float *s, double *estF0, double fmin, double fmax);

#ifdef __cplusplus
}
#endif
