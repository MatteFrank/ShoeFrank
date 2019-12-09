#ifndef _TAMSD_ContainerPoint_HXX
#define _TAMSD_ContainerPoint_HXX
/*!
 \file
 \version $Id: TAMSD_ContainerPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAMSD_ContainerPoint.
 */
/*------------------------------------------+---------------------------------*/


#include "TAMSD_Point.hxx"

#include "TAGdata.hxx"
#include <map>


class TAMSDparGeo;
class TAMSD_ContainerPoint : public TAGdata {
   
private:
   

	// class DegeneratePoint {
	// public: 
	// 	DegeneratePoint() {};
	// 	~DegeneratePoint() {};

	// 	void SetPrimaryPoint( TAMSD_Point* point )		{primaryPoint = point;};
	// 	void AddPoint( TAMSD_Point* point )				{allPoints.push_back(point);};
	// 	TAMSD_Point* GetPrimaryPoint()					{return primaryPoint;};
	// 	vector<TAMSD_Point*> GetAllPoints()				{return allPoints;};

	// 	TAMSD_Point* primaryPoint;
	// 	vector<TAMSD_Point*> allPoints;
	// };

	// struct DegeneratePoint {
	// 	TAMSD_Point* primaryPoint;
	// 	vector<TAMSD_Point*> allPoints;
	// };



	TObjArray*        m_listOfPoints; 
	static TString    fgkBranchName;    // Branch name in TTree

	TAMSDparGeo* m_msdGeo;

	// vector<int> m_pointVector;
	// map<int, DegeneratePoint> m_degeneratePointMap;


	// virtual void      SetupClones();



public:

	struct DegeneratePoint {
		TAMSD_Point* primaryPoint;
		vector<TAMSD_Point*> allPoints;
	};

	vector<int> m_pointVector;
	// map<int, DegeneratePoint> m_degeneratePointMap;


	TAMSD_ContainerPoint();
	virtual ~TAMSD_ContainerPoint();
	
	TAMSD_Point*         NewPoint( int layer, int iCol, int iRow, TVector3 pos );

	TClonesArray* GetListOfPoints(int iLayer);

	int                 	GetPointN(); 
	TAMSD_Point*         	GetPoint( int iPoint );
	// vector<TAMSD_Point*> 	GetPoint_AllTheDegenerate( int iPoint );

	int GetPointN_includingGhosts();
	// TAMSD_Point* GetPoint_includingGhosts( int iPoint );


	// int AlgoLinearizeCoord( int layer, int col, int row );
	// void AlgoLinearizeCoord_inverse( int mask, int* layer, int* col, int* row );

	virtual void      SetupClones();
	virtual void        Clear(Option_t* opt="");

	// delete?
	virtual void        ToStream(ostream& os=cout, Option_t* option="") const;
 
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAMSD_ContainerPoint,1)
};

#endif



















