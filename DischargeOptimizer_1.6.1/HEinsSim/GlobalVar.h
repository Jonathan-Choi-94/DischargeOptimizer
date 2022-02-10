#ifndef __GLOVAL_H__
#define __GLOVAL_H__
#include <limits>

#define NOMINMAX

#define INFINITY ( std::numeric_limits<double>::has_infinity ? std::numeric_limits<double>::infinity() : 1.7976931348623158e+308  )
const unsigned int kUnExistPort = std::numeric_limits<int>::max();


enum PortType { kInPort, kOutPort };
enum ModelType { kCoupled, kDevsAtomic, kEoAtomic, kPoAtomic, kCsAtomic };


#endif