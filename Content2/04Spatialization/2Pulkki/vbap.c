/* vbap.c  
(c) Ville Pulkki   24.10.2000 Helsinki University of Technology*/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pablio.h>
#include "vbap.h"



/* This is a program which pans a signal
using Vector Base Amplitude Panning (VBAP), 
see Pulkki: "Virtual Source Positioning using VBAP" 
in  Journal of the Audio Engineering Society June 1997.
The program can be easily modified to handle multiple audio input
channels separately. This program is just to demonstrate how 
VBAP can be implemented. 

*/




double *angle_to_cart(int azi, int ele)
{
  double *res;
  double atorad = (2 * 3.1415927 / 360) ;
  res = (double *) malloc(3*sizeof(double));
  res[0] = (float) (cos((double) (azi * atorad)) * cos((double)  (ele * atorad)));
  res[1] = (float) (sin((double) (azi * atorad)) * cos((double) (ele * atorad)));
  res[2] = (float) (sin((double) (ele * atorad)));
  return res;
}


void vbap(double gains[MAX_CHANNELS], LS_DATA *ls_data, int azi, int ele) 
{
  /* calculates gain factors using loudspeaker setup and given direction */
  double *cartdir;
  double power;
  int i,j,k;
  double small_g;
  double big_sm_g, gtmp[3];
  int winner_triplet;
  int ls[3];
  double g[3];

  cartdir=angle_to_cart(azi,ele);  
  big_sm_g = -100000.0;
  for(i=0;i<ls_data->triplet_amount;i++){
    small_g = 10000000.0;
    for(j=0;j<ls_data->dimension;j++){
      gtmp[j]=0.0;
      for(k=0;k<ls_data->dimension;k++)
	gtmp[j]+=cartdir[k]*ls_data->lsm[i][k+j*ls_data->dimension]; 
      if(gtmp[j] < small_g){
	small_g = gtmp[j];
      }
    }
    if(small_g > big_sm_g){
      big_sm_g = small_g;
      winner_triplet=i;
      g[0]=gtmp[0]; g[1]=gtmp[1]; 
      ls[0]=ls_data->lstripl[i][0]; ls[1]=ls_data->lstripl[i][1]; 
      if(ls_data->dimension==3){
	g[2]=gtmp[2];
	ls[2]=ls_data->lstripl[i][2];
      } else {
	g[2]=0.0;
	ls[2]=1;
      }
    }
  }

  power=sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);
  g[0] /= power; 
  g[1] /= power;
  g[2] /= power;

  for(i=0;i<MAX_CHANNELS;i++){
    gains[i]=0.0;
  }
  gains[ls[0]-1]=g[0];
  gains[ls[1]-1]=g[1];
  if(ls_data->dimension==3){
    gains[ls[2]-1]=g[2];
  }
  free(cartdir);
}



