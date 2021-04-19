#ifndef _TAGview_HXX
#define _TAGview_HXX
/*!
  \file
  \version $Id: TAGview.hxx,v 1.4 2003/06/09 18:43:10 mueller Exp $
  \brief   Declaration of TAGview.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGnamed.hxx"

class TList;
class TVirtualPad;

class TAGview : public TAGnamed {
  public:

    explicit        TAGview(TObject* p_obj=0);
    virtual         ~TAGview();

    void            AddObservable(TObject* p_obj, const char* baseclass=0);
    void            RemoveObservable(TObject* p_obj);
    void            ReplaceObservable(TObject* p_old, TObject* p_new, 
				      const char* baseclass=0);

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    virtual void    Draw(Option_t* option="");
    virtual void    RecursiveRemove(TObject* p_obj);

    ClassDef(TAGview,0)

  private:
    TVirtualPad*    fpPad;
    TList*          fpObservableList;
      
};

#endif
