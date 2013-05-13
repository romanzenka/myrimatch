//
// $Id: myrimatchConfig.h 135 2012-11-29 20:35:34Z chambm $
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
// The Original Code is the MyriMatch search engine.
//
// The Initial Developer of the Original Code is Matt Chambers.
//
// Copyright 2009 Vanderbilt University
//
// Contributor(s): Surendra Dasari
//

#ifndef _MYRIMATCHCONFIG_H
#define _MYRIMATCHCONFIG_H

#include "stdafx.h"
#include "freicore.h"
#include "BaseRunTimeConfig.h"
#include "pwiz/data/identdata/IdentDataFile.hpp"


using namespace freicore;


#define MYRIMATCH_RUNTIME_CONFIG \
    COMMON_RTCONFIG MULTITHREAD_RTCONFIG \
    RTCONFIG_VARIABLE( string,          OutputFormat,                   "pepXML"                ) \
    RTCONFIG_VARIABLE( string,          OutputSuffix,                   ""                      ) \
    RTCONFIG_VARIABLE( string,          ProteinDatabase,                ""                      ) \
    RTCONFIG_VARIABLE( string,          DecoyPrefix,                    "rev_"                  ) \
    RTCONFIG_VARIABLE( string,          PrecursorMzToleranceRule,       "auto"                  ) \
    RTCONFIG_VARIABLE( IntegerSet,      MonoisotopeAdjustmentSet,       string("[-1,2]")        ) \
    RTCONFIG_VARIABLE( MZTolerance,     MonoPrecursorMzTolerance,       string("10 ppm")        ) \
    RTCONFIG_VARIABLE( MZTolerance,     AvgPrecursorMzTolerance,        string("1.5 mz")        ) \
    RTCONFIG_VARIABLE( MZTolerance,     FragmentMzTolerance,            string("0.5 mz")        ) \
    RTCONFIG_VARIABLE( string,          FragmentationRule,              "cid"                   ) \
    RTCONFIG_VARIABLE( bool,            FragmentationAutoRule,          true                    ) \
    RTCONFIG_VARIABLE( int,             MaxResultRank,                  2                       ) \
    RTCONFIG_VARIABLE( int,             NumIntensityClasses,            3                       ) \
    RTCONFIG_VARIABLE( int,             NumMzFidelityClasses,           3                       ) \
    RTCONFIG_VARIABLE( int,             NumBatches,                     50                      ) \
    RTCONFIG_VARIABLE( double,          TicCutoffPercentage,            0.98                    ) \
    RTCONFIG_VARIABLE( double,          ClassSizeMultiplier,            2.0                     ) \
    RTCONFIG_VARIABLE( double,          MinResultScore,                 1e-7                    ) \
    RTCONFIG_VARIABLE( int,             MinMatchedFragments,            5                       ) \
    RTCONFIG_VARIABLE( double,          MinPeptideMass,                 0.0                     ) \
    RTCONFIG_VARIABLE( double,          MaxPeptideMass,                 10000.0                 ) \
    RTCONFIG_VARIABLE( int,             MinPeptideLength,               5                       ) \
    RTCONFIG_VARIABLE( int,             MaxPeptideLength,               75                      ) \
    RTCONFIG_VARIABLE( bool,            PreferIntenseComplements,       true                    ) \
    RTCONFIG_VARIABLE( int,             ProteinSamplingTime,            15                      ) \
    RTCONFIG_VARIABLE( bool,            EstimateSearchTimeOnly,         false                   ) \
    RTCONFIG_VARIABLE( string,          CleavageRules,                  "trypsin/p"             ) \
    RTCONFIG_VARIABLE( int,             MinTerminiCleavages,            2                       ) \
    RTCONFIG_VARIABLE( int,             MaxMissedCleavages,             -1                      ) \
    RTCONFIG_VARIABLE( int,             MaxFragmentChargeState,         0                       ) \
    RTCONFIG_VARIABLE( int,             ResultsPerBatch,                200000                  ) \
    RTCONFIG_VARIABLE( bool,            ComputeXCorr,                   true                    ) \
    RTCONFIG_VARIABLE( int,             MaxPeakCount,                   300                     ) \
    RTCONFIG_VARIABLE( string,          SpectrumListFilters,            "peakPicking true 2-"   ) \
    RTCONFIG_VARIABLE( string,          ProteinListFilters,             ""                      ) \
    RTCONFIG_VARIABLE( bool,            UseSmartPlusThreeModel,         true                    ) \
    RTCONFIG_VARIABLE( string,          StaticMods,                     ""                      ) \
    RTCONFIG_VARIABLE( string,          DynamicMods,                    ""                      ) \
    RTCONFIG_VARIABLE( int,             MaxDynamicMods,                 2                       ) \
    RTCONFIG_VARIABLE( int,             MaxPeptideVariants,             1000000                 ) \
    RTCONFIG_VARIABLE( bool,            KeepUnadjustedPrecursorMz,      false                   )


