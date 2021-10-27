#include <STEPControl_Reader.hxx>

int main(int argc, char** argv)
{
  // Read from file.
  TopoDS_Shape shape;
  //
  if ( argc > 1 )
  {
    STEPControl_Reader readerBase;
    readerBase.ReadFile(argv[1]);
    std::cout << readerBase.TransferRoots() << "Base roots transferred" << std::endl;
  }
  else
  {
    std::cout << "Please, pass filename (STEP) as an argument." << std::endl;
    return 1;
  }

  return 0;
}
