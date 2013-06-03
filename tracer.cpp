#include "tracer.hpp"
#include "stdafx.h"

#ifdef TRACER

#include "myrimatchSpectrum.h"

#define LITERAL '.'
#define STRUCT_START '['
#define STRUCT_END ']'
#define MAP_START '{'
#define MAP_END '}'

void esc(const char c) {
		if(c == '\n') {
			cout << "\\n";
		} else if(c == '\r') {
			cout << "\\r";
		} else if(c == '\t') {
			cout << "\\t";
		} else if(c == '\\') {
			cout << "\\\\";
		} else {
			cout << c;
		}
}

void esc(const char *s) {
	const char *ptr = s;
	while(*ptr) {
		esc(*ptr);
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

// Anything that has const iterator
template <typename T>
void dump_iterable(const T *x) {
	int i = 0;
	for(T::const_iterator it = x->cbegin(); it!=x->cend(); it++) {
		cout <<  '\t';
		cout << i << '\t';
		tracer_dump(&(*it));
		i++;
	}
	cout << '\t' << STRUCT_END;
}

// Anything that has const iterator containing pointers to the actual data
template <typename T>
void dump_iterable_ptr(const T *x) {
	int i = 0;
	for(T::const_iterator it = x->cbegin(); it!=x->cend(); it++) {
		cout <<  '\t';
		cout << i << '\t';
		tracer_dump(*it);
		i++;
	}
	cout << '\t' << STRUCT_END;
}

template<typename T, typename S>
void dump_map(const map<T, S> *x) {
	typedef map<T, S>::const_iterator iter;
	for(iter it = x->cbegin(); it != x->cend(); it++) {
		cout << '\t';
		tracer_dump(&(it->first));
		cout << '\t';
		tracer_dump(&(it->second));
	}
	cout << '\t' << MAP_END;
}

template<typename T, typename S>
void dump_map(const boost::container::flat_map<T, S> *x) {
	typedef boost::container::flat_map<T, S>::const_iterator iter;
	for(iter it = x->cbegin(); it != x->cend(); it++) {
		cout << "\t";
		tracer_dump(&(it->first));
		cout << "\t";
		tracer_dump(&(it->second));
	}
	cout << '\t' << MAP_END;
}

void tracer_dump(const vector<string> *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "std::vector<string>";
	dump_iterable(x);
}

void tracer_dump(const map<string, string> *x) {
	cout << MAP_START << '\t' << x << '\t' << "std::map<string, string>";
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
	cout << LITERAL << '\t' << x << '\t' << "std::vector<double>" << '\t' << flush;
	dump_vector_literal(x);
}

void tracer_dump(const vector<float> *x) {
	cout << LITERAL << '\t' << x << '\t' << "std::vector<float>" << '\t' << flush;
	dump_vector_literal(x);
}

void tracer_dump(const vector<int> *x) {
	cout << LITERAL << '\t' << x << '\t' << "std::vector<int>" << '\t' << flush;
	dump_vector_literal(x);
}

#define LIT_H(type) { cout << LITERAL << '\t' << ((void*)x) << '\t' << #type << '\t'; }
#define LIT(type)  { LIT_H(type); cout << *x; }

void tracer_dump(const size_t *x) {
	LIT(size_t);
}

void tracer_dump(const int *x) {
	LIT(int);
}

void tracer_dump(const float *x) {
	LIT(float);
}

void tracer_dump(const double *x) {
	LIT(double);
}

void tracer_dump(const char *x) {
	LIT_H(char);
	esc(*x);
}

void tracer_dump(const bool *x) {
	LIT_H(bool);
	cout << (*x ?  "true" : "false");
}

void tracer_dump(const IntegerSet *x) {
	LIT(IntegerSet);
}

void tracer_dump(const boost::regex *x) {
	LIT(boost::regex);
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
	LIT_H(freicore::PeakPreData);
	typedef std::pair<double, float> pairT; 
	bool tab = false;
	BOOST_FOREACH(const pairT & p, *x) {
		if(tab) {
			cout << "\\t";
		}
		tab = true;
		cout << p.first << ", " << p.second << flush;
	}
}

#define STRUCT_OPEN(type) { cout << STRUCT_START << '\t' << ((void*)x) << '\t' << #type; }
#define STRUCT_MEMBER(name) { cout << '\t' << #name << '\t'; { tracer_dump(&(x->name)); } }
#define STRUCT_CLOSE { cout << '\t' << STRUCT_END; }

void tracer_dump(const freicore::myrimatch::SpectraList *x) {
	STRUCT_OPEN(freicore::myrimatch::SpectraList);
	dump_iterable_ptr(x);
}

void tracer_dump(const freicore::NativeID *x) {
	STRUCT_OPEN(freicore::NativeId);	
	STRUCT_MEMBER(_id);
	STRUCT_CLOSE;
}

void tracer_dump(const freicore::SpectrumId *x) {
	STRUCT_OPEN(freicore::SpectrumId);
	STRUCT_MEMBER(id); 
	STRUCT_MEMBER(nativeID);
	STRUCT_MEMBER(charge);
	STRUCT_CLOSE;
}

void tracer_dump(const pwiz::cv::CVID *x) {
	LIT(pwiz::cv::CVID);
}

template <typename T>
void tracer_dump(const std::set<T> *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "std::set<T>";
	dump_iterable(x);	
}

void tracer_dump(const freicore::ComparisonStatistics *x) {
	STRUCT_OPEN(freicore::ComparisonStatistics);
	STRUCT_MEMBER(numDecoyModComparisons);
	STRUCT_MEMBER(numDecoyUnmodComparisons);
	STRUCT_MEMBER(numTargetModComparisons);
	STRUCT_MEMBER(numTargetUnmodComparisons);
	STRUCT_CLOSE;
}

void tracer_dump(const freicore::BasePeakData<float> *x) {
	LIT_H(freicore::BasePeakData<float>);
    bool tab = false;
    for (freicore::BasePeakData<float>::const_iterator itr = x->cbegin(); itr != x->cend(); ++itr)
    {
        double mz = itr->first;
        float intensity = itr->second;
        if(tab) {
            cout << "\\t";
        }
        tab = true;
		cout << mz << ", " << intensity;
    }
}

void tracer_dump(const freicore::FragmentTypesBitset *x) {
	LIT_H(freicore::FragmentTypesBitset);
	const freicore::FragmentTypesBitset &defaultFragmentTypes = *x;
	cout << (defaultFragmentTypes[FragmentType_A]?string(" a"):string())+(defaultFragmentTypes[FragmentType_B]?string(" b"):string())+(defaultFragmentTypes[FragmentType_C]?string(" c"):string())+(defaultFragmentTypes[FragmentType_X]?string(" x"):string())+(defaultFragmentTypes[FragmentType_Y]?string(" y"):string())+(defaultFragmentTypes[FragmentType_Z]?string(" z"):string())+(defaultFragmentTypes[FragmentType_Z_Radical]?string(" z*"):string());
}

void tracer_dump(const boost::container::flat_map<int, int> *x) {
	cout << MAP_START << '\t' << x << '\t' << "boost::container::flat_map<int, int>";
	dump_map(x);
}

void tracer_dump(const vector<freicore::myrimatch::SearchResult> *x) {
	cout << STRUCT_START << '\t' << x << '\t' << "std::vector<freicore::myrimatch::SearchResult>";
	dump_iterable(x);
}

void tracer_dump(const freicore::myrimatch::SearchResult *x) {
	STRUCT_OPEN(freicore::myrimatch::SearchResult);
	STRUCT_MEMBER(fragmentsMatched);
	STRUCT_MEMBER(fragmentsUnmatched);
	STRUCT_MEMBER(matchedIons);
	STRUCT_MEMBER(mvh);
	STRUCT_MEMBER(mzFidelity);
	STRUCT_MEMBER(precursorMassHypothesis);
	STRUCT_MEMBER(proteins);
	STRUCT_CLOSE;
}

void tracer_dump(const freicore::myrimatch::Spectrum *x) {
	STRUCT_OPEN(freicore::myrimatch::Spectrum);
	STRUCT_MEMBER(nativeID);
	STRUCT_MEMBER(id);
	STRUCT_MEMBER(basePeakMz);
	STRUCT_MEMBER(basePeakIntensity);
	// STRUCT_MEMBER(decoyState);
	STRUCT_MEMBER(detailedCompStats);
	STRUCT_MEMBER(dissociationTypes);
	STRUCT_MEMBER(fileName); 
	STRUCT_MEMBER(filteredPeaks);
	STRUCT_MEMBER(fragmentTypes); 
	STRUCT_MEMBER(intenClassCounts);
	STRUCT_MEMBER(mOfPrecursor);
	STRUCT_MEMBER(mOfUnadjustedPrecursor);
	STRUCT_MEMBER(mvhScoreDistribution); 
	STRUCT_MEMBER(mzFidelityDistribution);
	STRUCT_MEMBER(mzFidelityThresholds); 
	STRUCT_MEMBER(mzLowerBound);
	STRUCT_MEMBER(mzOfPrecursor);
	STRUCT_MEMBER(mzUpperBound);
	STRUCT_MEMBER(newMZFidelityThresholds);
	STRUCT_MEMBER(numDecoyComparisons);
	STRUCT_MEMBER(numFragmentChargeStates);
	STRUCT_MEMBER(numTargetComparisons);
	// STRUCT_MEMBER(numTerminiCleavages);
	STRUCT_MEMBER(peakCount);
	STRUCT_MEMBER(peakData);
	STRUCT_MEMBER(peakPreCount);
	STRUCT_MEMBER(peakPreData);
	STRUCT_MEMBER(possibleChargeStates);
	STRUCT_MEMBER(precursorMassHypotheses);
	STRUCT_MEMBER(precursorMzType);
	STRUCT_MEMBER(processingTime);
	// STRUCT_MEMBER(resultsByCharge);
	STRUCT_MEMBER(retentionTime);
	// STRUCT_MEMBER(scoreHistogram);
	// STRUCT_MEMBER(scores);
	STRUCT_MEMBER(totalIonCurrent);
	STRUCT_MEMBER(totalPeakSpace);

	STRUCT_CLOSE;
}

void tracer_dump(const freicore::BaseSpectrum *x) {
	LIT_H(freicore::BaseSpectrum);
    esc(x->nativeID.c_str());
}

void tracer_dump(const boost::container::flat_map<double, freicore::myrimatch::PeakInfo> *x) {
	LIT_H(freicore::myrimatch::Spectrum::PeakData);
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
    cout << STRUCT_START << '\t' << x << '\t' << "std::vector<freicore::PrecursorMassHypothesis>";
	int i = 0;
	BOOST_FOREACH(const freicore::PrecursorMassHypothesis & m, *x) {
		cout <<  "\t";
		cout << i << '\t';
		tracer_dump(&m);
		i++;
	}
	cout << '\t' << STRUCT_END;
}

void tracer_dump(const pwiz::chemistry::MZTolerance *x) {
	LIT_H(pwiz::chemistry::MZTolerance);
	cout << x->value << (x->units == pwiz::chemistry::MZTolerance::MZ ? "m/z" : "ppm");	
}

void tracer_dump(const freicore::MassType *x) {
	LIT_H(freicore::MassType);
	cout << (*x == freicore::MassType_Monoisotopic ? "monoisotopic" : "average");	
}

void tracer_dump(const freicore::myrimatch::MzToleranceRule *x) {
	LIT_H(freicore::MzToleranceRule);
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