namespace freicore
{
namespace myrimatch
{
    // TODO: move to its own class?
    enum MzToleranceRule { MzToleranceRule_Auto, MzToleranceRule_Mono, MzToleranceRule_Avg };

    struct RunTimeConfig : public BaseRunTimeConfig
    {

    public:
        RTCONFIG_DEFINE_MEMBERS( RunTimeConfig, MYRIMATCH_RUNTIME_CONFIG, "myrimatch.cfg" )

        string decoyPrefix;
        bool automaticDecoys;

        CVID cleavageAgent;
        boost::regex cleavageAgentRegex;
        Digestion::Config digestionConfig;

        FragmentTypesBitset defaultFragmentTypes;

        DynamicModSet   dynamicMods;
        StaticModSet    staticMods;
        double          largestNegativeDynamicModMass;
        double          largestPositiveDynamicModMass;

        int             SpectraBatchSize;
        int             ProteinBatchSize;
        int             ProteinIndexOffset;
        double          curMinPeptideMass;
        double          curMaxPeptideMass;
        int             minIntensityClassCount;
        int             minMzFidelityClassCount;
        int             maxFragmentChargeState;
        int             maxChargeStateFromSpectra;

        MzToleranceRule precursorMzToleranceRule;

        vector<MZTolerance> avgPrecursorMassTolerance;
        vector<MZTolerance> monoPrecursorMassTolerance;

        // Compute the fragment mass error bins and their associated log odds scores
        vector<double>  massErrors;
        vector<double>  mzFidelityLods;

        pwiz::identdata::IdentDataFile::Format outputFormat;

