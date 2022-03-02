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
   /*!
    \struct DeviceParameter_t
    \brief  Device parameters
    */
   struct DeviceParameter_t : public  TNamed {
      TVector3  Center;  ///< current center
      TVector3  Angle;   ///< current angles
   };

   TAGparTools* fFileStream; ///< File stream
   TObjArray*   fMatrixList; ///< List of transformation matrices
   TObjArray*   fDeviceList; ///< List of devices
   TString      fExpName;    ///< Name of the experiment
   
private:
   static TString fgDefaultActName;
   static const Float_t fgkCmToMu;        ///< conversion factor bw cm and micron
   static const Float_t fgkMuToCm;        ///< conversion factor bw micron and cm
   static const Float_t fgkMmToMu;        ///< conversion factor bw mm and micron
   static const Float_t fgkMuToMm;        ///< conversion factor bw micron and mm
   static const Float_t fgkMmToCm;        ///< conversion factor bw mm and cm
   static const Float_t fgkCmToMm;        ///< conversion factor bw cm and mm
   static const Float_t fgkSecToNs;       ///< conversion factor bw second and nanosecond
   static const Float_t fgkNsToSec;       ///< conversion factor bw nanosecond and second
   static const Float_t fgkSecToPs;       ///< conversion factor bw second and picosecond
   static const Float_t fgkPsToSec;       ///< conversion factor bw picosecond and second
   static const Float_t fgkPsToNs;        ///< conversion factor bw picosecond and nanosecond
   static const Float_t fgkNsToPs;        ///< conversion factor bw nanosecond and picosecond
   static const Float_t fgkCmToM;         ///< conversion factor bw cm and m
   static const Float_t fgkMToCm;         ///< conversion factor bw m and cm
   static const Float_t fgkGevToMev;      ///< conversion factor bw GeV and MeV
   static const Float_t fgkGevToKev;      ///< conversion factor bw GeV and keV
   static const Float_t fgkMevToGev;      ///< conversion factor bw MeV and GeV
   static const Float_t fgkGausToTesla;   ///< conversion factor bw Gaus and Tesla
   static const Float_t fgkGausToKGaus;   ///< conversion factor bw Gaus and kGaus

   static const Float_t fgkMassFactor;     ///< default proton mass
   static const Float_t fgkElectronMass;   ///< default electron mass
   static const Float_t fgkLightVelocity;  ///< Speed of light in vacuum

   static const Char_t* fgkGeomName;       ///< name of GeoManager
   static const Char_t* fgkGeomTitle;      ///< title of GeoManager
   static       Char_t  fgDefaultTransp;   ///< default transparency value
   static const Char_t* fgkTrafoBaseName;  ///< name of GeoManager
   static map<TString, Int_t> fgkDeviceType; ///< device name

