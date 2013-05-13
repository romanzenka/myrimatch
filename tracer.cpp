#include "tracer.hpp"

#ifdef TRACER

#include "myrimatchSpectrum.h"

void tracer_dump(const string& x) {
	cout << &x << '\t' << "std::string" << '\t' << x;
}

void tracer_dump(const vector<string>& x) {
	cout << &x << '\t' << "std::vector<string>" << '\t' << x;
}

void tracer_dump(const map<string, string>& x) {
	cout << &x << '\t' << "std::map<string, string>" << '\t' << x;
}

void tracer_dump(const vector<double>& x) {
	cout << &x << '\t' << "std::vector<double>" << '\t' << x;
}

void tracer_dump(const int &x) {
	cout << &x << '\t' << "int" << '\t' << x;
}

void tracer_dump(const float &x) {
	cout << &x << '\t' << "float" << '\t' << x;
}

void tracer_dump(const double &x) {
	cout << &x << '\t' << "double" << '\t' << x;
}

void tracer_dump(const char &x) {
	cout << &x << '\t' << "char" << '\t' << x;
}

void tracer_dump(const IntegerSet &x) {
	cout << &x << '\t' << "IntegerSet" << '\t' << x;
}

void tracer_dump(const boost::regex &x) {
	cout << &x << '\t' << "boost::regex" << '\t' << x;
}

void tracer_dump(const freicore::myrimatch::SpectraList &x) {
	cout << &x << '\t' << "SpectraList" << '\t' << "List of " << x.size() << " spectra";
}

#endif // TRACER