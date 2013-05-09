// This file was generated by the "svnrev" utility
// You should not modify it manually, as it may be re-generated.
//
// $Revision: 138 $
// $Date: 2012-12-1 $
//

#include "myrimatchVersion.hpp"
#include <sstream>

#ifdef PWIZ_USER_VERSION_INFO_H // in case you need to add any info version of your own
#include PWIZ_USER_VERSION_INFO_H  // must define PWIZ_USER_VERSION_INFO_H_STR for use below
#endif

namespace freicore {
namespace myrimatch {


int Version::Major()                {return 2;}
int Version::Minor()                {return 1;}
int Version::Revision()             {return 138;}
std::string Version::LastModified() {return "2012-12-1";}
std::string Version::str()
{
	std::ostringstream v;
	v << Major() << '.' << Minor() << '.' << Revision();
#ifdef PWIZ_USER_VERSION_INFO_H
	v << " (" << PWIZ_USER_VERSION_INFO_H_STR << ")";
#endif
	return v.str();
}


} // namespace myrimatch
} // namespace freicore