main() 
{
  LS_DATA ls_data;
  FILE *fp;
  double gains[MAX_CHANNELS];
  int azimuth=-10;
  int elevation=14;
  double *gainptr,  gain;
  short *inptr,*outptr;
  short out[BUFFER_LENGTH][MAX_CHANNELS];
  ALconfig c;
  AFfilehandle fh;
  ALport p;

  short *in;
  long frames;

  int i,j,k;
  int numchannels = 8; /* change this according your output device*/
  int ls_set_dim = 3;
  int ls_num = 8;
  int ls_dirs[MAX_FIELD_AM]={-30,0,  30,0, -45,45,  45,45,  -90,0, 
			     90,0,  180,0,  180,45};
  /* change these according to your loudspeaker positioning*/
  

  /* defining loudspeaker data */
  define_loudspeakers(&ls_data, ls_set_dim, ls_num, ls_dirs);
     /* ls_data is a struct containing matrices etc
     ls_set_dim  is 2 if loudspeakers are on a (horizontal) plane
     ls_set_dim  is 3 if also elevated or descended loudpeakers exist
     ls_num is the number of loudspeakers
     ls_dirs is an array containing the angular directions of loudsp*/

  
  /* gain factors for virtual source in direction
     (int azimuth, int elevation) */
  vbap(gains, &ls_data, azimuth, elevation);  
     /* panning monophonic stream  float *in 
     to multiple outputs           float *out[]
     with gain factors             float *gains  */
  

  /* input audio*/
  if((fh=afOpenFile("myaiff.aiff","r",0))==NULL){
    fprintf(stderr, "Could not open file myaiff.aiff\n");
    exit(-1);
  }
  frames=AFgetframecnt(fh,AF_DEFAULT_TRACK);
  if(afGetChannels(fh, AF_DEFAULT_TRACK) != 1){
    fprintf(stderr, "Supports only mono aiff-files\n");
    exit(-1);
  }
  in= malloc(frames*sizeof(short)*2);
  afReadFrames(fh,AF_DEFAULT_TRACK,in,frames);


  /*opening the audio port*/
  c = alNewConfig();
  if (!c) {
    printf("Couldn't create ALconfig:%s\n", alGetErrorString(oserror()));
    exit(-1);
  }
  alSetChannels(c,numchannels);
  ALsetqueuesize(c,BUFFER_LENGTH);
  p = alOpenPort("alVBAP example","w",c);
  if (!p) {
    printf("port open failed:%s\n", alGetErrorString(oserror()));
    exit(-1);
  }
  


  fprintf(stderr,"\nPanning audio");
  for(j=0;j<(frames/BUFFER_LENGTH);j++){
    inptr = &(in[j*BUFFER_LENGTH]); /* audio to be panned  */
    outptr= out[0];         

    for (i=0; i<BUFFER_LENGTH; i++){    /* panning */ 
      gainptr=gains;
      for (k=0; k<numchannels; k++){
	*outptr++ = (short) ((double) *inptr * *gainptr++); 
      }
      inptr++;
    }
    alWriteFrames(p, out, BUFFER_LENGTH); /* write frames */
    fprintf(stderr,".");
  }

  /*write rest samples out*/

  inptr = &(in[j*BUFFER_LENGTH]); /* partial buffer  */
  outptr= out[0];
  for (i=0; i<(frames-BUFFER_LENGTH*j); i++){    /* panning */ 
    gainptr=gains;
    for (k=0; k<numchannels; k++){
      *outptr++ = (short) ((double) *inptr * *gainptr++); 
    }
    inptr++;
  }
  for (;i<BUFFER_LENGTH; i++){    /* zeros  */ 
    for (k=0; k<numchannels; k++){
      *outptr++ = 0; 
    }
  }

  alWriteFrames(p, out, BUFFER_LENGTH); /* write frames */
  fprintf(stderr,".");

  printf("\n\nDone!\n\n");
}





