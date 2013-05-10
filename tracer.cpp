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

void tracer_var(const void *x, const char* name, const char *file, int linenum) {
	tracer_hdr("var", x);
	cout << name << "\t" << file << "\t" << linenum;
	tracer_ftr();
}

void tracer_unlink(const void *x) {
	tracer_hdr("unlink", x);
	tracer_ftr();
}

void tracer_start(const void *x) {
	tracer_hdr("start", x);
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

void tracer_dump(const IntegerSet &x) {
	tracer_dump_hdr(&x, "IntegerSet");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const boost::regex &x) {
	tracer_dump_hdr(&x, "boost::regex");
	cout << x;
	tracer_ftr();
}

void tracer_dump(const void *x, const char *type, const char * representation) {
	tracer_dump_hdr(x, type);
	cout << representation;
	tracer_ftr();
}

void tracer_dump(const void *x, const char * type, const string &representation) {
	tracer_dump_hdr(x, type);
	cout << representation;
	tracer_ftr();
}

void tracer_reason(const void *x, const char * reason) {
	tracer_hdr("reason", x);
	cout << reason;
	tracer_ftr();
}

void tracer_op_start(const char *name, const char *file, int linenum) {
	tracer_hdr("op_start");
	cout << name << "\t" << file << "\t" << linenum;
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

void tracer_op_end(const char *name, const char *file, int linenum) {
	tracer_hdr("op_end");
	cout << name << "\t" << file << "\t" << linenum;
	tracer_ftr();
}


#endif // TRACER