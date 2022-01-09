#ifndef _TAGview_HXX
#define _TAGview_HXX
/*!
  \file TAGview.hxx
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

    //! Add observable
    void            AddObservable(TObject* p_obj, const char* baseclass=0);
    //! Remove observable
    void            RemoveObservable(TObject* p_obj);
    //! Replace observable
    void            ReplaceObservable(TObject* p_old, TObject* p_new, 
				      const char* baseclass=0);

    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    //! Draw
    virtual void    Draw(Option_t* option="");
    //! Recursive remove
    virtual void    RecursiveRemove(TObject* p_obj);

  private:
    TVirtualPad*    fpPad;            ///< Pad
    TList*          fpObservableList; ///< List of observables
   
   ClassDef(TAGview,0)
};

#endif