void define_loudspeakers(LS_DATA *ls_data, int ls_set_dim, int ls_num, int ls_dirs[MAX_FIELD_AM])
     /* Inits the loudspeaker data. Calls choose_ls_tuplets or _triplets
	according to current dimension. The inversion matrices are 
     stored in transposed form to ease calculation at run time.*/
{
  char *s;
  int dim;
  float tmp;
  struct ls_triplet_chain *ls_triplets = NULL;
  ls lss[MAX_LS_AMOUNT];
  ang_vec a_vector;
  cart_vec c_vector;
  int i=0,j;
  float azi, ele;

  dim=ls_set_dim;
  if (!((dim==2) || (dim == 3))){
    fprintf(stderr,"Error in loudspeaker dimension.\n");
    exit (-1);
  }

  j=0;
  for(i=0;i<ls_num;i++) {
    azi= (float) ls_dirs[j++];
    if(dim == 3) 
      ele = (float) ls_dirs[j++];
    else
      ele = 0.0;
    a_vector.azi = azi;
    a_vector.ele = ele;
    angle_to_cart2(&a_vector,&c_vector);
    lss[i].coords.x = c_vector.x;
    lss[i].coords.y = c_vector.y;
    lss[i].coords.z = c_vector.z;
    lss[i].angles.azi = a_vector.azi;
    lss[i].angles.ele = a_vector.ele;
    lss[i].angles.length = 1.0;
  }


  if(ls_num < dim) {
    fprintf(stderr,"Too few loudspeakers %d\n",ls_num);
    exit (-1);
  }

  if(dim == 3){
    choose_ls_triplets(lss, &ls_triplets,ls_num);
    calculate_3x3_matrixes(ls_triplets,lss,ls_num,ls_data);
  } else if (dim ==2) {
    choose_ls_tuplets(lss, &ls_triplets,ls_num,ls_data); 
  }
}



 
void angle_to_cart2(ang_vec *from, cart_vec *to)
     /* from angular to cartesian coordinates*/
{
  float ang2rad = 2 * 3.141592 / 360;
  to->x= (float) (cos((double)(from->azi * ang2rad)) 
		  * cos((double) (from->ele * ang2rad)));
  to->y= (float) (sin((double)(from->azi * ang2rad)) 
		  * cos((double) (from->ele * ang2rad)));
  to->z= (float) (sin((double) (from->ele * ang2rad)));
}  


