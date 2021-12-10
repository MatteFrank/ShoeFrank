#ifndef _TAGgeoTrafo_HXX
#define _TAGgeoTrafo_HXX

/*!
 \file TAGgeoTrafo.hxx
 \brief   Declaration of TAGgeoTrafo.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <TVector3.h>
#include <TString.h>
#include <TMath.h>
#include <TGeoMatrix.h>

#include "TAGaction.hxx"
#include "TAGparTools.hxx"

class TObjArray;

class TAGgeoTrafo : public TAGaction {

private:
   struct DeviceParameter_t : public  TNamed {
      TVector3  Center;  // current center
      TVector3  Angle;   // current angles
   };

   TAGparTools* fFileStream;
   TObjArray*   fMatrixList;
   TObjArray*   fDeviceList;
   TString      fExpName; // name of the experiment
   
private:
   static TString fgDefaultActName;
   static const Float_t fgkCmToMu;        //! conversion factor bw cm and micron
   static const Float_t fgkMuToCm;        //! conversion factor bw micron and cm
   static const Float_t fgkMmToMu;        //! conversion factor bw mm and micron
   static const Float_t fgkMuToMm;        //! conversion factor bw micron and mm
   static const Float_t fgkMmToCm;        //! conversion factor bw mm and cm
   static const Float_t fgkCmToMm;        //! conversion factor bw cm and mm
   static const Float_t fgkSecToNs;       //! conversion factor bw second and nanosecond
   static const Float_t fgkNsToSec;       //! conversion factor bw nanosecond and second
   static const Float_t fgkSecToPs;       //! conversion factor bw second and picosecond
   static const Float_t fgkPsToSec;       //! conversion factor bw picosecond and second
   static const Float_t fgkPsToNs;        //! conversion factor bw picosecond and nanosecond
   static const Float_t fgkNsToPs;        //! conversion factor bw nanosecond and picosecond
   static const Float_t fgkCmToM;         //! conversion factor bw cm and m
   static const Float_t fgkMToCm;         //! conversion factor bw m and cm
   static const Float_t fgkGevToMev;      //! conversion factor bw GeV and MeV
   static const Float_t fgkGevToKev;      //! conversion factor bw GeV and keV
   static const Float_t fgkMevToGev;      //! conversion factor bw MeV and GeV
   static const Float_t fgkGausToTesla;   //! conversion factor bw Gaus and Tesla
   static const Float_t fgkGausToKGaus;   //! conversion factor bw Gaus and kGaus

   static const Float_t fgkMassFactor;     //! default proton mass
   static const Float_t fgkElectronMass;   //! default electron mass
   static const Float_t fgkLightVelocity;  //! Speed of light in vacuum

   static const Char_t* fgkGeomName;       //! name of GeoManager
   static const Char_t* fgkGeomTitle;      //! title of GeoManager
   static       Char_t  fgDefaultTransp;   //! default transparency value
   static const Char_t* fgkTrafoBaseName;  //! name of GeoManager
   static map<TString, Int_t> fgkDeviceType; //! device name

public:
   TAGgeoTrafo(const TString expName = "FOOT");
   virtual ~TAGgeoTrafo();

   bool FromFile(TString ifile = "./geomaps/FOOT.geo");

   // Start Counter
   TVector3 GetSTCenter();
   TVector3 GetSTAngles();

   // Beam Monitor
   TVector3 GetBMCenter();
   TVector3 GetBMAngles();

   // Target
   TVector3 GetTGCenter();
   TVector3 GetTGAngles();

   // Dipole
   TVector3 GetDICenter();
   TVector3 GetDIAngles();

   // Vertex
   TVector3 GetVTCenter();
   TVector3 GetVTAngles();

   // Inner tracker
   TVector3 GetITCenter();
   TVector3 GetITAngles();

   // MSD
   TVector3 GetMSDCenter();
   TVector3 GetMSDAngles();

   // tof Wall
   TVector3 GetTWCenter();
   TVector3 GetTWAngles();

   // Calorimeter
   TVector3 GetCACenter();
   TVector3 GetCAAngles();

   TVector3 FromSTLocalToGlobal(TVector3 apoi);
   TVector3 FromBMLocalToGlobal(TVector3 apoi);
   TVector3 FromTGLocalToGlobal(TVector3 apoi);
   TVector3 FromDILocalToGlobal(TVector3 apoi);
   TVector3 FromVTLocalToGlobal(TVector3 apoi);
   TVector3 FromITLocalToGlobal(TVector3 apoi);
   TVector3 FromMSDLocalToGlobal(TVector3 apoi);
   TVector3 FromTWLocalToGlobal(TVector3 apoi);
   TVector3 FromCALocalToGlobal(TVector3 apoi);

   TVector3 VecFromSTLocalToGlobal(TVector3 avec);
   TVector3 VecFromBMLocalToGlobal(TVector3 avec);
   TVector3 VecFromTGLocalToGlobal(TVector3 avec);
   TVector3 VecFromDILocalToGlobal(TVector3 avec);
   TVector3 VecFromVTLocalToGlobal(TVector3 avec);
   TVector3 VecFromITLocalToGlobal(TVector3 avec);
   TVector3 VecFromMSDLocalToGlobal(TVector3 avec);
   TVector3 VecFromTWLocalToGlobal(TVector3 avec);
   TVector3 VecFromCALocalToGlobal(TVector3 avec);

   TVector3 FromGlobalToSTLocal(TVector3 apoi);
   TVector3 FromGlobalToBMLocal(TVector3 apoi);
   TVector3 FromGlobalToTGLocal(TVector3 apoi);
   TVector3 FromGlobalToDILocal(TVector3 apoi);
   TVector3 FromGlobalToVTLocal(TVector3 apoi);
   TVector3 FromGlobalToITLocal(TVector3 apoi);
   TVector3 FromGlobalToMSDLocal(TVector3 apoi);
   TVector3 FromGlobalToTWLocal(TVector3 apoi);
   TVector3 FromGlobalToCALocal(TVector3 apoi);

   TVector3 VecFromGlobalToSTLocal(TVector3 avec);
   TVector3 VecFromGlobalToBMLocal(TVector3 avec);
   TVector3 VecFromGlobalToTGLocal(TVector3 avec);
   TVector3 VecFromGlobalToDILocal(TVector3 avec);
   TVector3 VecFromGlobalToVTLocal(TVector3 avec);
   TVector3 VecFromGlobalToITLocal(TVector3 avec);
   TVector3 VecFromGlobalToMSDLocal(TVector3 avec);
   TVector3 VecFromGlobalToTWLocal(TVector3 avec);
   TVector3 VecFromGlobalToCALocal(TVector3 avec);

   TVector3 FromTGLocalToBMLocal(TVector3 apoi);
   TVector3 FromTGLocalToVTLocal(TVector3 apoi);
   TVector3 FromTGLocalToITLocal(TVector3 apoi);
   TVector3 FromITLocalToVTLocal(TVector3 apoi);
   TVector3 FromTWLocalToVTLocal(TVector3 apoi);

   const TGeoHMatrix*   GetTrafo(const char* name) const;
   TGeoCombiTrans*      GetCombiTrafo(const char* name) const;
   void                 AddTrafo(TGeoHMatrix* mat);
   void                 AddDevice(DeviceParameter_t* device);
   TVector3             GetDeviceCenter(const char* name) const;
   TVector3             GetDeviceAngle(const char* name) const;

   TVector3             Global2Local(const char* name, TVector3& glob) const;
   TVector3             Global2LocalVect(const char* name, TVector3& glob) const;
   TVector3             Local2Global(const char* name, TVector3& loc)  const;
   TVector3             Local2GlobalVect(const char* name, TVector3& loc)  const;

public:
   static void      SetDefaultActName(TString& name) { fgDefaultActName = name; }
   static TString   GetDefaultActName()              { return fgDefaultActName; }

   static           Float_t CmToMu()                 { return fgkCmToMu;        }
   static           Float_t MuToCm()                 { return fgkMuToCm;        }

   static           Float_t MmToMu()                 { return fgkMmToMu;        }
   static           Float_t MuToMm()                 { return fgkMuToMm;        }

   static           Float_t MmToCm()                 { return fgkMmToCm;        }
   static           Float_t CmToMm()                 { return fgkCmToMm;        }

   static           Float_t MToCm()                  { return fgkMToCm;         }
   static           Float_t CmToM()                  { return fgkCmToM;         }

   static           Float_t SecToNs()                { return fgkSecToNs;       }
   static           Float_t NsToSec()                { return fgkNsToSec;       }

   static           Float_t SecToPs()                { return fgkSecToPs;       }
   static           Float_t PsToSec()                { return fgkPsToSec;       }

   static           Float_t PsToNs()                 { return fgkPsToNs;        }
   static           Float_t NsToPs()                 { return fgkNsToPs;        }

   static           Float_t GevToMev()               { return fgkGevToMev;      }
   static           Float_t MevToGev()               { return fgkMevToGev;      }

   static           Float_t GevToKev()               { return fgkGevToKev;      }
   static           Float_t GausToTesla()            { return fgkGausToTesla;   }
   static           Float_t GausToKGaus()            { return fgkGausToKGaus;   }

   static           Float_t GetMassFactorMeV()       { return fgkMassFactor*fgkGevToMev;  }
   static           Float_t GetMassFactor()          { return fgkMassFactor;    }

   static           Float_t GetElectronMassMeV()     { return fgkElectronMass;  }
   static           Float_t GetElectronMass()        { return fgkElectronMass*fgkMevToGev; }
   static           Float_t GetLightVelocity()       { return fgkLightVelocity; }


   static     const Char_t* GetBaseName()            { return fgkGeomName;      }
   static     const Char_t* GetDefaultGeomName()     { return fgkGeomName;      }
   static     const Char_t* GetDefaultGeomTitle()    { return fgkGeomTitle;     }

   static           Char_t  GetDefaultTransp()       { return fgDefaultTransp;  }
   static           void    SetDefaultTransparency(Char_t transparency) { fgDefaultTransp = transparency; }
   static           Int_t   GetDeviceType(const TString name) { return fgkDeviceType[name];}
   static    const Char_t*  GetDeviceName(Int_t devType);
   
   ClassDef(TAGgeoTrafo,1)
};

#endif
