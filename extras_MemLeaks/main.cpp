//-----------------------------------------------------------------------------
// Copyright (c) 2022-present, Quaoar Studio (ask@quaoar.pro)
//----------------------------------------------------------------------------

// OpenCascade includes
#include <Standard_Type.hxx>

// Local includes
#include "MemTracker.h"

#include "windows.h"

class MyEntity : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(MyEntity, Standard_Transient)

  DEFINE_STANDARD_ALLOC

public:

  MyEntity()
  {
    std::cout << "ctor" << std::endl;
  }

  ~MyEntity()
  {
    std::cout << "dtor" << std::endl;
  }

private:

  int num[1024];
};

int main(int argc, char *argv[])
{
  const int numIter = 10000;

  for ( int i = 0; i < numIter; ++i )
  {
    Handle(MyEntity) ent = new MyEntity;
    MEMCHECK
    //Sleep(1);
  }

  MEMCHECK_DUMP("C:/users/serge/desktop/mem.txt")

  return 0;
}