public:
   TAGgeoTrafo(const TString expName = "FOOT");
   virtual ~TAGgeoTrafo();

   // From file
   bool FromFile(TString ifile = "./geomaps/FOOT.geo");

   // Get Start Counter center
   TVector3 GetSTCenter();
   // Get Start Counter angle
   TVector3 GetSTAngles();

   // Get Beam Monitor center
   TVector3 GetBMCenter();
   // Get Beam Monitor angle
   TVector3 GetBMAngles();

   // Get Target center
   TVector3 GetTGCenter();
   // Get Target angle
   TVector3 GetTGAngles();

   // Get Dipole center
   TVector3 GetDICenter();
   // Get Dipole angle
   TVector3 GetDIAngles();

   // Get Vertex center
   TVector3 GetVTCenter();
   // Get Vertex angle
   TVector3 GetVTAngles();

   // Get Inner tracker center
   TVector3 GetITCenter();
   // Get Inner tracker angle
   TVector3 GetITAngles();

   // Get MSD center
   TVector3 GetMSDCenter();
   // Get MSD angle
   TVector3 GetMSDAngles();

   // Get Tof Wall center
   TVector3 GetTWCenter();
   // Get Tof Wall angle
   TVector3 GetTWAngles();

   // Get Calorimeter center
   TVector3 GetCACenter();
   // Get Calorimeter angle
   TVector3 GetCAAngles();

   // Get from local STC to global FOOT framework
   TVector3 FromSTLocalToGlobal(TVector3 apoi);
   // Get from local BM to global FOOT framework
   TVector3 FromBMLocalToGlobal(TVector3 apoi);
   // Get from local target to global FOOT framework
   TVector3 FromTGLocalToGlobal(TVector3 apoi);
   // Get from local dipole to global FOOT framework
   TVector3 FromDILocalToGlobal(TVector3 apoi);
   // Get from local VTX to global FOOT framework
   TVector3 FromVTLocalToGlobal(TVector3 apoi);
   // Get from local ITR to global FOOT framework
   TVector3 FromITLocalToGlobal(TVector3 apoi);
   // Get from local MSD to global FOOT framework
   TVector3 FromMSDLocalToGlobal(TVector3 apoi);
   // Get from local TW to global FOOT framework
   TVector3 FromTWLocalToGlobal(TVector3 apoi);
   // Get from local CAL to global FOOT framework
   TVector3 FromCALocalToGlobal(TVector3 apoi);

   // Get from local STC to global FOOT framework for vectors
   TVector3 VecFromSTLocalToGlobal(TVector3 avec);
   // Get from local BM to global FOOT framework for vectors
   TVector3 VecFromBMLocalToGlobal(TVector3 avec);
   // Get from local target to global FOOT framework for vectors
   TVector3 VecFromTGLocalToGlobal(TVector3 avec);
   // Get from local dipole to global FOOT framework for vectors
   TVector3 VecFromDILocalToGlobal(TVector3 avec);
   // Get from local VTX to global FOOT framework for vectors
   TVector3 VecFromVTLocalToGlobal(TVector3 avec);
   // Get from local ITR to global FOOT framework for vectors
   TVector3 VecFromITLocalToGlobal(TVector3 avec);
   // Get from local MSD to global FOOT framework for vectors
   TVector3 VecFromMSDLocalToGlobal(TVector3 avec);
   // Get from local TW to global FOOT framework for vectors
   TVector3 VecFromTWLocalToGlobal(TVector3 avec);
   // Get from local CAL to global FOOT framework for vectors
   TVector3 VecFromCALocalToGlobal(TVector3 avec);

   // Get from global FOOT to local STC framework
   TVector3 FromGlobalToSTLocal(TVector3 apoi);
   // Get from global FOOT to local BM framework
   TVector3 FromGlobalToBMLocal(TVector3 apoi);
   // Get from global FOOT to local target framework
   TVector3 FromGlobalToTGLocal(TVector3 apoi);
   // Get from global FOOT to local dipole framework
   TVector3 FromGlobalToDILocal(TVector3 apoi);
   // Get from global FOOT to local VTX framework
   TVector3 FromGlobalToVTLocal(TVector3 apoi);
   // Get from global FOOT to local ITR framework
   TVector3 FromGlobalToITLocal(TVector3 apoi);
   // Get from global FOOT to local MSD framework
   TVector3 FromGlobalToMSDLocal(TVector3 apoi);
   // Get from global FOOT to local TW framework
   TVector3 FromGlobalToTWLocal(TVector3 apoi);
   // Get from global FOOT to local CAL framework
   TVector3 FromGlobalToCALocal(TVector3 apoi);

   // Get from global FOOT to local STC framework for vectors
   TVector3 VecFromGlobalToSTLocal(TVector3 avec);
   // Get from global FOOT to local BM framework for vectors
   TVector3 VecFromGlobalToBMLocal(TVector3 avec);
   // Get from global FOOT to local target framework for vectors
   TVector3 VecFromGlobalToTGLocal(TVector3 avec);
   // Get from global FOOT to local dipole framework for vectors
   TVector3 VecFromGlobalToDILocal(TVector3 avec);
   // Get from global FOOT to local VTX framework for vectors
   TVector3 VecFromGlobalToVTLocal(TVector3 avec);
   // Get from global FOOT to local ITR framework for vectors
   TVector3 VecFromGlobalToITLocal(TVector3 avec);
   // Get from global FOOT to local MSD framework for vectors
   TVector3 VecFromGlobalToMSDLocal(TVector3 avec);
   // Get from global FOOT to local TW framework for vectors
   TVector3 VecFromGlobalToTWLocal(TVector3 avec);
   // Get from global FOOT to local CAL framework for vectors
   TVector3 VecFromGlobalToCALocal(TVector3 avec);

   // Get from local target to local BM framework
   TVector3 FromTGLocalToBMLocal(TVector3 apoi);
   // Get from local target to local VTX framework
   TVector3 FromTGLocalToVTLocal(TVector3 apoi);
   // Get from local target to local ITR framework
   TVector3 FromTGLocalToITLocal(TVector3 apoi);
   // Get from local ITR to local VTX framework
   TVector3 FromITLocalToVTLocal(TVector3 apoi);
   // Get from local TW to local VTX framework
   TVector3 FromTWLocalToVTLocal(TVector3 apoi);

   // Get transformation matrix for a given device
   const TGeoHMatrix*   GetTrafo(const char* name) const;
   // Get combinaison transformation matrix for a given device
   TGeoCombiTrans*      GetCombiTrafo(const char* name) const;
   // Add transformation matrix
   void                 AddTrafo(TGeoHMatrix* mat);
   // Add device
   void                 AddDevice(DeviceParameter_t* device);
   // Get center for device
   TVector3             GetDeviceCenter(const char* name) const;
   // Get angle for device
   TVector3             GetDeviceAngle(const char* name) const;

   // FOOT Global to device local transformation
   TVector3             Global2Local(const char* name, TVector3& glob) const;
   // FOOT Global to device local transformation for vector
   TVector3             Global2LocalVect(const char* name, TVector3& glob) const;
   // Local device to FOOT global transformation
   TVector3             Local2Global(const char* name, TVector3& loc)  const;
   // Local device to FOOT global transformation for vector
   TVector3             Local2GlobalVect(const char* name, TVector3& loc)  const;

