#ifndef INCLUDED_BASICAMPLITUDEESSENCE_H
#define INCLUDED_BASICAMPLITUDEESSENCE_H

/*
    BasicAmplitudeEssence models:
    concept AmplitudeEssenceT {
        float amplitude();
    }
*/

class BasicAmplitudeEssence{
public:
    BasicAmplitudeEssence() {}

    BasicAmplitudeEssence( float amplitude )
        : amplitude_( amplitude ) {}

    BasicAmplitudeEssence& amplitude( float x )
        { amplitude_ = x; return *this; }

    float amplitude() const {
        return amplitude_;
    }

private:
    float amplitude_;
};

#endif /* INCLUDED_BASICAMPLITUDEESSENCE_H */
