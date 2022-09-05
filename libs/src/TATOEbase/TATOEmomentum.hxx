//
//File      : TATOEmomentum.hxx
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 12/05/2021
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//

#ifndef _TATOEmomentum_HXX
#define _TATOEmomentum_HXX

/*!
 \file TATOEmomentum.hxx
 \brief Declaration of TATOEmomentum
 \author A. Sécher
 */


#include "TAGntuGlbTrack.hxx"

// Get momentum for a given position in Z
TVector3 GetMomentumAtZ( TAGtrack* track_ph, double z );


#endif
