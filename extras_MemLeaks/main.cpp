#include <Geom_Line.hxx>

class MyCurve : public Standard_Transient
{
  DEFINE_STANDARD_RTTI_INLINE(MyCurve, Standard_Transient)

public:

  MyCurve()
  {
    std::cout << "Ctor" << std::endl;
  }

  ~MyCurve()
  {
    std::cout << "Dtor" << std::endl;
  }

};

void Foo(const Handle(MyCurve)& c)
{
  std::cout << c->DynamicType()->Name() << std::endl;
}

int main(int argc, char *argv[])
{
  //Handle(MyCurve) curve = new MyCurve;
  // curve is alive
  Foo(curve);

  auto curve = new MyCurve;

  return 0;
}
