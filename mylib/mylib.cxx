module;
#include <ostream>
#include <stdexcept>

module mylib;

using namespace std;

namespace mylib
{
  void say_hello (ostream& o, const string& n)
  {
    if (n.empty ())
      throw invalid_argument ("empty name");

    o << "Hello, " << n << '!' << endl;
  }
}
