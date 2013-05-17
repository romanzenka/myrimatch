#include "tracer.hpp"
#include "stdafx.h"

#ifdef TRACER

#include "myrimatchSpectrum.h"

void esc(const char *s) {
	const char *ptr = s;
	while(*ptr) {
		if(*ptr == '\n') {
			cout << "\\n";
		} else if(*ptr == '\r') {
			cout << "\\r";
		} else if(*ptr == '\t') {
			cout << "\\t";
		} else if(*ptr == '\\') {
			cout << "\\\\";
		} else {
			cout << *ptr;
		}
		ptr++;
	}
}

// Escape for strings where tab is meaningful (separates elements in array). Escape the tab as <tab>
void esc_tab(const char *s) {
	const char *ptr = s;
	while(*ptr) {
		if(*ptr == '\n') {
			cout << "\\n";
		} else if(*ptr == '\r') {
			cout << "\\r";
		} else if(*ptr == '\t') {
			cout << "<tab>";
		} else if(*ptr == '\\') {
			cout << "\\\\";
		} else {
			cout << *ptr;
		}
		ptr++;
	}
}

void tracer_dump(const string *x) {
	cout << x << '\t' << "std::string" << '\t'; esc(x->c_str());
}

void tracer_dump(const vector<string> *x) {
	cout << x << '\t' << "std::vector<string>" << '\t';
	bool tab = false;
	BOOST_FOREACH(string s, *x) {
		if(tab) {
			cout <<  "\\t";
		}
		tab = true;
		esc_tab(s.c_str());
	}
}

void tracer_dump(const map<string, string> *x) {
	cout << x << '\t' << "std::map<string, string>" << '\t';
	std::pair<string, string> p;
	bool tab = false;
	BOOST_FOREACH(p, *x) {
		if(tab) {
			cout << "\\t";
		}
		tab = true;
		esc_tab(p.first.c_str());
		cout << " ==> ";
		esc_tab(p.second.c_str());
	}
}

void tracer_dump(const vector<double> *x) {
	cout << x << '\t' << "std::vector<double>" << '\t';
	bool tab = false;
	BOOST_FOREACH(double v, *x) {
		if(tab) {
			cout <<  "\\t";
		}
		tab = true;
		cout << v;
	}
}

void tracer_dump(const vector<float> *x) {
	cout << x << '\t' << "std::vector<float>" << '\t';
	bool tab = false;
	BOOST_FOREACH(float v, *x) {
		if(tab) {
			cout <<  "\\t";
		}
		tab = true;
		cout << v;
	}
}

void tracer_dump(const int *x) {
	cout << x << '\t' << "int" << '\t' << *x;
}

void tracer_dump(const float *x) {
	cout << x << '\t' << "float" << '\t' << *x;
}

void tracer_dump(const double *x) {
	cout << x << '\t' << "double" << '\t' << *x;
}

void tracer_dump(const char *x) {
	cout << x << '\t' << "char" << '\t' << *x;
}

void tracer_dump(const bool *x) {
	cout << x << '\t' << "bool" << '\t' << (*x ?  "true" : "false");
}

void tracer_dump(const IntegerSet *x) {
	cout << x << '\t' << "IntegerSet" << '\t' << *x;
}

void tracer_dump(const boost::regex *x) {
	cout << x << '\t' << "boost::regex" << '\t' << *x;
}

void tracer_dump(const freicore::PeakPreData *x) {
	cout << x << '\t' << "freicore::PeakPreData" << '\t';
	std::pair<double, float> p;
	bool tab = false;
	BOOST_FOREACH(p, *x) {
		if(tab) {
			cout << "\\t";
		}
		tab = true;
        cout << p.first << ", " << p.second;
	}
}

void tracer_dump(const freicore::myrimatch::SpectraList *x) {
	cout << x << '\t' << "SpectraList" << '\t' << "List of " << x->size() << " spectra";
}

#endif // TRACER