/*
    Ross Bencina's Granular Synthesis Toolkit (RB-GST)
    Copyright (C) 2001 Ross Bencina.
    email: rossb@audiomulch.com
    web: http://www.audiomulch.com/~rossb/

    This file is part of RB-GST.

    RB-GST is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    RB-GST is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RB-GST; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef INCLUDED_DCBLOCKER_H
#define INCLUDED_DCBLOCKER_H

class DCBlocker{
public:
    DCBlocker() : pos_( 0.f ), speed_( 0.f ) {}
    
    float operator()( float input ){
        speed_ = speed_ + (input - pos_) * 0.000004567f;
        pos_ = pos_ + speed_;
        speed_ = speed_ * 0.96f;
        return input - pos_;
    }
private:
    float pos_, speed_;
};

#endif /* INCLUDED_DCBLOCKER_H */
