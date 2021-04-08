#include <TColStd_PackedMapOfInteger.hxx>

// 5 lower bits in a 32-bit word:
// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1
static const unsigned int MASK_LOW = 0x001f;

// 27 upper bits in a 32-bit word:
// 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0
static const unsigned int MASK_HIGH = ~MASK_LOW;

void printBits(unsigned int num,
               const char* decorator = 0)
{
  unsigned int size = sizeof(unsigned int);
  unsigned int maxPow = 1<<(size*8-1);

  if ( decorator )
    std::cout << decorator << " // ";

  int i=0,j;
  for(;i<size*8;++i)
  {
    // print last bit and shift left.
    printf("%u ",num&maxPow ? 1 : 0);
    num = num<<1;
  }
  std::cout << "\n";
}

struct intBlock
{
  //! 32 integers in one integer.
  unsigned int myData;

  unsigned int myMask;

  intBlock() : myData(0), myMask(0) {}

  //! Return TRUE if the given integer key is set within this packed node.
  int HasValue(const int val) const
  {
    int res = (myData & (1 << (val & MASK_LOW)));

    printBits(res, "HasValue()");
    return res;
  }

  //! Add integer key to this packed node.
  //! @return TRUE if key has been added
  Standard_Boolean AddValue (Standard_Integer theValue)
  {
    const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
    printBits(aValInt, "AddValue() aValInt");
    if ((myData & aValInt) == 0)
    {
      myData ^= aValInt;
      ++myMask;

      printBits(myData, "AddValue() myData");
      printBits(myMask, "AddValue() myMask");

      return Standard_True;
    }

    return Standard_False;
  }

  //! Delete integer key from this packed node.
  //! @return TRUE if key has been deleted
  Standard_Boolean DelValue (Standard_Integer theValue)
  {
    const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
    if ((myData & aValInt) != 0)
    {
      myData ^= aValInt;
      myMask--;
      return Standard_True;
    }
    return Standard_False;
  }
};

int main(int argc, char** argv)
{
  printBits(MASK_LOW,  "MASK_LOW ");
  printBits(MASK_HIGH, "MASK_HIGH");

  intBlock block;
  block.AddValue(0);
  if ( block.HasValue(0) )
    std::cout << "has value\n";
  block.AddValue(1);
  if ( block.HasValue(1) )
    std::cout << "has value\n";
  block.AddValue(2);
  if ( block.HasValue(2) )
    std::cout << "has value\n";
  block.AddValue(3);
  if ( block.HasValue(3) )
    std::cout << "has value\n";
  block.AddValue(10);
  if ( block.HasValue(10) )
    std::cout << "has value\n";
  block.AddValue(15);
  if ( block.HasValue(15) )
    std::cout << "has value\n";
  block.AddValue(101);
  if ( block.HasValue(101) )
    std::cout << "has value\n";

  TColStd_PackedMapOfInteger map;
  map.Add(0); // ???
  map.Add(1);
  map.Add(2);
  map.Add(3);
  map.Add(5);
  map.Add(7);
  map.Add(12);
  map.Add(24);
  map.Add(50);
  map.Add(101);

  // More reading:
  // https://learn.sparkfun.com/tutorials/hexadecimal/all

  /*
   The data are stored in 64 bits as:
    - bits  0 - 4 : (number of integers stored in the block) - 1;
    - bits  5 - 31: base address of the block of integers (low bits assumed 0)
    - bits 32 - 63: 32-bit field where each bit indicates the presence of the corresponding integer in the block.
                    Number of non-zero bits must be equal to the number expressed in bits 0-4.
  */
  return 0;
}