    private:
        void finalize()
        { TRACER_BI; TRACER_OP_START("RunTimeConfig::finalize"); TRACER_P(*this, "RunTimeConfig", "", READ, HEAP, "Full config of Myrimatch");
            if (bal::iequals(OutputFormat, "pepXML"))
                outputFormat = pwiz::identdata::IdentDataFile::Format_pepXML;
            else if (bal::iequals(OutputFormat, "mzIdentML"))
                outputFormat = pwiz::identdata::IdentDataFile::Format_MzIdentML;
            else
                m_warnings << "Invalid value \"" << OutputFormat << "\" for OutputFormat\n";
			TRACER(outputFormat, WRITE, HEAP, "File format to be produced");
            decoyPrefix = DecoyPrefix.empty() ? "rev_" : DecoyPrefix; TRACER(decoyPrefix, WRITE, HEAP, "Prefix for decoy sequences");
            automaticDecoys = DecoyPrefix.empty() ? false : true; TRACER(automaticDecoys, WRITE, HEAP, "Automatically determine decoy prefixes");

            if (MonoisotopeAdjustmentSet.empty())
                MonoisotopeAdjustmentSet.insert(0);
			TRACER(MonoisotopeAdjustmentSet, NOTE, HEAP, "What alternative monoisotopic peaks we should try to match");
            // TODO: move CleavageRules parsing to its own class
            trim(CleavageRules); // trim flanking whitespace

            if (bal::iequals(CleavageRules, "NoEnzyme"))
                m_warnings << "NoEnzyme is not supported. If you want non-specific digestion, set CleavageRules to the enzyme that digested your sample and set MinTerminiCleavages to 0.\n";
            else if (CleavageRules.empty())
                m_warnings << "Blank value for CleavageRules is invalid.\n";
            else
            {
                // first try to parse the token as the name of an agent
                cleavageAgent = Digestion::getCleavageAgentByName(CleavageRules);
                cleavageAgentRegex = boost::regex();

                if (cleavageAgent != CVID_Unknown || CleavageRules.find(' ') == string::npos)
                {
                    // a single token must be either a cleavage agent name or regex
                    // multiple tokens could be a cleavage agent or an old-style cleavage rule set

                    if (bal::iequals(CleavageRules, "unspecific cleavage"))
                    {
                        m_warnings << "Unspecific cleavage is not recommended. For a non-specific search, you should almost always set CleavageRules to the enzyme that digested your sample and set MinTerminiCleavages to 0.\n";
                        MinTerminiCleavages = 0;

                        // there is no regex
                    }
                    else if (bal::iequals(CleavageRules, "no cleavage"))
                    {
                        // there is no regex
                    }
                    else if (cleavageAgent == CVID_Unknown)
                    {
                        // next try to parse the token as a Perl regex
                        try
                        {
                            // regex must be zero width, so it must use at least one parenthesis;
                            // this will catch most bad cleavage agent names (e.g. "tripsen")
                            if( CleavageRules.find('(') == string::npos )
                                throw boost::bad_expression(boost::regex_constants::error_bad_pattern);
                            cleavageAgentRegex = boost::regex(CleavageRules);
                        }
                        catch (boost::bad_expression&)
                        {
                            // a bad regex or agent name is fatal
                            m_warnings << "Invalid cleavage agent name or regex \"" << CleavageRules << "\"\n";
                        }
                    }
                    else
                    {
                        // use regex for predefined cleavage agent
                        cleavageAgentRegex = boost::regex(Digestion::getCleavageAgentRegex(cleavageAgent));
                    }
                }
                else if (cleavageAgent == CVID_Unknown)
                {
                    // multiple tokens must be a CleavageRuleSet
                    CleavageRuleSet tmpRuleSet;
                    stringstream CleavageRulesStream( CleavageRules );
                    CleavageRulesStream >> tmpRuleSet;
                    cleavageAgentRegex = boost::regex(tmpRuleSet.asCleavageAgentRegex());
                }
            } TRACER(cleavageAgentRegex, WRITE, HEAP, "Regular expression marking places where the enzyme cleaves");
			TRACER_OP_START("Clean up MaxMissedCleavages"); TRACER(MaxMissedCleavages, READ, HEAP, "Previous number of missed cleavages");
            MaxMissedCleavages = MaxMissedCleavages < 0 ? 100000 : MaxMissedCleavages; TRACER(MaxMissedCleavages, WRITE, HEAP, "Cleaned up maximum missed cleavages"); TRACER_OP_END("Clean up MaxMissedCleavages"); 

            // TODO: move fragmentation rule parsing to its own class
            vector<string> fragmentationRuleTokens; TRACER_OP_START("Parse fragmentation rule"); TRACER(FragmentationRule, READ, HEAP, "Command line fragmentation rule");
            split( fragmentationRuleTokens, FragmentationRule, is_any_of(":") ); TRACER(fragmentationRuleTokens, WRITE, STACK, "Fragmentation rules split by ':'");
            if( fragmentationRuleTokens.empty() )
                m_warnings << "Blank value for FragmentationRule is invalid.\n";
            else
            {
                const string& mode = fragmentationRuleTokens[0];
                defaultFragmentTypes.reset();
                if( mode.empty() || mode == "cid" )
                {
                    defaultFragmentTypes[FragmentType_B] = true;
                    defaultFragmentTypes[FragmentType_Y] = true;
                } else if( mode == "etd" )
                {
                    defaultFragmentTypes[FragmentType_C] = true;
                    defaultFragmentTypes[FragmentType_Z_Radical] = true;
                } else if( mode == "manual" )
                {
                    if( fragmentationRuleTokens.size() != 2 )
                        m_warnings << "Manual FragmentationRule setting requires comma-separated list of ion series, e.g. 'manual:b,y'\n";
                    else
                    {
                        vector<string> fragmentTypeTokens;
                        split( fragmentTypeTokens, fragmentationRuleTokens[1], is_any_of(",") );
                
                        if( fragmentTypeTokens.empty() )
                            m_warnings << "Manual FragmentationRule setting requires comma-separated list of ion series, e.g. 'manual:b,y'\n";

                        for( size_t i=0; i < fragmentTypeTokens.size(); ++i )
                        {
                            string fragmentType = to_lower_copy(fragmentTypeTokens[i]);
                            if( fragmentType == "a" )
                                defaultFragmentTypes[FragmentType_A] = true;
                            else if( fragmentType == "b" )
                                defaultFragmentTypes[FragmentType_B] = true;
                            else if( fragmentType == "c" )
                                defaultFragmentTypes[FragmentType_C] = true;
                            else if( fragmentType == "x" )
                                defaultFragmentTypes[FragmentType_X] = true;
                            else if( fragmentType == "y" )
                                defaultFragmentTypes[FragmentType_Y] = true;
                            else if( fragmentType == "z" )
                                defaultFragmentTypes[FragmentType_Z] = true;
                            else if( fragmentType == "z*" )
                                defaultFragmentTypes[FragmentType_Z_Radical] = true;
                        }
                    }
                } else
                    m_warnings << "Invalid mode \"" << mode << "\" for FragmentationRule.\n";
            } TRACER_P(defaultFragmentTypes, "freicore::FragmentTypesBitset", (defaultFragmentTypes[FragmentType_A]?string(" a"):string())+(defaultFragmentTypes[FragmentType_B]?string(" b"):string())+(defaultFragmentTypes[FragmentType_C]?string(" c"):string())+(defaultFragmentTypes[FragmentType_X]?string(" x"):string())+(defaultFragmentTypes[FragmentType_Y]?string(" y"):string())+(defaultFragmentTypes[FragmentType_Z]?string(" z"):string())+(defaultFragmentTypes[FragmentType_Z_Radical]?string(" z*"):string()), WRITE, HEAP, "Supported fragment types"); TRACER_OP_END("Parse fragmentation rule");
			TRACER(EstimateSearchTimeOnly, READ, HEAP, "Only estimate search time");
            if( ProteinSamplingTime == 0 )
            {
                if( EstimateSearchTimeOnly )
                    m_warnings << "ProteinSamplingTime = 0 disables EstimateSearchTimeOnly.\n";
                EstimateSearchTimeOnly = 0; TRACER(EstimateSearchTimeOnly, WRITE, HEAP, "ProteinSamplingTime = 0 disables EstimateSearchTimeOnly");
            } TRACER(ProteinSamplingTime, READ, HEAP, "For how long to sample proteins to guess the total search time");
			TRACER(PrecursorMzToleranceRule, READ, HEAP, "How to calculate precursor tolerances, coming from the command line");
            // TODO: move mzToleranceRule to its own class 
            bal::to_lower(PrecursorMzToleranceRule);
            if( PrecursorMzToleranceRule == "auto" )
                precursorMzToleranceRule = MzToleranceRule_Auto;
            else if( PrecursorMzToleranceRule == "mono" )
                precursorMzToleranceRule = MzToleranceRule_Mono;
            else if( PrecursorMzToleranceRule == "avg" )
                precursorMzToleranceRule = MzToleranceRule_Avg;
            else
                m_warnings << "Invalid mode \"" << PrecursorMzToleranceRule << "\" for PrecursorMzToleranceRule.\n";
			TRACER(precursorMzToleranceRule, WRITE, HEAP, "Parsed precursor tolerance rule (0-auto, 1-monoisotopic mass, 2-average mass)");
            if (MonoisotopeAdjustmentSet.size() > 1 && (1000.0 + MonoPrecursorMzTolerance) - 1000.0 > 0.2)
                m_warnings << "MonoisotopeAdjustmentSet should be set to 0 when the MonoPrecursorMzTolerance is wide.\n";

            ProteinIndexOffset = 0;

            string cwd;
            cwd.resize( MAX_PATH );
            getcwd( &cwd[0], MAX_PATH );
            WorkingDirectory = cwd.c_str(); TRACER(WorkingDirectory, HEAP, WRITE, "Working directory");
			TRACER_OP_START("Cleanup TicCutoffPercentage"); TRACER(TicCutoffPercentage, READ, HEAP, "Each peak should be above this percent of TIC to be counted");
			if( TicCutoffPercentage > 1.0 )
            {
                TicCutoffPercentage /= 100.0; TRACER(TicCutoffPercentage, WRITE, HEAP, "The value was > 1.0, was divided by 100");
                m_warnings << "TicCutoffPercentage must be between 0 and 1 (100%)\n";
            } TRACER_OP_END("Cleanup TicCutoffPercentage"); 

			TRACER_OP_START("parse dynamic mods"); TRACER(DynamicMods, READ, HEAP, "Command-line dynamic mods specification");
            if( !DynamicMods.empty() )
            {
                try {dynamicMods = DynamicModSet( DynamicMods );}
                catch (exception& e) {m_warnings << "Unable to parse DynamicMods \"" << DynamicMods << "\": " << e.what() << "\n";}
            } TRACER_P(dynamicMods, "freicore::DynamicModSet", (string)dynamicMods, WRITE, HEAP, "Parsed set of dynamic mods"); TRACER_OP_END("parse dynamic mods");
			TRACER_OP_START("parse static mods"); TRACER(StaticMods, READ, HEAP, "Command-line static mods specification");
            if( !StaticMods.empty() )
            {
                try {staticMods = StaticModSet( StaticMods );}
                catch (exception& e) {m_warnings << "Unable to parse StaticMods \"" << StaticMods << "\": " << e.what() << "\n";}
            } TRACER_P(staticMods, "freicore::StaticModSet", (string)staticMods, WRITE, HEAP, "Parsed set of dynamic mods"); TRACER_OP_END("parse static mods");
            TRACER_OP_START("largest positive and negative mod mass"); TRACER_REF(dynamicMods, READ, HEAP, "All the dynamic mods"); TRACER_REF(MaxDynamicMods, READ, HEAP, "Maximum number of dynamic mods");
            BOOST_FOREACH(const DynamicMod& mod, dynamicMods)
            {
                largestPositiveDynamicModMass = max(largestPositiveDynamicModMass, mod.modMass * MaxDynamicMods);
                largestNegativeDynamicModMass = min(largestNegativeDynamicModMass, mod.modMass * MaxDynamicMods);
            } TRACER(largestPositiveDynamicModMass, WRITE, HEAP, "Largest positive mass shift arising from dynamic mods"); TRACER(largestNegativeDynamicModMass, WRITE, HEAP, "Largest negative mass shift arising from dynamic mods"); TRACER_OP_END("largest positive and negative mod mass");
			TRACER_OP_START("determine number of intensity/mz fidelity class counts"); TRACER(ClassSizeMultiplier, READ, HEAP, "Each class is this multiplier times the size of previous class"); TRACER(NumIntensityClasses, READ, HEAP, "How many peak intensity classes"); TRACER(NumMzFidelityClasses, READ, HEAP, "How many classes for m/z fidelity calculation");
            if( ClassSizeMultiplier > 1 )
            {
                minIntensityClassCount = int( ( pow( ClassSizeMultiplier, NumIntensityClasses ) - 1 ) / ( ClassSizeMultiplier - 1 ) );
                minMzFidelityClassCount = int( ( pow( ClassSizeMultiplier, NumMzFidelityClasses ) - 1 ) / ( ClassSizeMultiplier - 1 ) );
            } else
            {
                minIntensityClassCount = NumIntensityClasses;
                minMzFidelityClassCount = NumMzFidelityClasses;
            } TRACER(minIntensityClassCount, WRITE, HEAP, "???"); TRACER(minMzFidelityClassCount, WRITE, HEAP, "???"); TRACER_OP_END("determine number of intensity/mz fidelity class counts");
            
			maxChargeStateFromSpectra = 0; TRACER_OP_START("Determine maximum fragment charge state"); TRACER(MaxFragmentChargeState, READ, HEAP, "???"); TRACER(NumChargeStates, READ, HEAP, "???");
            maxFragmentChargeState = ( MaxFragmentChargeState > 0 ? MaxFragmentChargeState+1 : NumChargeStates );
            TRACER(maxFragmentChargeState, WRITE, HEAP, "Either MaxFragmentChargeState or NumChargeStates..."); TRACER_OP_END("Determine maximum fragment charge state");
			vector<double> insideProbs; TRACER_OP_START("Calculate mz fidelity log odds ratio vector");
            int numBins = 5; TRACER(numBins, READ, STACK, "Number of bins is set to a constant");
            // Divide the fragment mass error into half and use it as standard deviation
            double stdev = FragmentMzTolerance.value*0.5; TRACER_OP_START("determine stdev"); TRACER_P(FragmentMzTolerance, "MZTolerance", TRACER_S2S(FragmentMzTolerance), READ, HEAP, "Fragment m/z tolerance"); TRACER(stdev, WRITE, STACK, "Standard deviation is 1/2 of FragmentMzTolerance"); TRACER_OP_END("determine stdev"); 
            massErrors.clear();
            insideProbs.clear();
            mzFidelityLods.clear();
            // Divide the mass error distributions into 10 bins.
            for(int j = 1; j <= numBins; ++j) 
            { TRACER_OP_START("Calculate bin mass error and probability value"); TRACER_BI; TRACER(j, READ, STACK, "Bin number");
                // Compute the mass error associated with each bin.
                double massError = FragmentMzTolerance.value * ((double)j/(double)numBins); TRACER(massError, WRITE, STACK, "mass error for the bin - increases linearly");
                // Compute the cumulative distribution function of massError 
                // with mu=0 and sig=stdev
                double errX = (massError-0)/(stdev*sqrt(2.0)); 
                double cdf = 0.5 * (1.0+pwiz::math::erf(errX)); TRACER(cdf, WRITE, STACK, "probability that the error is less than massError, given normal distribution with stdev"); 
                // Compute the gaussian inside probability
                double insideProb = 2.0*cdf-1.0; TRACER(insideProb, WRITE, STACK, "probability that an error lies within (-massError, massError)");
                // Save the mass errors and inside probabilities
                massErrors.push_back(massError);
                insideProbs.push_back(insideProb); TRACER_BO; TRACER_OP_END("Calculate bin mass error and probability value");
            } TRACER(massErrors, WRITE, HEAP, "Mass error vector"); TRACER(insideProbs, WRITE, HEAP, "Probability that the error is at least as big for the massError vector");
            // mzFidelity bin probablities are dependent on the number of bin. So,
            // compute the probabilities only once.
            // Compute the probability associated with each mass error bin
            double denom = insideProbs.back(); TRACER_BI; TRACER(denom, WRITE, STACK, "The probability of the biggest bin, should be close to 1.0");
            for(int j = 0; j < numBins; ++j) 
            { TRACER_OP_START("Calculate mz fidelity log odds ratio for bin"); TRACER_BI; TRACER(j, READ, STACK, "Bin number");
                double prob;
                if(j==0) {
                    prob = insideProbs[j]/denom;
                } else {
                    prob = (insideProbs[j]-insideProbs[j-1])/denom;
                } TRACER(prob, WRITE, STACK, "Probability that the value lies exactly in the particular bin");
                // Compute the log odds ratio of GaussianProb to Uniform probability and save it
                mzFidelityLods.push_back(log(prob*(double)numBins)); TRACER(mzFidelityLods.back(), WRITE, HEAP, "log(prob of the bin * numBins)"); TRACER_OP_END("Calculate mz fidelity log odds ratio for bin"); TRACER_BO;
            } TRACER(mzFidelityLods, WRITE, HEAP, "Calculated mz fidelity log odds ratios"); TRACER_OP_END("Calculate mz fidelity log odds ratio vector");
            /*cout << "Error-Probs:" << endl;
            for(int j = 0; j < numBins; ++j) 
            {
                cout << massErrors[j] << ":" << mzFidelityLods[j] << " ";
            }
            cout << endl;*/
            //exit(1);

            BaseRunTimeConfig::finalize(); TRACER_BO; TRACER_OP_END("RunTimeConfig::finalize"); TRACER_BO;
        }
    };

    extern RunTimeConfig* g_rtConfig;
}
}

#endif