public:
   //! Set default action name
   static void      SetDefaultActName(TString& name) { fgDefaultActName = name; }
   //! Get default action name
   static TString   GetDefaultActName()              { return fgDefaultActName; }

   //! Conversion factor of centimeter in micrometer
   static           Float_t CmToMu()                 { return fgkCmToMu;        }
   //! Conversion factor of micrometer in centimeter
   static           Float_t MuToCm()                 { return fgkMuToCm;        }

   //! Conversion factor of milimeter in micrometer
   static           Float_t MmToMu()                 { return fgkMmToMu;        }
   //! Conversion factor of micrometer in milimeter
   static           Float_t MuToMm()                 { return fgkMuToMm;        }

   //! Conversion factor of milimeter in centimeter
   static           Float_t MmToCm()                 { return fgkMmToCm;        }
   //! Conversion factor of centimeter in milimeter
   static           Float_t CmToMm()                 { return fgkCmToMm;        }

   //! Conversion factor of meter in centimeter
   static           Float_t MToCm()                  { return fgkMToCm;         }
   //! Conversion factor of centimeter in meter
   static           Float_t CmToM()                  { return fgkCmToM;         }

   //! Conversion factor of second in nanosecond
   static           Float_t SecToNs()                { return fgkSecToNs;       }
   //! Conversion factor of nanosecond in second
   static           Float_t NsToSec()                { return fgkNsToSec;       }

   //! Conversion factor of second in picosecond
   static           Float_t SecToPs()                { return fgkSecToPs;       }
   //! Conversion factor of picosecond in second
   static           Float_t PsToSec()                { return fgkPsToSec;       }

   //! Conversion factor of picosecond in nanosecond
   static           Float_t PsToNs()                 { return fgkPsToNs;        }
   //! Conversion factor of nanosecond in picosecond
   static           Float_t NsToPs()                 { return fgkNsToPs;        }

   //! Conversion factor of GeV in MeV
   static           Float_t GevToMev()               { return fgkGevToMev;      }
   //! Conversion factor of MeV in GeV
   static           Float_t MevToGev()               { return fgkMevToGev;      }
   //! Conversion factor of GeV in keV
   static           Float_t GevToKev()               { return fgkGevToKev;      }
   
   //! Conversion factor of Gass in Tesla
   static           Float_t GausToTesla()            { return fgkGausToTesla;   }
   //! Conversion factor of Gass in KiloGaus
   static           Float_t GausToKGaus()            { return fgkGausToKGaus;   }

   //! Get mass factor in MeV (938.3 MeV/c^2)
   static           Float_t GetMassFactorMeV()       { return fgkMassFactor*fgkGevToMev;  }
   //! Get mass factor in GeV (0.9383 GeV/c^2)
   static           Float_t GetMassFactor()          { return fgkMassFactor;    }

   //! Get Electron mass in MeV (510.9 MeV/c^2)
   static           Float_t GetElectronMassMeV()     { return fgkElectronMass;  }
   //! Get Electron mass in GeV (0.5109 GeV/c^2)
   static           Float_t GetElectronMass()        { return fgkElectronMass*fgkMevToGev; }
   //! Get speed of light in cm/ns (29.98 cm/ns)
   static           Float_t GetLightVelocity()       { return fgkLightVelocity; }

   //! Get base name
   static     const Char_t* GetBaseName()            { return fgkGeomName;      }
   //! Get default TGeoManager name
   static     const Char_t* GetDefaultGeomName()     { return fgkGeomName;      }
   //! Get default TGeoManager title
   static     const Char_t* GetDefaultGeomTitle()    { return fgkGeomTitle;     }

   //! Get default transparency for volume
   static           Char_t  GetDefaultTransp()       { return fgDefaultTransp;  }
   //! Set default transparency for volume
   static           void    SetDefaultTransparency(Char_t transparency) { fgDefaultTransp = transparency; }
   //! Get device number from device name
   static           Int_t   GetDeviceType(const TString name) { return fgkDeviceType[name];}
   // Get device name from device number
   static    const Char_t*  GetDeviceName(Int_t devType);
   
   ClassDef(TAGgeoTrafo,1)
};

#endif
