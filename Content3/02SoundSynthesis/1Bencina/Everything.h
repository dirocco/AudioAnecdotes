// include everything for testing

#include "ParabolicEnvelope.h"
#include "RaisedCosineBellEnvelope.h"
#include "TrapezoidalEnvelope.h"

#include "BasicAmplitudeEssence.h"
#include "BasicAmplitudeSustainSkewEssence.h"
#include "BasicSampleRateFrequencyPhaseEssence.h"

#include "StochasticAmplitudeEssence.h"
#include "StochasticAmplitudeSustainSkewEssence.h"

#include "SinusoidalSource.h"

#include "PeriodicSequenceStrategy.h"
#include "StochasticDensitySequenceStrategy.h"
#include "StochasticInteronsetTimeSequenceStrategy.h"

#include "Grain.h"
#include "Scheduler.h"

#include "EnvelopeIterator.h"

#include "FofGenerator.h"

#include "DelayLine.h"
#include "DelayLineTapSource.h"
#include "StochasticDelaySamplesPlaybackRateEssence.h"
#include "StochasticDelayLineGranulator.h"



#include "Sample.h"
#include "SamplePlaybackSource.h"
#include "StochasticInskipSamplesPlaybackRateEssence.h"
#include "StochasticStoredSampleGranulator.h"

#include "SinusoidalFMOscillator.h"
#include "StochasticSingleModulatorFMEssence.h"
#include "StochasticFMGranulator.h"