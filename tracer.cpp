#include "tracer.hpp"

#ifdef TRACER

void tracer_hdr(const char * op) {
	cout << "[TRACER]\t" << op << "\t";
}

void tracer_ftr() {
	cout << "\n";
}

void tracer_hdr(const char * op, const void * obj) {
	tracer_hdr(op);
	cout << obj << "\t";
}

void tracer_label(const void *x, const char* name) {	
	tracer_hdr("label", x);
	cout << name;
	tracer_ftr();
}

void tracer_unlink(const void *x) {
	tracer_hdr("unlink", x);
	tracer_ftr();
}

void tracer_dump_hdr(const void *object, const char* type) {
	tracer_hdr("dump", object);
	cout << type << "\t";
}

void tracer_dump(const string& x) {
	tracer_dump_hdr(&x, "std::string");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const vector<string>& x) {
	tracer_dump_hdr(&x, "std::vector<std::string>");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const int &x) {
	tracer_dump_hdr(&x, "int");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const float &x) {
	tracer_dump_hdr(&x, "float");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const double &x) {
	tracer_dump_hdr(&x, "double");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const char &x) {
	tracer_dump_hdr(&x, "char");
	cout << x;
	tracer_ftr();
}

void tracer_op_start(const char *name) {
	tracer_hdr("op_start");
	cout << name;
	tracer_ftr();
}

void tracer_op_input(const void *x) {
	tracer_hdr("op_in");
	cout << x;
	tracer_ftr();
}

void tracer_op_output(const void *x) {
	tracer_hdr("op_out");
	cout << x;
	tracer_ftr();
}

void tracer_op_end(const char *name) {
	tracer_hdr("op_end");
	cout << name;
	tracer_ftr();
}


#endif // TRACER