void choose_ls_triplets(ls lss[MAX_LS_AMOUNT],   
			struct ls_triplet_chain **ls_triplets, int ls_amount) 
     /* Selects the loudspeaker triplets, and
      calculates the inversion matrices for each selected triplet.
     A line (connection) is drawn between each loudspeaker. The lines
     denote the sides of the triangles. The triangles should not be 
     intersecting. All crossing connections are searched and the 
     longer connection is erased. This yields non-intesecting triangles,
     which can be used in panning.*/
{
  int i,j,k,l,m,li, table_size;
  int *i_ptr;
  cart_vec vb1,vb2,tmp_vec;
  int connections[MAX_LS_AMOUNT][MAX_LS_AMOUNT];
  float angles[MAX_LS_AMOUNT];
  int sorted_angles[MAX_LS_AMOUNT];
  float distance_table[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  int distance_table_i[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  int distance_table_j[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  float distance;
  struct ls_triplet_chain *trip_ptr, *prev, *tmp_ptr;

  if (ls_amount == 0) {
    fprintf(stderr,"Number of loudspeakers is zero\nExiting\n");
    exit(-1);
  }
  for(i=0;i<ls_amount;i++)
    for(j=i+1;j<ls_amount;j++)
      for(k=j+1;k<ls_amount;k++){
	if(vol_p_side_lgth(i,j, k, lss) > MIN_VOL_P_SIDE_LGTH){
	  connections[i][j]=1;
	  connections[j][i]=1;
	  connections[i][k]=1;
	  connections[k][i]=1;
	  connections[j][k]=1;
	  connections[k][j]=1;
	  add_ldsp_triplet(i,j,k,ls_triplets, lss);
	}
      }
  /*calculate distancies between all lss and sorting them*/
  table_size =(((ls_amount - 1) * (ls_amount)) / 2); 
  for(i=0;i<table_size; i++)
    distance_table[i] = 100000.0;
  for(i=0;i<ls_amount;i++){ 
    for(j=(i+1);j<ls_amount; j++){ 
      if(connections[i][j] == 1) {
	distance = fabs(vec_angle(lss[i].coords,lss[j].coords));
	k=0;
	while(distance_table[k] < distance)
	  k++;
	for(l=(table_size - 1);l > k ;l--){
	  distance_table[l] = distance_table[l-1];
	  distance_table_i[l] = distance_table_i[l-1];
	  distance_table_j[l] = distance_table_j[l-1];
	}
	distance_table[k] = distance;
	distance_table_i[k] = i;
	distance_table_j[k] = j;
      } else
	table_size--;
    }
  }

  /* disconnecting connections which are crossing shorter ones,
     starting from shortest one and removing all that cross it,
     and proceeding to next shortest */
  for(i=0; i<(table_size); i++){
    int fst_ls = distance_table_i[i];
    int sec_ls = distance_table_j[i];
    if(connections[fst_ls][sec_ls] == 1)
      for(j=0; j<ls_amount ; j++)
	for(k=j+1; k<ls_amount; k++)
	  if( (j!=fst_ls) && (k != sec_ls) && (k!=fst_ls) && (j != sec_ls)){
	    if(lines_intersect(fst_ls, sec_ls, j,k,lss) == 1){
	      connections[j][k] = 0;
	      connections[k][j] = 0;
	    }
	  }
  }

  /* remove triangles which had crossing sides
     with smaller triangles or include loudspeakers*/
  trip_ptr = *ls_triplets;
  prev = NULL;
  while (trip_ptr != NULL){
    i = trip_ptr->ls_nos[0];
    j = trip_ptr->ls_nos[1];
    k = trip_ptr->ls_nos[2];
    if(connections[i][j] == 0 || 
       connections[i][k] == 0 || 
       connections[j][k] == 0 ||
       any_ls_inside_triplet(i,j,k,lss,ls_amount) == 1 ){
      if(prev != NULL) {
	prev->next = trip_ptr->next;
	tmp_ptr = trip_ptr;
	trip_ptr = trip_ptr->next;
	free(tmp_ptr);
      } else {
	*ls_triplets = trip_ptr->next;
	tmp_ptr = trip_ptr;
	trip_ptr = trip_ptr->next;
	free(tmp_ptr);
      }
    } else {
      prev = trip_ptr;
      trip_ptr = trip_ptr->next;

    }
  }
}


int any_ls_inside_triplet(int a, int b, int c,ls lss[MAX_LS_AMOUNT],int ls_amount)
     /* returns 1 if there is loudspeaker(s) inside given ls triplet */
{
  float invdet;
  cart_vec *lp1, *lp2, *lp3;
  float invmx[9];
  int i,j,k;
  float tmp;
  int any_ls_inside, this_inside;

  lp1 =  &(lss[a].coords);
  lp2 =  &(lss[b].coords);
  lp3 =  &(lss[c].coords);

  /* matrix inversion */
  invdet = 1.0 / (  lp1->x * ((lp2->y * lp3->z) - (lp2->z * lp3->y))
		    - lp1->y * ((lp2->x * lp3->z) - (lp2->z * lp3->x))
		    + lp1->z * ((lp2->x * lp3->y) - (lp2->y * lp3->x)));
  
  invmx[0] = ((lp2->y * lp3->z) - (lp2->z * lp3->y)) * invdet;
  invmx[3] = ((lp1->y * lp3->z) - (lp1->z * lp3->y)) * -invdet;
  invmx[6] = ((lp1->y * lp2->z) - (lp1->z * lp2->y)) * invdet;
  invmx[1] = ((lp2->x * lp3->z) - (lp2->z * lp3->x)) * -invdet;
  invmx[4] = ((lp1->x * lp3->z) - (lp1->z * lp3->x)) * invdet;
  invmx[7] = ((lp1->x * lp2->z) - (lp1->z * lp2->x)) * -invdet;
  invmx[2] = ((lp2->x * lp3->y) - (lp2->y * lp3->x)) * invdet;
  invmx[5] = ((lp1->x * lp3->y) - (lp1->y * lp3->x)) * -invdet;
  invmx[8] = ((lp1->x * lp2->y) - (lp1->y * lp2->x)) * invdet;

  any_ls_inside = 0;
  for(i=0; i< ls_amount; i++) {
    if (i != a && i!=b && i != c){
      this_inside = 1;
      for(j=0; j< 3; j++){
	tmp = lss[i].coords.x * invmx[0 + j*3];
	tmp += lss[i].coords.y * invmx[1 + j*3];
	tmp += lss[i].coords.z * invmx[2 + j*3];
	if(tmp < -0.001)
	  this_inside = 0;
      }
      if(this_inside == 1)
	any_ls_inside=1;
    }
  }
  return any_ls_inside;
}


void add_ldsp_triplet(int i, int j, int k, 
		       struct ls_triplet_chain **ls_triplets,
		       ls lss[MAX_LS_AMOUNT])
     /* adds i,j,k triplet to triplet chain*/
{
  struct ls_triplet_chain *trip_ptr, *prev;
  trip_ptr = *ls_triplets;
  prev = NULL;

  while (trip_ptr != NULL){
    prev = trip_ptr;
    trip_ptr = trip_ptr->next;
  }
  trip_ptr = (struct ls_triplet_chain*) 
    malloc (sizeof (struct ls_triplet_chain));
  if(prev == NULL)
    *ls_triplets = trip_ptr;
  else 
    prev->next = trip_ptr;
  trip_ptr->next = NULL;
  trip_ptr->ls_nos[0] = i;
  trip_ptr->ls_nos[1] = j;
  trip_ptr->ls_nos[2] = k;
}




float vec_angle(cart_vec v1, cart_vec v2)
{
  float inner= ((v1.x*v2.x + v1.y*v2.y + v1.z*v2.z)/
	      (vec_length(v1) * vec_length(v2)));
  if(inner > 1.0)
    inner= 1.0;
  if (inner < -1.0)
    inner = -1.0;
  return fabsf((float) acos((double) inner));
}

float vec_length(cart_vec v1)
{
  return (fsqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z));
}

float vec_prod(cart_vec v1, cart_vec v2)
{
  return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}


float vol_p_side_lgth(int i, int j,int k, ls  lss[MAX_LS_AMOUNT] ){
  /* calculate volume of the parallelepiped defined by the loudspeaker
     direction vectors and divide it with total length of the triangle sides. 
     This is used when removing too narrow triangles. */

  float volper, lgth;
  cart_vec xprod;
  cross_prod(lss[i].coords, lss[j].coords, &xprod);
  volper = fabsf(vec_prod(xprod, lss[k].coords));
  lgth = (fabsf(vec_angle(lss[i].coords,lss[j].coords)) 
	  + fabsf(vec_angle(lss[i].coords,lss[k].coords)) 
	  + fabsf(vec_angle(lss[j].coords,lss[k].coords)));
  if(lgth>0.00001)
    return volper / lgth;
  else
    return 0.0;
}

void cross_prod(cart_vec v1,cart_vec v2, 
		cart_vec *res) 
{
  float length;
  res->x = (v1.y * v2.z ) - (v1.z * v2.y);
  res->y = (v1.z * v2.x ) - (v1.x * v2.z);
  res->z = (v1.x * v2.y ) - (v1.y * v2.x);

  length= vec_length(*res);
  res->x /= length;
  res->y /= length;
  res->z /= length;
}


int lines_intersect(int i,int j,int k,int l,ls  lss[MAX_LS_AMOUNT])
     /* checks if two lines intersect on 3D sphere 
      see theory in paper Pulkki, V. Lokki, T. "Creating Auditory Displays
      with Multiple Loudspeakers Using VBAP: A Case Study with
      DIVA Project" in International Conference on 
      Auditory Displays -98. E-mail Ville.Pulkki@hut.fi
     if you want to have that paper.*/
{
  cart_vec v1;
  cart_vec v2;
  cart_vec v3, neg_v3;
  float angle;
  float dist_ij,dist_kl,dist_iv3,dist_jv3,dist_inv3,dist_jnv3;
  float dist_kv3,dist_lv3,dist_knv3,dist_lnv3;

  cross_prod(lss[i].coords,lss[j].coords,&v1);
  cross_prod(lss[k].coords,lss[l].coords,&v2);
  cross_prod(v1,v2,&v3);

  neg_v3.x= 0.0 - v3.x;
  neg_v3.y= 0.0 - v3.y;
  neg_v3.z= 0.0 - v3.z;

  dist_ij = (vec_angle(lss[i].coords,lss[j].coords));
  dist_kl = (vec_angle(lss[k].coords,lss[l].coords));
  dist_iv3 = (vec_angle(lss[i].coords,v3));
  dist_jv3 = (vec_angle(v3,lss[j].coords));
  dist_inv3 = (vec_angle(lss[i].coords,neg_v3));
  dist_jnv3 = (vec_angle(neg_v3,lss[j].coords));
  dist_kv3 = (vec_angle(lss[k].coords,v3));
  dist_lv3 = (vec_angle(v3,lss[l].coords));
  dist_knv3 = (vec_angle(lss[k].coords,neg_v3));
  dist_lnv3 = (vec_angle(neg_v3,lss[l].coords));

  /* if one of loudspeakers is close to crossing point, don't do anything*/


  if(fabsf(dist_iv3) <= 0.01 || fabsf(dist_jv3) <= 0.01 || 
  fabsf(dist_kv3) <= 0.01 || fabsf(dist_lv3) <= 0.01 ||
     fabsf(dist_inv3) <= 0.01 || fabsf(dist_jnv3) <= 0.01 || 
     fabsf(dist_knv3) <= 0.01 || fabsf(dist_lnv3) <= 0.01 )
    return(0);


 
  if (((fabsf(dist_ij - (dist_iv3 + dist_jv3)) <= 0.01 ) &&
       (fabsf(dist_kl - (dist_kv3 + dist_lv3))  <= 0.01)) ||
      ((fabsf(dist_ij - (dist_inv3 + dist_jnv3)) <= 0.01)  &&
       (fabsf(dist_kl - (dist_knv3 + dist_lnv3)) <= 0.01 ))) {
    return (1);
  } else {
    return (0);
  }
}



void  calculate_3x3_matrixes(struct ls_triplet_chain *ls_triplets, 
			 ls lss[MAX_LS_AMOUNT], int ls_amount, LS_DATA *ls_data)
     /* Calculates the inverse matrices for 3D */
{  
  float invdet;
  cart_vec *lp1, *lp2, *lp3;
  float *invmx;
  float *ptr;
  struct ls_triplet_chain *tr_ptr = ls_triplets;
  int triplet_amount = 0, ftable_size,i,j,k;
  float *ls_table;

  if (tr_ptr == NULL){
    fprintf(stderr,"Not valid 3-D configuration\n");
    exit(-1);
  }

  /* counting triplet amount */
  while(tr_ptr != NULL){
    triplet_amount++;
    tr_ptr = tr_ptr->next;
  }

  /* calculations and data storage to a global array */
  ls_table = (float *) malloc( (triplet_amount*12 + 3) * sizeof (float));
  ls_table[0] = 3.0; /*dimension*/
  ls_table[1] = (float) ls_amount;
  ls_table[2] = (float) triplet_amount;
  tr_ptr = ls_triplets;
  ptr = (float *) &(ls_table[3]);
  while(tr_ptr != NULL){
    lp1 =  &(lss[tr_ptr->ls_nos[0]].coords);
    lp2 =  &(lss[tr_ptr->ls_nos[1]].coords);
    lp3 =  &(lss[tr_ptr->ls_nos[2]].coords);

    /* matrix inversion */
    invmx = tr_ptr->inv_mx;
    invdet = 1.0 / (  lp1->x * ((lp2->y * lp3->z) - (lp2->z * lp3->y))
		    - lp1->y * ((lp2->x * lp3->z) - (lp2->z * lp3->x))
		    + lp1->z * ((lp2->x * lp3->y) - (lp2->y * lp3->x)));

    invmx[0] = ((lp2->y * lp3->z) - (lp2->z * lp3->y)) * invdet;
    invmx[3] = ((lp1->y * lp3->z) - (lp1->z * lp3->y)) * -invdet;
    invmx[6] = ((lp1->y * lp2->z) - (lp1->z * lp2->y)) * invdet;
    invmx[1] = ((lp2->x * lp3->z) - (lp2->z * lp3->x)) * -invdet;
    invmx[4] = ((lp1->x * lp3->z) - (lp1->z * lp3->x)) * invdet;
    invmx[7] = ((lp1->x * lp2->z) - (lp1->z * lp2->x)) * -invdet;
    invmx[2] = ((lp2->x * lp3->y) - (lp2->y * lp3->x)) * invdet;
    invmx[5] = ((lp1->x * lp3->y) - (lp1->y * lp3->x)) * -invdet;
    invmx[8] = ((lp1->x * lp2->y) - (lp1->y * lp2->x)) * invdet;
    for(i=0;i<3;i++){
      *(ptr++) = (float) tr_ptr->ls_nos[i]+1;
    }
    for(i=0;i<9;i++){
      *(ptr++) = (float) invmx[i];
    }
    tr_ptr = tr_ptr->next;
  }

  k=3;
  printf("Configured %d sets in 3 dimensions:\n", triplet_amount);
  for(i=0 ; i < triplet_amount ; i++) {
    printf("Triplet %d Loudspeakers: ", i);
    for (j=0 ; j < 3 ; j++) {
      printf("%d ",ls_data->lstripl[i][j]=(int) ls_table[k++]);
    }
    printf(" Matrix ");
    for (j=0 ; j < 9; j++) {
      printf("%f ", ls_data->lsm[i][j]=(double) ls_table[k++]);
    }
    printf("\n");
  }
  ls_data->triplet_amount=triplet_amount;
  ls_data->dimension=3;
  ls_data->numchannels=ls_amount;
}



void choose_ls_tuplets( ls lss[MAX_LS_AMOUNT],
			ls_triplet_chain **ls_triplets,
			int ls_amount, LS_DATA *ls_data)
     /* selects the loudspeaker pairs, calculates the inversion
	matrices and stores the data to a global array*/
{
  float atorad = (2 * 3.1415927 / 360) ;
  int i,j,k;
  float w1,w2;
  float p1,p2;
  int sorted_lss[MAX_LS_AMOUNT];
  int exist[MAX_LS_AMOUNT];
  int amount=0;
  float inv_mat[MAX_LS_AMOUNT][4], *ptr;
  float *ls_table;
  
  for(i=0;i<MAX_LS_AMOUNT;i++){
    exist[i]=0;
  }

  /* sort loudspeakers according their aximuth angle */
  sort_2D_lss(lss,sorted_lss,ls_amount);

  /* adjacent loudspeakers are the loudspeaker pairs to be used.*/
  for(i=0;i<(ls_amount-1);i++){
    if((lss[sorted_lss[i+1]].angles.azi - 
	lss[sorted_lss[i]].angles.azi) <= (3.1415927 - 0.175)){
      if (calc_2D_inv_tmatrix( lss[sorted_lss[i]].angles.azi, 
			       lss[sorted_lss[i+1]].angles.azi, 
			       inv_mat[i]) != 0){
	exist[i]=1;
	amount++;
      }
    }
  }

  if(((6.283 - lss[sorted_lss[ls_amount-1]].angles.azi) 
      +lss[sorted_lss[0]].angles.azi) <= (3.1415927 - 0.175)) {
    if(calc_2D_inv_tmatrix(lss[sorted_lss[ls_amount-1]].angles.azi, 
			   lss[sorted_lss[0]].angles.azi, 
			   inv_mat[ls_amount-1]) != 0) { 
      	exist[ls_amount-1]=1;
	amount++;
    } 
  }
  ls_table = (float*) malloc ((amount * 6 + 3 + 100 ) * sizeof (float));
  ls_table[0] = 2.0; /*dimension*/
  ls_table[1] = (float) ls_amount;
  ls_table[2] = (float) amount;
  ptr = &(ls_table[3]);
  for (i=0;i<ls_amount - 1;i++){
    if(exist[i] == 1) {
      *(ptr++) = sorted_lss[i]+1;
      *(ptr++) = sorted_lss[i+1]+1;
      for(j=0;j<4;j++) {
        *(ptr++) = inv_mat[i][j];
      }
    }
  }

  if(exist[ls_amount-1] == 1) {
    *(ptr++) = sorted_lss[ls_amount-1]+1;
    *(ptr++) = sorted_lss[0]+1;
    for(j=0;j<4;j++) {
      *(ptr++) = inv_mat[ls_amount-1][j];
    }
  }
  k=3;
  printf("Configured %d pairs in 2 dimensions:\n",amount);
  for(i=0 ; i < amount ; i++) {
    printf("Pair %d Loudspeakers: ", i);
    for (j=0 ; j < 2 ; j++) {
      ls_data->lstripl[i][j]=(int) ls_table[k];
      printf("%d ", (int) ls_table[k++]);
    }
    ls_data->lstripl[i][2]=0;
    printf(" Matrix ");
    for (j=0 ; j < 4; j++) {
      ls_data->lsm[i][j]=(double) ls_table[k];
      printf("%f ", ls_table[k]);
      k++;
    }
    for (j=4 ; j < 9; j++) {
      ls_data->lsm[i][j]=0.0;
    }
    printf("\n");
  }
  ls_data->triplet_amount=amount;
  ls_data->dimension=2;
  ls_data->numchannels=ls_amount;
}

void sort_2D_lss(ls lss[MAX_LS_AMOUNT], int sorted_lss[MAX_LS_AMOUNT], 
		 int ls_amount)
{
  int i,j,index;
  float tmp, tmp_azi;
  float rad2ang = 360.0 / ( 2 * 3.141592 );

  float x,y;
  /* Transforming angles between -180 and 180 */
  for (i=0;i<ls_amount;i++) {
    angle_to_cart2(&lss[i].angles, &lss[i].coords);
    lss[i].angles.azi = (float) acos((double) lss[i].coords.x);
    if (fabsf(lss[i].coords.y) <= 0.001)
    	tmp = 1.0;
    else
    	tmp = lss[i].coords.y / fabsf(lss[i].coords.y);
    lss[i].angles.azi *= tmp;
  }
  for (i=0;i<ls_amount;i++){
    tmp = 2000;
    for (j=0 ; j<ls_amount;j++){
      if (lss[j].angles.azi <= tmp){
	tmp=lss[j].angles.azi;
	index = j ;
      }
    }
    sorted_lss[i]=index;
    tmp_azi = (lss[index].angles.azi);
    lss[index].angles.azi = (tmp_azi + (float) 4000.0);
  }
  for (i=0;i<ls_amount;i++) {
    tmp_azi = (lss[i].angles.azi);
    lss[i].angles.azi = (tmp_azi - (float) 4000.0);
  }
}
  

int calc_2D_inv_tmatrix(float azi1,float azi2, float inv_mat[4])
{
  float x1,x2,x3,x4; /* x1 x3 */
  float y1,y2,y3,y4; /* x2 x4 */
  float det;
  x1 = (float) cos((double) azi1 );
  x2 = (float) sin((double) azi1 );
  x3 = (float) cos((double) azi2 );
  x4 = (float) sin((double) azi2 );
  det = (x1 * x4) - ( x3 * x2 );
   if(fabsf(det) <= 0.001) {

    inv_mat[0] = 0.0;
    inv_mat[1] = 0.0;
    inv_mat[2] = 0.0;
    inv_mat[3] = 0.0;
    return 0;
  } else {
    inv_mat[0] =  (float) (x4 / det);
    inv_mat[1] =  (float) (-x3 / det);
    inv_mat[2] =  (float) (-x2 / det);
    inv_mat[3] =  (float)  (x1 / det);
    return 1;
  }
}


