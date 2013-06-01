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

namespace pwiz { namespace util { class IntegerSet; } }

#define TRACER_ENABLED 1
#define HEAP 1
#define STACK 2

#define READ 1
#define WRITE 2
#define NOTE 3

#ifdef TRACER_ENABLED

namespace freicore {
    class PeakPreData;
    struct BaseSpectrum;
    namespace myrimatch {
        struct Spectrum;
        struct PeakInfo;
        struct SpectraList;
    }
}

typedef boost::container::flat_map<double, freicore::myrimatch::PeakInfo> PeakSpectrumData;

void tracer_dump(const string *x);
void tracer_dump(const vector<string> *x);
void tracer_dump(const map<string, string> *x);
void tracer_dump(const vector<double> *x);
void tracer_dump(const vector<float> *x);
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

void tracer_dump(const PeakSpectrumData *x);

const char * tracer_id(const freicore::myrimatch::Spectrum *x);
const char * tracer_id(const freicore::BaseSpectrum *x);
const char * tracer_id(void *ptr);

#define TRACER(variable, operation, heap, note) { cout << "[TRACER]" << '\t' << "dump" << '\t'; tracer_dump(&(variable)); cout << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
#define TRACER_P(variable, type, representation, operation, heap, note) { cout << "[TRACER]" << '\t' << "dump" << '\t' << &(variable) << '\t' << type << '\t' << representation << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
// Reference a variable in an operation, do not dump all its info
#define TRACER_REF(variable, operation, heap, note) { cout << "[TRACER]" << '\t' << "ref" << '\t' << &(variable) << '\t' << #variable << '\t' << heap << '\t' << operation << '\t' << note << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
#define TRACER_UNLINK(variable) { cout << "[TRACER]" << '\t' << "unlink" << '\t' << &(variable) << '\t' << #variable << '\t' __FILE__ << '\t' << __LINE__ << '\n'; }

// Start operation of a given name
#define TRACER_OP_START(name) { cout << "[TRACER]" << '\t' << "op_start" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }
// End operation of a given name
#define TRACER_OP_END(name) { cout << "[TRACER]" << '\t' <<  "op_end" << '\t' << name << '\t' << __FILE__ << '\t' << __LINE__ << '\n'; }

#define TRACER_METHOD_START(name) TRACER_BI; TRACER_OP_START(name); TRACER(*this, READ, HEAP, tracer_id(this));
#define TRACER_METHOD_END(name) TRACER_OP_END(name); TRACER_BO;

#define TRACER_S2S(variable) lexical_cast<string>(variable)
// Defines scope of stack variables
#define TRACER_BI { cout << "[TRACER]" << '\t' << "block_in" << '\n'; }
#define TRACER_BO { cout << "[TRACER]" << '\t' << "block_out" << '\n'; }

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