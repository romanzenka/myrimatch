#ifndef _TRACER_H
#define _TRACER_H

#include <string>
#include <map>
#include <vector>
#include <boost/interprocess/containers/container/flat_map.hpp>

#include <boost/regex.hpp>

using std::string;
using std::map;
using std::vector;
using std::set;

#define TRACER_ENABLED 1
#define HEAP 1
#define STACK 2

#define READ 1
#define WRITE 2
#define NOTE 3

#ifdef TRACER_ENABLED

namespace pwiz {
	namespace chemistry {
		struct MZTolerance;
	}
	namespace util { 
		class IntegerSet;
	}
	namespace proteome {
		class Peptide;
		class Digestion;
		class Modification;
		class ModificationMap;
		class DigestedPeptide;
	}
}

namespace freicore {
    class PeakPreData;
    struct BaseSpectrum;
	struct PrecursorMassHypothesis;
	enum MassType;
	struct FragmentTypesBitset;
    namespace myrimatch {
        struct Spectrum;
        struct PeakInfo;
        struct SpectraList;
		enum MzToleranceRule;
		struct SearchResult;
		struct RunTimeConfig;
    }
}

typedef boost::container::flat_map<double, freicore::myrimatch::PeakInfo> PeakSpectrumData;

template <typename T>
void tracer_dump(const boost::shared_ptr<T> *x) {
	tracer_dump(x->get());
}

template <size_t N>
void tracer_dump(const char (*x)[N]) {
	tracer_dump((const char*)(&(*x[0])));
}

void tracer_dump(const string *x);
void tracer_dump(const vector<string> *x);
void tracer_dump(const map<string, string> *x);
void tracer_dump(const vector<double> *x);
void tracer_dump(const vector<float> *x);
void tracer_dump(const vector<int> *x);
void tracer_dump(const std::vector<pwiz::chemistry::MZTolerance> *x);
void tracer_dump(const vector<freicore::PrecursorMassHypothesis> *x);

void tracer_dump(const size_t *x);
void tracer_dump(const int *x);
void tracer_dump(const float *x);
void tracer_dump(const double *x);
void tracer_dump(const char *x);
void tracer_dump(const bool *x);
void tracer_dump(const pwiz::util::IntegerSet *x);
void tracer_dump(const boost::regex *x);
void tracer_dump(const freicore::PeakPreData *x);
void tracer_dump(const freicore::myrimatch::SpectraList *x);
void tracer_dump(const freicore::myrimatch::Spectrum *x);
void tracer_dump(const freicore::BaseSpectrum *x);
void tracer_dump(const freicore::PrecursorMassHypothesis *x);
void tracer_dump(const pwiz::chemistry::MZTolerance *x);
void tracer_dump(const freicore::MassType * x);
void tracer_dump(const freicore::myrimatch::MzToleranceRule * x);
void tracer_dump(const freicore::FragmentTypesBitset *x);
void tracer_dump(const freicore::myrimatch::SearchResult *x);
void tracer_dump(const freicore::myrimatch::RunTimeConfig *x);
void tracer_dump(const pwiz::proteome::Peptide *x);
void tracer_dump(const pwiz::proteome::DigestedPeptide *x);

void tracer_dump(const PeakSpectrumData * x);

const char * tracer_id(const freicore::myrimatch::Spectrum * x);
const char * tracer_id(const freicore::BaseSpectrum * x);
const char * tracer_id(const void * const ptr);

#define TRACER(variable, operation, heap, note) { cout << flush << "[TRACER]" << '\t' << "dump" << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\t'; tracer_dump(&(variable)); cout << flush << '\n'; }
#define TRACER_P(variable, type, representation, operation, heap, note) { cout << flush << "[TRACER]" << '\t' << "dump" << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\t' << '.' << '\t' << &(variable) << '\t' << type << '\t' << representation << flush << '\n'; }
// Reference a variable in an operation, do not dump all its info
#define TRACER_REF(variable, operation, heap, note) { cout << flush << "[TRACER]" << '\t' << "ref" << '\t' << &(variable) << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << flush << '\n'; }
#define TRACER_UNLINK(variable) { cout << flush << "[TRACER]" << '\t' << "unlink" << '\t' << &(variable) << '\t' << #variable << '\t' __FILE__ << '\t' << __LINE__ << flush << '\n'; }

// Start operation of a given name
#define TRACER_OP_START(name) { cout << flush << "[TRACER]" << '\t' << "op_start" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << flush << '\n'; }
// End operation of a given name
#define TRACER_OP_END(name) { cout << flush << "[TRACER]" << '\t' <<  "op_end" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << flush << '\n'; }

#define TRACER_METHOD_START(name) { TRACER_BI; TRACER_OP_START(name); TRACER(*this, READ, HEAP, tracer_id(this)); }
#define TRACER_METHOD_END(name) { TRACER(*this, WRITE, HEAP, tracer_id(this)); TRACER_OP_END(name); TRACER_BO; }

#define TRACER_S2S(variable) lexical_cast<string>(variable)
// Defines scope of stack variables
#define TRACER_BI { cout << flush << "[TRACER]" << '\t' << "block_in" << '\n'; }
#define TRACER_BO { cout << flush << "[TRACER]" << '\t' << "block_out" << '\n'; }

#else // !TRACER_ENABLED

#define TRACER(variable, operation, heap, note)
#define TRACER_R(variable, type, representation, operation, heap, note)
#define TRACER_UNLINK(variable)
#define TRACER_OP_START(name)
#define TRACER_OP_END(name)
#define TRACER_S2S(variable)
#define TRACER_BI
#define TRACER_BO

#endif // TRACER_ENABLED


#endif