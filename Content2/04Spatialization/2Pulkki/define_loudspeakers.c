/* defining loudspeaker data */
define_loudspeakers(&ls_data, ls_set_dim, ls_num, ls_dirs);

/* ls_data is a struct containing data specific to loudspeaker setup
ls_set_dim is 2 if loudspeakers are on a (horizontal) plane
ls_set_dim is 3 if also elevated or descended loudpeakers exist
ls_num is the number of loudspeakers
ls_dirs is an array containing the angular directions of loudspeakers*/

/* calculate gain factors for virtual source
   in direction (int azimuth, int elevation) */
vbap(gains, &ls_data, azimuth, elevation);

/* panning monophonic audio to multiple outputs*/
for(j=0;j<num_of_buffers;j++){
  gainptr=gains;
  for (k=0; k<numchannels; k++){
    outptr= out[k]; /* outptr points to current output channel */
    inptr = in; /* inptr points to audio to be panned */
    gain = *gainptr++; /* gain factor for this channel (loudspeaker)*/
    if(gain != 0.0)
      for (i=0; i<BUFFER_LENGTH; ++i) /* panning */
        *outptr++ = *inptr++ * gain;
    else /* no output to this channel */
    for (i=0; i<BUFFER_LENGTH; ++i)
      *outptr++ = 0.0;
  }
  /* to be added:
  buffer "out" is written to your multi-channel audio device */
}
