#ifndef INCLUDED_ENVELOPEITERATOR_H
#define INCLUDED_ENVELOPEITERATOR_H

/*
    EnvelopeIterator is an adapter which makes
    an envelope behave like an iterator. Using
    an EnvelopeIterator directly is less efficient
    but more convenient than using the standard
    envelope interface.

    depends on GrainEnvelopeT (see Grain.h)
*/

template< class GrainEnvelopeT >
class EnvelopeIterator{
public:
    typedef GrainEnvelopeT envelope_type;

    EnvelopeIterator( envelope_type& envelope )
        : envelope_( envelope ) {}

    float operator*() const {
        return *envelope_;
    }

    EnvelopeIterator& operator++(){
        if( !envelope_.atEnd() ){
            envelope_.advance();
            envelope_.checkBoundary(1);
        }
        return *this;
    }

private:
    envelope_type& envelope_;
};

#endif /* INCLUDED_ENVELOPEITERATOR_H */
