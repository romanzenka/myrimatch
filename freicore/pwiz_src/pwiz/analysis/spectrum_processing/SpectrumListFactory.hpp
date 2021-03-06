//
// $Id: SpectrumListFactory.hpp 1191 2009-08-14 19:33:05Z chambm $
//
//
// Original author: Darren Kessner <darren@proteowizard.org>
//
// Copyright 2008 Spielberg Family Center for Applied Proteomics
//   Cedars-Sinai Medical Center, Los Angeles, California  90048
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
//


#ifndef _SPECTRUMLISTFACTORY_HPP_ 
#define _SPECTRUMLISTFACTORY_HPP_ 


#include "pwiz/data/msdata/MSData.hpp"
#include "pwiz/data/msdata/examples.hpp"
#include <string>
#include <vector>


namespace pwiz {
namespace analysis {


/// Factory for instantiating and wrapping SpectrumLists
class PWIZ_API_DECL SpectrumListFactory
{
    public:

    /// instantiate the SpectrumListWrapper indicated by wrapper
    static void wrap(msdata::MSData& msd, const std::string& wrapper);

    /// instantiate a list of SpectrumListWrappers
    static void wrap(msdata::MSData& msd, const std::vector<std::string>& wrappers);

    /// user-friendly documentation
    static std::string usage();
};


} // namespace analysis 
} // namespace pwiz


#endif // _SPECTRUMLISTFACTORY_HPP_ 

