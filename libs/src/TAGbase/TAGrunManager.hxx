#ifndef _TAGrunManager_HXX
#define _TAGrunManager_HXX
/*!
 \file TAGrunManager.hxx
 \brief   Declaration of TAGrunManager.
 */
/*------------------------------------------+---------------------------------*/
#include <map>
#include <TArrayI.h>
#include <TString.h>
#include <TMath.h>
#include <TGeoMatrix.h>

#include "TAGaction.hxx"
#include "TAGparTools.hxx"


using namespace std;

class TAGrunManager : public TAGaction {
public:
   /*!
    \struct TypeParameter_t
    \brief  Run type parameters
    */
   struct TypeParameter_t : public  TNamed {
      Int_t     TypeId;      ///< Index type run
      TString   TypeName;    ///< Type of run name
      TString   Trigger;     ///< Trigger type
      TString   Beam;        ///< Beam element
      Float_t   BeamEnergy;  ///< Beam energy per nucleon
      TString   Target;      ///< Target element
      Float_t   TargetSize;  ///< Target size
      Int_t     TotalEvts;   ///< Total event niumber
      TString   Comments;    ///< Comments
   };
   
   /*!
    \struct RunParameter_t
    \brief  Run  parameters
    */
   struct RunParameter_t : public  TNamed {
      Int_t     RunId;       ///< Index type run
      TString   StartTime;   ///< Run starting time
      TString   StopTime;    ///< Run stopping time
      Int_t     DaqEvts;     ///< DAQ event niumber in run
      Int_t     Duration;    ///< Run duration
      Int_t     DaqRate;     ///< DAQ rate niumber in run
      Int_t     RunType;     ///< Run type
   };

private:
   TAGparTools*    fFileStream;     ///< File stream
   TString         fCampaignName;   ///< Current campaign name
   Int_t           fRunNumber;      ///< Current campaign number
   TString         fName;           ///< Campaign name
   TArrayI         fRunArray;       ///< Run array
   TypeParameter_t fCurType;        ///< Current type parameter

   map<int, TypeParameter_t> fTypeParameter; ///< Run type parameter
   map<int, RunParameter_t>  fRunParameter; ///< Run type parameter
   
public:
   TAGrunManager(const TString exp = "", Int_t runNumber = -1);
   virtual ~TAGrunManager();
   
   // From file
   Bool_t               FromFile(TString ifile = "");
   
   //! Get campaign name
   const Char_t*        GetName()                const { return fName.Data();        }
   //! Get run array of campaign
   const TArrayI&       GetRunArray()            const { return fRunArray;           }
   
   //! Get run number
   Int_t                GetRunNumber()           const { return fRunNumber;          }

   //! Set run number
   void                 SetRunNumber(Int_t run)        { fRunNumber = run ;          }

   //! Get Current type
   TypeParameter_t      GetCurrentType()         const { return fCurType ;           }
   
   //! Get parameter for a given run type
   TypeParameter_t&     GetTypePar(Int_t idx)          { return fTypeParameter[idx]; }
   //! Get parameter for a given run
   RunParameter_t&      GetRunPar(Int_t idx)           { return fRunParameter[idx];  }

   // Print out informations
   void                 Print(Option_t* opt = "") const;
   
private:
   // Decode type info
   void DecodeTypeLine(TString& line);
   // Decode run info
   void DecodeRunLine(TString& line);

private:
   static const TString fgkDefaultActName;  ///< Default action name
   static const TString fgkDefaultFolder;   ///< Default campaign folder
   static const TString fgkDefaultExt;      ///< Default extension name
   
public:
   //! Get default action name
   static TString       GetDefaultActName() { return fgkDefaultActName;        }
   //! Get default campaign folder
   static const Char_t* GetDefaultFolder()  { return fgkDefaultFolder.Data();  }
   //! Get default extension name
   static const Char_t* GetDefaultExt()     { return fgkDefaultExt.Data();     }
   
   ClassDef(TAGrunManager,1)
};

#endif