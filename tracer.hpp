#ifndef _TRACER_H
#define _TRACER_H

#include "stdafx.h"

#define TRACER 1

#ifdef TRACER

void tracer_label(const void *x, const char* name);
void tracer_unlink(const void *x);

void tracer_dump(const string& x);
void tracer_dump(const vector<string>& x);
void tracer_dump(const int &x);
void tracer_dump(const float &x);
void tracer_dump(const double &x);
void tracer_dump(const char &x);

void tracer_op_start(const char *name);
void tracer_op_input(const void *x);
void tracer_op_output(const void *x);
void tracer_op_end(const char *name);

// Give a name to a memory address
#define TRACER_LABEL(x, name) tracer_label(&(x), name);
// Unlink the label once the memory address ceases to exist
#define TRACER_UNLINK(x) tracer_unlink(&(x));

// Dump contents of a particular object
#define TRACER_DUMP(x) tracer_dump(x);

// Start operation of a given name
#define TRACER_OP_START(name) tracer_op_start(name);
// The operation uses a particular input
#define TRACER_OP_INPUT(x) tracer_op_input(&(x));
// The operation produces a particular output
#define TRACER_OP_OUTPUT(x) tracer_op_output(&(x));
// End operation of a given name
#define TRACER_OP_END(name) tracer_op_end(name);

#else

#define TRACER(x)

#endif


#endif 
