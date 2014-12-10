//===========================================================================
// Copyright © 2003 Ivan Johansen
// VclObject.h
//===========================================================================
#ifndef VclObjectH
#define VclObjectH

template<typename T> class TVclObject
{
  T *const Object;
public:
  TVclObject() : Object(new T) {}
  TVclObject(T *O) : Object(new T) {if(O != NULL) Object->Assign(O);}
  TVclObject(const TVclObject &V) : Object(new T) {Object->Assign(V.Object);}
  ~TVclObject()   {delete Object;}
  const TVclObject& operator=(const TVclObject<T> &V) {Object->Assign(V.Object); return *this;}
  operator T*() const {return Object;}
  T* operator->() {return Object;}
  T* Get() const {return Object;}
};

template<typename T> class TVclControl
{
  T *const Object;
public:
  TVclControl(TComponent *Owner) : Object(new T(Owner)) {}
//  TVclControl(const TVclControl &V) : Object(new T(V.Object->Owner)) {Object->Assign(V.Object);}
  ~TVclControl()   {delete Object;}
//  const TVclControl& operator=(const TVclControl<T> &V) {Object->Assign(V.Object); return *this;}
  operator T*() const {return Object;}
  T* operator->() {return Object;}
};
#endif
