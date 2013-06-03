#include "tracer.hpp"
#include "stdafx.h"

#ifdef TRACER

#include "myrimatchSpectrum.h"

#define LITERAL '.'
#define STRUCT_START '['
#define STRUCT_END ']'
#define MAP_START '{'
#define MAP_END '}'

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
	cout << LITERAL << '\t' << x << '\t' << "std::string" << '\t'; esc(x->c_str());
}

template <typename T>
void dump_vector_struct(const T *x) {
	cout << '\t';
	bool tab = false;
	int i = 0;
	for(T::const_iterator it = x->cbegin(); it!=x->cend(); it++) {
		if(tab) {
			cout <<  '\t';
		}
		tab = true;
		cout << i << '\t';
		tracer_dump(&(*it));
		i++;
	}
	cout << '\t' << STRUCT_END;
}

template <typename T>
void dump_vector_ptr_struct(const T *x) {
	cout << '\t';
	bool tab = false;
	int i = 0;
	for(T::const_iterator it = x->cbegin(); it!=x->cend(); it++) {
		if(tab) {
			cout <<  '\t';
		}
		tab = true;
		cout << i << '\t';
		tracer_dump(*it);
		i++;
	}
	cout << '\t' << STRUCT_END;
}


void tracer_dump(const vector<string> *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "std::vector<string>";
	dump_vector_struct(x);
}

template<typename T, typename S>
void dump_map(const map<T, S> *x) {
	typedef map<T, S>::const_iterator iter;
	bool tab = false;
	for(iter it = x->cbegin(); it != x->cend(); it++) {
		if(tab) {
			cout << "\t";
		}
		tab = true;
		tracer_dump(&(it->first));
		cout << "\t";
		tracer_dump(&(it->second));
	}
	cout << '\t' << MAP_END;
}

void tracer_dump(const map<string, string> *x) {
	cout << MAP_START << '\t' << x << '\t' << "std::map<string, string>" << '\t';
	dump_map(x);
}

template <typename T>
void dump_vector_literal(const vector<T> *x) {
	bool tab = false;
	BOOST_FOREACH(T v, *x) {
		if(tab) {
			cout <<  "\\t";
		}
		tab = true;
		cout << v;
	}
}

void tracer_dump(const vector<double> *x) {
	cout << LITERAL << '\t' << x << '\t' << "std::vector<double>" << '\t';
	dump_vector_literal(x);
}

void tracer_dump(const vector<float> *x) {
	cout << LITERAL << '\t' << x << '\t' << "std::vector<float>" << '\t';
	dump_vector_literal(x);
}

void tracer_dump(const vector<int> *x) {
	cout << LITERAL << '\t' << x << '\t' << "std::vector<int>" << '\t';
	dump_vector_literal(x);
}

void tracer_dump(const size_t *x) {
	cout << LITERAL << '\t' << x << '\t' << "size_t" << '\t' << *x;
}

void tracer_dump(const int *x) {
	cout << LITERAL << '\t' << x << '\t' << "int" << '\t' << *x;
}

void tracer_dump(const float *x) {
	cout << LITERAL << '\t' << x << '\t' << "float" << '\t' << *x;
}

void tracer_dump(const double *x) {
	cout << LITERAL << '\t' << x << '\t' << "double" << '\t' << *x;
}

void tracer_dump(const char *x) {
	cout << LITERAL << '\t' << x << '\t' << "char" << '\t' << *x;
}

void tracer_dump(const bool *x) {
	cout << LITERAL << '\t' << x << '\t' << "bool" << '\t' << (*x ?  "true" : "false");
}

void tracer_dump(const IntegerSet *x) {
	cout << LITERAL << '\t' << x << '\t' << "IntegerSet" << '\t' << *x;
}

void tracer_dump(const boost::regex *x) {
	cout << LITERAL << '\t' << x << '\t' << "boost::regex" << '\t' << *x;
}

template <typename T, typename S> 
void dump_pair(const std::pair<T, S> *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "std::pair" << '\t';
	cout << "first" << '\t';
	tracer_dump(&(x->first));
	cout << '\t' << "second" << '\t';
	tracer_dump(&(x->second));
	cout << '\t' << STRUCT_END;
}

