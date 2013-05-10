#ifndef _TRACER_H
#define _TRACER_H

#include "stdafx.h"

#define TRACER 1

#ifdef TRACER

void tracer_label(const void *x, const char* name);
void tracer_var(const void *x, const char* name, const char *file, int linenum);
void tracer_unlink(const void *x);
void tracer_start(const void *x);

void tracer_dump(const string& x);
void tracer_dump(const vector<string>& x);
void tracer_dump(const int &x);
void tracer_dump(const float &x);
void tracer_dump(const double &x);
void tracer_dump(const char &x);
void tracer_dump(const IntegerSet &x);
void tracer_dump(const boost::regex &x);
void tracer_dump(const void *x, const char * type, const char * representation);
void tracer_dump(const void *x, const char * type, const string &representation);

void tracer_reason(const void *x, const char * reason);

void tracer_op_start(const char *name, const char *file, int linenum);
void tracer_op_input(const void *x);
void tracer_op_output(const void *x);
void tracer_op_end(const char *name, const char *file, int linenum);

// Give a name to a memory address
#define TRACER_LABEL(x, name) tracer_label(&(x), name)
// Unlink the label before the memory address ceases to exist
#define TRACER_UNLINK(x) tracer_unlink(&(x))
// Mark end of a 'logical lifetime' for a particular object
#define TRACER_START(x) tracer_start(&(x))

// Dump contents of a particular object
#define TRACER_DUMP(x) tracer_dump(x)

// Dump contents of a particular object with a specified string representation
#define TRACER_DUMP_R(x, type, representation) tracer_dump(&(x), type, representation)

// Why has the object value changed here?
#define TRACER_REASON(x, reason) tracer_reason(&(x), reason);

// Start operation of a given name
#define TRACER_OP_START(name) { tracer_op_start(name, __FILE__, __LINE__); }
// The operation uses a particular input
#define TRACER_OP_INPUT(x) { tracer_var(&(x), #x, __FILE__, __LINE__); tracer_op_input(&(x)); }
#define TRACER_OP_IN_DUMP(x) { TRACER_OP_INPUT(x); TRACER_DUMP(x); }
#define TRACER_OP_IN_DUMP_R(x, type, representation) { TRACER_OP_INPUT(x); TRACER_DUMP_R(x, type, representation); }
// The operation produces a particular output
#define TRACER_OP_OUTPUT(x) { tracer_var(&(x), #x, __FILE__, __LINE__); tracer_op_output(&(x)); }
#define TRACER_OP_OUT_DUMP(x) { TRACER_OP_OUTPUT(x); TRACER_DUMP(x); }
#define TRACER_OP_OUT_DUMP_R(x, type, representation) { TRACER_OP_OUTPUT(x); TRACER_DUMP_R(x, type, representation); }
// End operation of a given name
#define TRACER_OP_END(name) tracer_op_end(name, __FILE__, __LINE__)

#else

#define TRACER_LABEL(x, name)
#define TRACER_UNLINK(x)
#define TRACER_DUMP(x)
#define TRACER_DUMP_R(x, type, representation)
#define TRACER_REASON(x, reason)
#define TRACER_OP_START(name)
#define TRACER_OP_INPUT(x)
#define TRACER_OP_OUTPUT(x)
#define TRACER_OP_OUT_DUMP(x, name)
#define TRACER_OP_OUT_DUMP_R(x, name, type, representation)
#define TRACER_OP_END(name)
#endif


#endif 
