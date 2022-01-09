#ifndef XMLREADER_H
#define XMLREADER_H
/*!
 \file TAGxmlParser.hxx
 \brief Declaration of TAGxmlParser.
 */
/*------------------------------------------+---------------------------------*/

#include "TXMLEngine.h"
#include "TString.h"


class TAGxmlParser
{
private:
  //! Object to handle xml parsing engine
  TXMLEngine* fXMLEngine;
  //! Pointer to the actual xmlfile
  XMLDocPointer_t fXMLDoc;
   
public:
  //! Create the TXMLEngine
  TAGxmlParser();
  
  //! Desctructor takes care of destroying the fXMLEngine and fXMLDoc
  ~TAGxmlParser();
   
  //! Load xml file
  void ReadFile(std::string FileName);

  //! Show the content of the xml file
  void PrintXmlContent();

  //! Get all the first generation child  of a node (StartingNode) whose name is (NodeName)
  std::vector<XMLNodePointer_t> GetChildNodesByName(
                                XMLNodePointer_t StartingNode,
                                std::string NodeName);

  //! Return the content of a node as Int_t
  Int_t GetContentAsInt(std::string Name,XMLNodePointer_t Node);
  
  //! Return the content of a node as std::string
  std::string GetContentAsString(std::string Name,XMLNodePointer_t Node);

  //! Return the content of a node as a double
  Double_t GetContentAsDouble(std::string Name,XMLNodePointer_t Node);
  
  //! Return the pointer to the document main node
  XMLNodePointer_t GetMainNode() { return fXMLEngine->DocGetRootElement(fXMLDoc);}
   
  //! export to file
  //! Add element
  XMLNodePointer_t AddElement(std::string Name, XMLNodePointer_t ParentNode) { return fXMLEngine->NewChild(ParentNode, 0, Name.c_str(),0);}
  //! Add main node
  XMLNodePointer_t CreateMainNode(std::string Name) { return fXMLEngine->NewChild(0, 0, Name.c_str());}
  //! Add element with content
  void AddElementWithContent(std::string Name, XMLNodePointer_t ParentNode, std::string Content) { fXMLEngine->NewChild(ParentNode, 0,Name.c_str(),Content.c_str());}
  //! Export to file
  void ExportToFile(std::string Filename,XMLDocPointer_t mainnode);

};
#endif