void tracer_dump(const freicore::PeakPreData *x) {
	cout << LITERAL << '\t' << x << '\t' << "freicore::PeakPreData" << '\t';
	typedef std::pair<double, float> pairT; 
	bool tab = false;
	BOOST_FOREACH(const pairT & p, *x) {
		if(tab) {
			cout << "\\t";
		}
		tab = true;
		cout << p.first << ", " << p.second;
	}
}

void tracer_dump(const freicore::myrimatch::SpectraList *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "freicore::myrimatch::SpectraList";
	dump_vector_ptr_struct(x);
}

void tracer_dump(const freicore::myrimatch::Spectrum *x) {
    cout << LITERAL << '\t' << x << '\t' << "freicore::myrimatch::Spectrum" << '\t';
    esc(x->nativeID.c_str());
}

void tracer_dump(const freicore::BaseSpectrum *x) {
    cout << LITERAL << '\t' << x << '\t' << "freicore::BaseSpectrum" << '\t';
    esc(x->nativeID.c_str());
}

void tracer_dump(const boost::container::flat_map<double, freicore::myrimatch::PeakInfo> *x) {
    cout << LITERAL << '\t' << x << '\t' << "freicore::myrimatch::Spectrum::PeakData" << '\t';
    bool tab = false;
    for (boost::container::flat_map<double, freicore::myrimatch::PeakInfo>::const_iterator itr = x->cbegin(); itr != x->cend(); ++itr)
    {
        double mz = itr->first;
        float normalizedIntensity = itr->second.normalizedIntensity;
        if(tab) {
            cout << "\\t";
        }
        tab = true;
		cout << mz << ", " << normalizedIntensity;
    }
}

void tracer_dump(const freicore::PrecursorMassHypothesis *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "freicore::PrecursorMassHypothesis";
	cout  << '\t' << "mass" << '\t';
	tracer_dump(&(x->mass));
	cout << '\t' << "charge" << '\t';
	tracer_dump(&(x->charge));
	cout << '\t' << STRUCT_END;
}

void tracer_dump(const vector<freicore::PrecursorMassHypothesis> *x) {
    cout << STRUCT_START << '\t' << x << '\t' << "std::vector<freicore::PrecursorMassHypothesis>" << '\t';
	bool tab = false;
	int i = 0;
	BOOST_FOREACH(const freicore::PrecursorMassHypothesis & m, *x) {
		if(tab) {
			cout <<  "\t";
		}
		tab = true;
		cout << i << '\t';
		tracer_dump(&m);
		i++;
	}
	cout << '\t' << STRUCT_END;
}

void tracer_dump(const pwiz::chemistry::MZTolerance *x) {
	cout << LITERAL << '\t' << x << '\t' << "pwiz::chemistry::MZTolerance" << '\t';
	cout << x->value << (x->units == pwiz::chemistry::MZTolerance::MZ ? "m/z" : "ppm");	
}

void tracer_dump(const freicore::MassType *x) {
	cout << LITERAL << '\t' << x << '\t' << "freicore::MassType" << '\t';
	cout << (*x == freicore::MassType_Monoisotopic ? "monoisotopic" : "average");	
}

void tracer_dump(const freicore::myrimatch::MzToleranceRule *x) {
	cout << LITERAL << '\t' << x << '\t' << "freicore::MzToleranceRule" << '\t';
	switch(*x) {
	case freicore::myrimatch::MzToleranceRule_Auto:
		cout << "auto";
		break;
	case freicore::myrimatch::MzToleranceRule_Mono:
		cout << "mono";
		break;
	case freicore::myrimatch::MzToleranceRule_Avg:
		cout << "avg";
		break;
	default:
		cout << "<unknown>";
	}
}


const char * tracer_id(const freicore::BaseSpectrum *x) {
    return x->nativeID.c_str();
}

const char * tracer_id(const freicore::myrimatch::Spectrum *x) {
    return x->nativeID.c_str();
}

const char * tracer_id(void *ptr) { 
    return "this*"; 
}


#endif // TRACER