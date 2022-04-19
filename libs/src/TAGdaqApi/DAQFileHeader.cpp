/*!
 \file DAQFileHeader.cpp
 \brief  Implementation of DAQFileHeader
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "DAQFileHeader.hh"

/*!
 \class DAQFileHeader
 \brief Base class for DAQ file header
 */


//------------------------------------------+-----------------------------------
//! Print data
void DAQFileHeader::printData () const{

    printf ("\n");
    printf ("-------------------HEADER OF FILE--------------------\n");
    printf ("Info Header WRITER=: %s\n",  writer.c_str());
    printf ("Info Header FILENAME=: %s\n",  filename.c_str());
    printf ("Info Header number of file=: %d\n", fileNumber);
    printf ("Info Header GUID: %s\n",  guid.c_str());
    printf ("Info Header stream: %s\n",  stream.c_str());
    printf ("Info Header project: %s\n",  project.c_str());
    printf ("Info Header lumi: %s\n",  lumiblock.c_str());
    printf ("-------------------END OF HEADER FILE----------------\n");
    printf ("\n");

}
