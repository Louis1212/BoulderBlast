#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;
int main()
{
  ostringstream oss;
  oss << "level";
  unsigned k = 8;
  oss.fill('0');
  oss << setw(2) << k;
  oss << ".bat";
  string s = oss.str();
  cout << s << endl;
}
