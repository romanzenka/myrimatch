#ifndef _TRACER_H
#define _TRACER_H

#include "stdafx.h"

#define TRACER_ENABLED 1
#define HEAP 1
#define STACK 2

#define READ 1
#define WRITE 2
#define NOTE 3

#ifdef TRACER_ENABLED

void tracer_dump(const string& x);
void tracer_dump(const vector<string>& x);
void tracer_dump(const map<string, string>& x);
void tracer_dump(const vector<double>& x);
void tracer_dump(const int &x);
void tracer_dump(const float &x);
void tracer_dump(const double &x);
void tracer_dump(const char &x);
void tracer_dump(const IntegerSet &x);
void tracer_dump(const boost::regex &x);

namespace freicore {
namespace myrimatch {
struct SpectraList;
}
}
void tracer_dump(const freicore::myrimatch::SpectraList &x);

#define TRACER(variable, operation, heap, note) { cout << "[TRACER]" << '\t' << "dump" << '\t'; tracer_dump(variable); cout << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
#define TRACER_P(variable, type, representation, operation, heap, note) { cout << "[TRACER]" << '\t' << "dump" << '\t' << &(variable) << '\t' << type << '\t' << representation << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
// Reference a variable in an operation, do not dump all its info
#define TRACER_REF(variable, operation, heap, note) { cout << "[TRACER]" << '\t' << "ref" << '\t' << &(variable) << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
#define TRACER_UNLINK(variable) { cout << "[TRACER]" << '\t' << "unlink" << '\t' << &(variable) << '\t' << #variable << '\t' __FILE__ << '\t' << __LINE__ << '\n'; }

// Start operation of a given name
#define TRACER_OP_START(name) { cout << "[TRACER]" << '\t' << "op_start" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
// End operation of a given name
#define TRACER_OP_END(name) { cout << "[TRACER]" << '\t' << "op_end" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
#define TRACER_S2S(variable) lexical_cast<string>(variable)

#else // !TRACER_ENABLED

#define TRACER(variable, operation, heap, note)
#define TRACER_R(variable, type, representation, operation, heap, note)
#define TRACER_UNLINK(variable)
#define TRACER_OP_START(name)
#define TRACER_OP_END(name)
#define TRACER_S2S(variable)

#endif // TRACER_ENABLED


#endif 

