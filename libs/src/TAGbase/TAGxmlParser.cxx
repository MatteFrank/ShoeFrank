#include <iostream>
#include "TError.h"

#include "TAGxmlParser.hxx"
/*!
 \file TAGxmlParser.cxx
 \brief   Implementation of TAGxmlParser.
 */

#include <TSystem.h>
#include "TAGobject.hxx"

/*!
 \class TAGxmlParser
 \brief XML parser class. **
 */

//------------------------------------------+-----------------------------------
//! Constructor
TAGxmlParser::TAGxmlParser()
{
  fXMLDoc=nullptr;
  //  create engine
  fXMLEngine= new TXMLEngine;
}

//------------------------------------------+-----------------------------------
//! Destructor
TAGxmlParser::~TAGxmlParser()
{
  // take care of the document
  if (fXMLDoc!=nullptr)
  {
    fXMLEngine->FreeDoc(fXMLDoc);
  }
  delete fXMLEngine;
}

//------------------------------------------+-----------------------------------
//! open the xml file
//!
//! \param[in] FileName file name
void TAGxmlParser::ReadFile(std::string FileName)
{
   // Only file with restricted xml syntax are supported
   fXMLDoc = fXMLEngine->ParseFile(FileName.c_str());
   if (fXMLDoc==0)
   {
     Error("ReadFile()", "Cannot open file %s", FileName.data());
     return;
   }
}

//------------------------------------------+-----------------------------------
//!  Get the all the child node of StartingNode named NodeName
//!
//! \param[in] StartingNode starting node
//! \param[in] NodeName node name
std::vector<XMLNodePointer_t> TAGxmlParser::GetChildNodesByName(XMLNodePointer_t StartingNode, std::string NodeName)
{
  // get first child
  XMLNodePointer_t child = fXMLEngine->GetChild(StartingNode);
  std::vector<XMLNodePointer_t> NodeVec;
  // navigate all the first generation node
  while (child!=0)
  {
    // if current node name is equal to
    if (strcmp(fXMLEngine->GetNodeName(child), NodeName.c_str())==0)
    {
      NodeVec.push_back(child);
    }
    child = fXMLEngine->GetNext(child);
  }
  // return a vector containing all the selected nodes
  return NodeVec;
}

//------------------------------------------+-----------------------------------
//! Get content of a node as Int_t. In case a node has no content an exception is thrown
//! If there are none or multiple nodes named Name and exception is thrown
//!
//! \param[in] Name node name
//! \param[in] Node node
Int_t TAGxmlParser::GetContentAsInt(std::string Name,  XMLNodePointer_t Node)
{
      std::vector<XMLNodePointer_t> tmp= this->GetChildNodesByName(Node,Name);
      if (tmp.size()!=1)
      {
        throw -1;
      }
      const char* content=fXMLEngine->GetNodeContent(tmp[0]);
      if (content!=0)
      {
        return TString(content).Atoi();
      }
      throw -1;
}
//------------------------------------------+-----------------------------------
//! Get content of a node as string. In case a node has no content an exception is thrown
//! If there are none or multiple nodes named Name and exception is thrown
//!
//! \param[in] Name node name
//! \param[in] Node node
std::string TAGxmlParser::GetContentAsString(std::string Name,XMLNodePointer_t Node)
{
      std::vector<XMLNodePointer_t> tmp= this->GetChildNodesByName(Node,Name);
      if (tmp.size()!=1)
      {
        throw -1;
      }
      const char* content=fXMLEngine->GetNodeContent(tmp[0]);
      if (content!=0)
      {
        return std::string(content);
      }
      throw -1;
}
//------------------------------------------+-----------------------------------
//! Get content of a nodeas Double_t. In case a node has no content an exception is thrown
//! If there are none or multiple nodes named Name and exception is thrown
//!
//! \param[in] Name node name
//! \param[in] Node node
Double_t TAGxmlParser::GetContentAsDouble(std::string Name,XMLNodePointer_t Node)
{
  std::vector<XMLNodePointer_t> tmp= this->GetChildNodesByName(Node,Name);
  if (tmp.size()!=1)
  {
    throw -1;
  }
  const char* content=fXMLEngine->GetNodeContent(tmp[0]);
  if (content!=0)
  {
    return TString(content).Atof();
  }
  throw -1;
}

//------------------------------------------+-----------------------------------
//! Export to file
//!
//! \param[in] Filename file name
//! \param[in] mainnode nain node
void TAGxmlParser::ExportToFile(std::string Filename,XMLDocPointer_t mainnode)
{
	XMLDocPointer_t xmldoc = fXMLEngine->NewDoc();
	fXMLEngine->DocSetRootElement(xmldoc, mainnode);
	 // Save document to file
	fXMLEngine->SaveDoc(xmldoc, Filename.c_str());
	fXMLEngine->FreeDoc(xmldoc);
}

