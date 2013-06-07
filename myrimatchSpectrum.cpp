//
// $Id: myrimatchSpectrum.cpp 132 2012-09-25 22:04:42Z chambm $
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

#include "stdafx.h"
#include "myrimatchSpectrum.h"

using namespace freicore;

namespace std
{
	ostream& operator<< ( ostream& o, const freicore::myrimatch::PeakInfo& rhs )
	{
		return o << rhs.normalizedIntensity;
	}
}

namespace freicore
{
namespace myrimatch
{
	void Spectrum::Preprocess()
	{   TRACER_METHOD_START("Spectrum::Preprocess");
		PeakPreData::iterator itr;
		PeakPreData::reverse_iterator r_itr;
		TRACER(mzOfPrecursor, READ, HEAP, "m/z of precursor ion"); 
		if( mzOfPrecursor < 1 )
		{ TRACER_OP_START("Clear the preprocessed peaks");
			peakPreData.clear(); TRACER(peakPreData, WRITE, HEAP, "Clearing the preprocessed peaks"); TRACER_OP_END("Clear the preprocessed peaks");
			TRACER_METHOD_END("Spectrum::Preprocess"); return;
		}

		if( peakPreData.empty() ) {
			TRACER_METHOD_END("Spectrum::Preprocess"); return; }
		TRACER(precursorMzType, READ, HEAP, "precursor m/z type"); TRACER(g_rtConfig->precursorMzToleranceRule, READ, HEAP, "precursor m/z tolerance rule");
        // calculate precursor mass hypotheses
        if (precursorMzType == MassType_Monoisotopic || g_rtConfig->precursorMzToleranceRule == MzToleranceRule_Mono)
        {	TRACER_OP_START("Generate monoisotopic precursor hypotheses"); TRACER(g_rtConfig->MonoisotopeAdjustmentSet, READ, HEAP, "monoisotope adjustment set"); TRACER(possibleChargeStates, READ, HEAP, "possible spectrum charge states");
            // for monoisotopic precursors, create a hypothesis for each adjustment and possible charge state
            IntegerSet::const_iterator itr = g_rtConfig->MonoisotopeAdjustmentSet.begin();
            for (; itr != g_rtConfig->MonoisotopeAdjustmentSet.end(); ++itr)
                BOOST_FOREACH(int charge, possibleChargeStates)
                {	TRACER_OP_START("Generate monoisotopic precursor hypothesis"); int adj=*itr; TRACER(adj, READ, STACK, "monoisotope adjustment"); TRACER(charge, READ, STACK, "spectrum charge");
                    PrecursorMassHypothesis p;
                    p.mass = Ion::neutralMass(mzOfPrecursor, charge, 0, *itr); TRACER(p.mass, WRITE, STACK, "mass shift"); 
                    p.massType = MassType_Monoisotopic; TRACER(p.massType, WRITE, STACK, "mass type - monoisotopic");
                    p.charge = charge;
                    precursorMassHypotheses.push_back(p); TRACER_OP_END("Generate monoisotopic precursor hypothesis");
                } 
        TRACER_OP_END("Generate monoisotopic precursor hypotheses");}
        else
        { TRACER_OP_START("Generate average precursor hypotheses"); TRACER(possibleChargeStates, READ, HEAP, "possible spectrum charge states");
            // for average precursors, create a hypothesis for each possible charge state
            BOOST_FOREACH(int charge, possibleChargeStates)
            { TRACER_OP_START("Generate average precursor hypothesis");
                PrecursorMassHypothesis p; TRACER(charge, READ, STACK, "spectrum charge");
                p.mass = Ion::neutralMass(mzOfPrecursor, charge); TRACER(p.mass, WRITE, STACK, "mass shift"); 
                p.massType = MassType_Average; TRACER(p.massType, WRITE, STACK, "mass type - average");
                p.charge = charge;
                precursorMassHypotheses.push_back(p);
            TRACER_OP_END("Generate average precursor hypothesis"); }
        TRACER_OP_END("Generate average precursor hypotheses"); }

        // sort hypotheses ascending by mass
        sort(precursorMassHypotheses.begin(), precursorMassHypotheses.end());
		BOOST_FOREACH(PrecursorMassHypothesis &h, precursorMassHypotheses) { TRACER(h, WRITE, HEAP, "Precursor mass hypothesis"); }
        //PeakPreData unfilteredPeakPreData = peakPreData;
		TRACER_OP_START("Filter out peaks above the largest precursor hypothesis mass"); TRACER(peakPreData, READ, HEAP, "Preprocessed peak data"); TRACER(precursorMassHypotheses.back(), READ, HEAP, "Maximum precursor mass hypothesis"); TRACER(g_rtConfig->AvgPrecursorMzTolerance, READ, HEAP, "Average precursor m/z tolerance");
        // filter out peaks above the largest precursor hypthesis' mass
        peakPreData.erase( peakPreData.upper_bound( precursorMassHypotheses.back().mass + g_rtConfig->AvgPrecursorMzTolerance ), peakPreData.end() );
		TRACER(peakPreData, WRITE, HEAP, "Preprocessed peak data without peaks over max precursor mass"); TRACER_OP_END("Filter out peaks above the largest precursor hypothesis mass"); 
		// The old way of calculating these values:
        /*mzLowerBound = peakPreData.begin()->first;
		mzUpperBound = peakPreData.rbegin()->first;
		totalPeakSpace = mzUpperBound - mzLowerBound;*/

		FilterByTIC( g_rtConfig->TicCutoffPercentage );
        FilterByPeakCount( g_rtConfig->MaxPeakCount );

		if( peakPreData.empty() ) {
			TRACER_METHOD_END("Spectrum::Preprocess"); return; }
		TRACER_OP_START("Remove water loss peaks"); TRACER(WATER_MONO, READ, HEAP, "monoisotopic mass of water"); TRACER(peakPreData, READ, HEAP, "Before removing water loss peaks");
        BOOST_FOREACH(int charge, possibleChargeStates)
        {   TRACER_OP_START("Remove water loss for precursor and charge"); TRACER_BI; TRACER(mzOfPrecursor, READ, HEAP, "m/z of precursor"); TRACER(charge, READ, STACK, "charge"); double remove=mzOfPrecursor - WATER_MONO/charge; TRACER(remove, WRITE, STACK, "m/z for removing single water loss");
		    PeakPreData::iterator precursorWaterLossItr = peakPreData.findNear( mzOfPrecursor - WATER_MONO/charge, g_rtConfig->FragmentMzTolerance, true );
		    if( precursorWaterLossItr != peakPreData.end() )
			    peakPreData.erase( precursorWaterLossItr );
			double remove2=mzOfPrecursor - 2*WATER_MONO/charge; TRACER(remove2, WRITE, STACK, "m/z for removing double water loss");
		    PeakPreData::iterator precursorDoubleWaterLossItr = peakPreData.findNear( mzOfPrecursor - 2*WATER_MONO/charge, g_rtConfig->FragmentMzTolerance, true );
		    if( precursorDoubleWaterLossItr != peakPreData.end() )
                peakPreData.erase( precursorDoubleWaterLossItr ); 
        TRACER_BO; TRACER_OP_END("Remove water loss for precursor and charge"); } TRACER(peakPreData, WRITE, HEAP, "After removing water loss peaks");
		TRACER_OP_END("Remove water loss peaks");
		if( peakPreData.empty() ) {
			TRACER_METHOD_END("Spectrum::Preprocess"); return; }

        // results for each possible charge state are stored separately
        resultsByCharge.resize(possibleChargeStates.back());
        BOOST_FOREACH(SearchResultSetType& resultSet, resultsByCharge)
            resultSet.max_ranks( g_rtConfig->MaxResultRank );

		ClassifyPeakIntensities(); // for mvh

        //swap(peakPreData, unfilteredPeakPreData);
        NormalizePeakIntensities(); // for xcorr
        //swap(peakPreData, unfilteredPeakPreData);

		peakCount = (int) peakData.size();
		TRACER_OP_START("Divide the spectrum peak space into equal m/z bins");
		// Divide the spectrum peak space into equal m/z bins
		//cout << mzUpperBound << "," << mzLowerBound << endl;
		double spectrumMedianMass = totalPeakSpace/2.0; TRACER(spectrumMedianMass, WRITE, STACK, "median spectrum mass (max peak - min peak) / 2"); TRACER(g_rtConfig->FragmentMzTolerance, READ, HEAP, "fragment m/z tolerance");
        double fragMassError = g_rtConfig->FragmentMzTolerance.units == MZTolerance::PPM ? (spectrumMedianMass*g_rtConfig->FragmentMzTolerance.value*1e-6):g_rtConfig->FragmentMzTolerance.value; TRACER(fragMassError, WRITE, STACK, "fragment mass error (for PPM uses median mass for reference)");
		//cout << fragMassError << "," << mOfPrecursor << endl;
		int totalPeakBins = (int) round( totalPeakSpace / ( fragMassError * 2.0f ), 0 ); TRACER(totalPeakBins, WRITE, STACK, "total peak bins - ~one bin per each +-fragment mass error interval");
		initialize( g_rtConfig->NumIntensityClasses+1, g_rtConfig->NumMzFidelityClasses );
		for( PeakData::iterator itr = peakData.begin(); itr != peakData.end(); ++itr )
		{
            if (itr->second.intenClass > 0)
			    ++ intenClassCounts[ itr->second.intenClass-1 ];
		}
		intenClassCounts[ g_rtConfig->NumIntensityClasses ] = totalPeakBins - peakCount; TRACER(intenClassCounts, WRITE, HEAP, "The last class is totalPeakBins - peakCount");
		//cout << id.nativeID << ": " << intenClassCounts << endl;
		TRACER_OP_START("calculate mzFidelity thresholds");
        int divider = 0;
	    for( int i=0; i < g_rtConfig->NumMzFidelityClasses-1; ++i )
	    {
		    divider += 1 << i;
		    mzFidelityThresholds[i] = g_rtConfig->FragmentMzTolerance.value * (double)divider / (double)g_rtConfig->minMzFidelityClassCount;
	    }
		mzFidelityThresholds.back() = g_rtConfig->FragmentMzTolerance.value; TRACER(mzFidelityThresholds, WRITE, HEAP, "m/z fidelity thresholds"); TRACER_OP_END("calculate mzFidelity thresholds");
        //cout << id.nativeID << ": " << mzFidelityThresholds << endl;

		//totalPeakSpace = peakPreData.rbegin()->first - peakPreData.begin()->first;
		//if( id.nativeID == 1723 )
		//	cout << totalPeakSpace << " " << mzUpperBound << " " << mzLowerBound << endl;
		TRACER_OP_END("Divide the spectrum peak space into equal m/z bins");
        // we no longer need the raw intensities
		peakPreData.clear(); TRACER(peakPreData, WRITE, HEAP, "Data no longer needed");
		TRACER_OP_START("set fragment types"); TRACER(g_rtConfig->FragmentationAutoRule, READ, HEAP, "Automatic fragmentation");
        // set fragment types
        fragmentTypes.reset();
        if( g_rtConfig->FragmentationAutoRule )
        {
            if( dissociationTypes.count(pwiz::cv::MS_CID) > 0 )
            {
                fragmentTypes[FragmentType_B] = true;
                fragmentTypes[FragmentType_Y] = true; TRACER(fragmentTypes, WRITE, HEAP, "CID fragmentation");
            }

            if( dissociationTypes.count(pwiz::cv::MS_ETD) > 0 )
            {
                fragmentTypes[FragmentType_B] = false; // override CID
                fragmentTypes[FragmentType_C] = true;
                fragmentTypes[FragmentType_Z_Radical] = true; TRACER(fragmentTypes, WRITE, HEAP, "ETD fragmentation");
            }
        }
		
        if( fragmentTypes.none() ) {
			fragmentTypes = g_rtConfig->defaultFragmentTypes; TRACER(fragmentTypes, WRITE, HEAP, "Use default fragment types"); }
	TRACER_OP_END("set fragment types"); TRACER_METHOD_END("Spectrum::Preprocess");}

    void Spectrum::ClassifyPeakIntensities()
	{ TRACER_METHOD_START("Spectrum::ClassifyPeakIntensities");
		// Sort peaks by intensity.
		// Use multimap because multiple peaks can have the same intensity.
		typedef multimap< double, double > IntenSortedPeakPreData;
		IntenSortedPeakPreData intenSortedPeakPreData;
		for( PeakPreData::iterator itr = peakPreData.begin(); itr != peakPreData.end(); ++itr )
		{
			IntenSortedPeakPreData::iterator iItr = intenSortedPeakPreData.insert( make_pair( itr->second, itr->second ) );
			iItr->second = itr->first;
		}

		// Restore the sorting order to be based on MZ
		IntenSortedPeakPreData::reverse_iterator r_iItr = intenSortedPeakPreData.rbegin();
        //cout << id.nativeID << peakPreData.size() << endl;
		peakPreData.clear();
		peakData.clear(); int peakNumber = intenSortedPeakPreData.size(); /* ROMAN */
		TRACER(g_rtConfig->ClassSizeMultiplier, READ, HEAP, "class size multiplier"); TRACER(g_rtConfig->minIntensityClassCount, READ, HEAP, "minimum intensity class count"); TRACER(peakNumber, READ, STACK, "number of peaks"); 
		TRACER(g_rtConfig->NumIntensityClasses, READ, HEAP, "number of intensity classes"); for( int i=0; i < g_rtConfig->NumIntensityClasses; ++i )
		{ TRACER_OP_START("Place peaks for intensity class"); TRACER_BI; TRACER(i, WRITE, STACK, "intensity class");
			int numFragments = (int) round( (double) ( pow( (double) g_rtConfig->ClassSizeMultiplier, i ) * intenSortedPeakPreData.size() ) / (double) g_rtConfig->minIntensityClassCount, 0 ); TRACER(numFragments, WRITE, STACK, "number of fragments");
			//cout << numFragments << endl;
			for( int j=0; r_iItr != intenSortedPeakPreData.rend() && j < numFragments; ++j, ++r_iItr )
			{
				double mz = r_iItr->second;
				double inten = r_iItr->first;
				peakPreData[ mz ] = inten;
				peakData[ mz ].intenClass = i+1;
			}
		TRACER_BO; TRACER_OP_END("Place peaks for intensity class"); }
		intenSortedPeakPreData.clear();
	TRACER_METHOD_END("Spectrum::ClassifyPeakIntensities");}

    // the m/z width for xcorr bins
    const double binWidth = Proton;

    #define IS_VALID_INDEX(index,length) (index >=0 && index < length ? true : false)

    /* This function processes the spectra to compute the fast XCorr implemented in Crux. 
       Ideally, this function has to be called prior to spectrum filtering.             
    */
    void Spectrum::NormalizePeakIntensities()
    { TRACER_METHOD_START("Spectrum::NormalizePeakIntensities"); TRACER_BI; TRACER(precursorMassHypotheses.back().mass, READ, HEAP, "maximum mass from the precursor mass hypotheses for this spectrum");
        // Get the number of bins and bin width for the processed peak array
        double massCutOff = precursorMassHypotheses.back().mass + 50; TRACER(massCutOff, WRITE, STACK, "mass cutoff"); 
        TRACER(binWidth, READ, HEAP, "Bin width - 1 proton mass");
        int maxBins;
        if (massCutOff > 512)
            maxBins = (int) ceil(massCutOff / 1024) * 1024;
        else
            maxBins = 512;
        TRACER(maxBins, WRITE, STACK, "max number of bins");  TRACER(peakPreData, READ, HEAP, "Peak data for the spectrum");
        // Detemine the max mass of a fragmet peak.
        double maxPeakMass = peakPreData.rbegin()->first; TRACER(maxPeakMass, WRITE, STACK, "maximum peak mass"); 
        TRACER_OP_START("section spectrum into regions, find base peaks"); TRACER(peakPreData, READ, HEAP, "peaks intensities normal");
        // Section the original peak array in 10 regions and find the
        // base peak in each region. Also, square-root the peak intensities
        const int numberOfRegions = 10; TRACER(numberOfRegions, READ, STACK, "number of regions"); 

        vector<float> basePeakIntensityByRegion(numberOfRegions, 1); TRACER(basePeakIntensityByRegion, WRITE, STACK, "initial base peak intensity for each region"); 
        int regionSelector = (int) (maxPeakMass / numberOfRegions); /* ROMAN - for better output */ int prevNormalizationIndex = -1; TRACER(regionSelector, WRITE, STACK, "region size");  
        for(PeakPreData::iterator itr = peakPreData.begin(); itr != peakPreData.end(); ++itr)
        { 
            itr->second = sqrt(itr->second);
            int mzBin = round(itr->first / binWidth); 
            int normalizationIndex = mzBin / regionSelector; /* ROMAN */ if(prevNormalizationIndex!=normalizationIndex) { TRACER(itr->first, READ, HEAP, "Peak mass starting new region"); TRACER_BI; TRACER(normalizationIndex, WRITE, STACK, "Region number"); TRACER_BO; prevNormalizationIndex = normalizationIndex; }
            if( IS_VALID_INDEX( normalizationIndex,numberOfRegions ) )
                basePeakIntensityByRegion[normalizationIndex] = max(basePeakIntensityByRegion[normalizationIndex],
                                                                    itr->second);
        } TRACER(peakPreData, WRITE, HEAP, "peaks intensity square rooted"); TRACER(basePeakIntensityByRegion, WRITE, STACK, "final base peak intensity for each region"); 
        TRACER_OP_END("section spectrum into regions, find base peaks"); TRACER_OP_START("Normalize peaks 0-50"); TRACER(peakPreData, READ, HEAP, "Peak data to normalize");
        // Normalize peaks in each region from 0 to 50. 
        // Use base peak in each region for normalization. 
        for(PeakPreData::iterator itr = peakPreData.begin(); itr != peakPreData.end(); ++itr)
        { 
            int mzBin = round(itr->first / binWidth); 
            int normalizationIndex = mzBin / regionSelector;
            if( IS_VALID_INDEX( normalizationIndex,numberOfRegions ) ) /* ROMAN */ { 
                peakData[itr->first].normalizedIntensity = (itr->second / basePeakIntensityByRegion[normalizationIndex]) * 50; } else { TRACER(itr->first, READ, HEAP, "Peak intensity not normalized - BUG"); }
        } TRACER(peakData, WRITE, HEAP, "Normalized peaks"); TRACER_OP_END("Normalize peaks 0-50"); TRACER_METHOD_END("Spectrum::NormalizePeakIntensities");
    }

    // Assign an intensity of 50 to fragment ions. 
    // Assign an intensity of 25 to bins neighboring the fragment ions.
    // Assign an intensity of 10 to neutral losses.
    void addXCorrFragmentIon(vector<float>& theoreticalSpectrum, double fragmentMass, int fragmentCharge, FragmentTypes fragmentType)
    { TRACER_BI; TRACER_OP_START("addXCorrFragmentIon"); TRACER(theoreticalSpectrum, READ, HEAP, "theoretical spectrum"); TRACER(fragmentMass, READ, STACK, "fragment mass"); TRACER(fragmentCharge, READ, STACK, "fragment charge"); TRACER_P(fragmentType, "FragmentTypes", lexical_cast<string>(fragmentType), READ, STACK, "fragment type");
        int peakDataLength = theoreticalSpectrum.size();
        int mzBin = round(fragmentMass / binWidth); TRACER(mzBin, WRITE, STACK, "peak bin");
        if( IS_VALID_INDEX( mzBin, peakDataLength ) )
        {
            theoreticalSpectrum[mzBin] = 50;

            // Fill the neighbouring bins
            if( IS_VALID_INDEX( (mzBin-1), peakDataLength ) )
                theoreticalSpectrum[mzBin-1] = 25;
            if( IS_VALID_INDEX( (mzBin+1), peakDataLength ) )
                theoreticalSpectrum[mzBin+1] = 25;
            
            // Neutral loss peaks
            if(fragmentType == FragmentType_B || fragmentType == FragmentType_Y)
            {
                int NH3LossIndex = round( (fragmentMass - (AMMONIA_MONO / fragmentCharge)) / binWidth ); TRACER(NH3LossIndex, WRITE, STACK, "ammonia loss bin");
                if( IS_VALID_INDEX( NH3LossIndex, peakDataLength ) )
                    theoreticalSpectrum[NH3LossIndex] = 10;
            }

            if(fragmentType == FragmentType_B)
            {
                int H20LossIndex = round( (fragmentMass - (WATER_MONO / fragmentCharge)) / binWidth ); TRACER(H20LossIndex, WRITE, STACK, "water loss bin");
                if ( IS_VALID_INDEX( H20LossIndex, peakDataLength ) )
                    theoreticalSpectrum[H20LossIndex] = 10;
            }
        } TRACER(theoreticalSpectrum, WRITE, HEAP, "theoretical spectrum");
    TRACER_OP_END("addXCorrFragmentIon"); TRACER_BO;}

    void Spectrum::ComputeXCorrs()
    { TRACER_METHOD_START("Spectrum::ComputeXCorrs"); 
        BOOST_FOREACH(int chargeHypothesis, possibleChargeStates)
        { TRACER_OP_START("Test charge hypothesis"); TRACER_BI; TRACER(chargeHypothesis, WRITE, STACK, "charge hypothesis");
            // Get the number of bins and bin width for the processed peak array
            double massCutOff; TRACER(precursorMassHypotheses, READ, HEAP, "Mass hypotheses");
            BOOST_REVERSE_FOREACH(PrecursorMassHypothesis& hypothesis, precursorMassHypotheses)
                if (hypothesis.charge == chargeHypothesis)
                {
                    massCutOff = hypothesis.mass + 50; TRACER(massCutOff, WRITE, STACK, "mass cutoff - 50 + mass hypothesis for the charge");
                    break;
                }

            int maxBins;
            if (massCutOff > 512)
                maxBins = (int) ceil(massCutOff / 1024) * 1024;
            else
                maxBins = 512;
			TRACER(maxBins, WRITE, STACK, "Maximum bins for the spectrum");
            // populate a vector representation of the peak data
            vector<float> peakDataForXCorr(maxBins, 0);
            int peakDataLength = peakDataForXCorr.size();
            TRACER(peakDataForXCorr, WRITE, STACK, "binned spectrum peaks, empty"); TRACER(peakData, READ, HEAP, "normalized spectrum peaks");
            for (PeakData::iterator itr = peakData.begin(); itr != peakData.end(); ++itr)
            {
                int mzBin = round(itr->first / binWidth);
				if ( IS_VALID_INDEX(mzBin,maxBins) ) { if(peakDataForXCorr[mzBin]!=0) { TRACER_OP_START("Overwriting bin value with newer peak"); TRACER(mzBin, READ, STACK, "Bin number"); TRACER(peakDataForXCorr[mzBin], READ, STACK, "Old value"); TRACER(itr->second.normalizedIntensity, WRITE, STACK, "New value"); TRACER_OP_END("Overwriting bin value with newer peak"); }
					peakDataForXCorr[mzBin] = itr->second.normalizedIntensity; }
            } TRACER(peakDataForXCorr, WRITE, STACK, "binned spectrum peaks");

            // Compute the cumulative spectrum
            for (int i = 0; i < peakDataLength; ++i)
                for (int j = i - 75; j <= i + 75; ++j)
                    if ( IS_VALID_INDEX(j,maxBins) )
                        peakDataForXCorr[i] -= (peakDataForXCorr[j] / 151);
			TRACER(peakDataForXCorr, WRITE, STACK, "dampen each bin by average intensity of (-75, +75) bins - BUG");
            int z = chargeHypothesis-1; TRACER(z, WRITE, STACK, "charge - 1");
            int maxIonCharge = max(1, z); TRACER(maxIonCharge, WRITE, STACK, "max ion charge -1");

            SearchResultSetType& resultSet = resultsByCharge[z]; //TRACER(resultSet, READ, HEAP, "result for charge");
            typedef SearchResultSetType::RankMap RankMap;

            if (resultSet.empty()) {
				TRACER_BO; bool empty=true; TRACER(empty, WRITE, STACK, "empty result set for charge, skipping"); TRACER_OP_END("Test charge hypothesis"); continue; }

            RankMap resultsByRank = resultSet.byRankAndCategory(); //TRACER(resultsByRank, WRITE, STACK, "results by rank and category");

            // first=rank, second=vector of tied results
            BOOST_FOREACH(RankMap::value_type& rank, resultsByRank)
            BOOST_FOREACH(const SearchResultSetType::SearchResultPtr& resultPtr, rank.second)
            { TRACER_OP_START("calculate xcorr for search result"); TRACER_BI; //TRACER(rank, WRITE, STACK, "rank"); 
                const SearchResult& result = *resultPtr; // TRACER(result, READ, STACK, "search results");

                // Get the expected width of the array
                vector<float> theoreticalSpectrum(peakDataLength, 0); TRACER(theoreticalSpectrum, WRITE, STACK, "theoretical spectrum (binned)");

                size_t seqLength = result.sequence().length();

                // For each peptide bond and charge state
                Fragmentation fragmentation = result.fragmentation(true, true); //TRACER(fragmentation, WRITE, STACK, "fragmentation");
                for(int charge = 1; charge <= maxIonCharge; ++charge)
                { TRACER_OP_START("add theoretical ions for charge"); TRACER_BI; TRACER(charge, WRITE, STACK, "charge"); 
                    for(size_t fragIndex = 0; fragIndex < seqLength; ++fragIndex)
                    {
                        size_t nLength = fragIndex;
                        size_t cLength = seqLength - fragIndex;

                        if(nLength > 0)
                        {
                            if ( fragmentTypes[FragmentType_A] )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.a(nLength, charge), charge, FragmentType_A);
                            if ( fragmentTypes[FragmentType_B] )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.b(nLength, charge), charge, FragmentType_B);
                            if ( fragmentTypes[FragmentType_C] && nLength < seqLength )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.c(nLength, charge), charge, FragmentType_C);
                        }

                        if(cLength > 0)
                        {
                            if ( fragmentTypes[FragmentType_X] && cLength < seqLength )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.x(cLength, charge), charge, FragmentType_X);
                            if ( fragmentTypes[FragmentType_Y] )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.y(cLength, charge), charge, FragmentType_Y);
                            if ( fragmentTypes[FragmentType_Z] )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.z(cLength, charge), charge, FragmentType_Z);
                            if ( fragmentTypes[FragmentType_Z_Radical] )
                                addXCorrFragmentIon(theoreticalSpectrum, fragmentation.zRadical(cLength, charge), charge, FragmentType_Z_Radical);
                        }
                    }
                TRACER_BO; TRACER_OP_END("add theoretical ions for charge"); }
                TRACER_OP_START("compute dot product of theoretical spectrum and binned peaks"); TRACER(peakDataForXCorr, READ, STACK, "peak data"); TRACER(theoreticalSpectrum, READ, STACK, "theoretical spectrum");
                double rawXCorr = 0.0; 
                for(int index = 0; index < peakDataLength; ++index)
                    rawXCorr += peakDataForXCorr[index] * theoreticalSpectrum[index];
                TRACER(rawXCorr, WRITE, STACK, "raw xcorr"); double &XCorrRef = (const_cast<Spectrum::SearchResultType&>(result)).XCorr; XCorrRef = (rawXCorr / 1e4); TRACER(XCorrRef, WRITE, HEAP, "search result xcorr"); TRACER_OP_END("compute dot product of theoretical spectrum and binned peaks"); 
            TRACER_BO; TRACER_OP_END("calculate xcorr for search result"); } TRACER_BO; TRACER_OP_END("Test charge hypothesis");
        }
    TRACER_METHOD_END("Spectrum::ComputeXCorrs");}

    void Spectrum::computeSecondaryScores()
    {
		//Compute the average and the mode of the MVH and mzFidelity distrubutions
		double averageMVHValue = 0.0;
		double totalComps = 0.0;
		int maxValue = INT_MIN;
		for(flat_map<int,int>::iterator itr = mvhScoreDistribution.begin(); itr!= mvhScoreDistribution.end(); ++itr) {
			if((*itr).first==0) {
				continue;
			}
			// Sum the score distribution
			averageMVHValue += ((*itr).second * (*itr).first);
			totalComps += (*itr).second;
			// Get the max value
			maxValue = maxValue < (*itr).second ? (*itr).second : maxValue;
		}
		// Compute the average
		averageMVHValue /= totalComps;

		// Locate the most frequent mvh score
		double mvhMode;
		for(flat_map<int,int>::iterator itr = mvhScoreDistribution.begin(); itr!= mvhScoreDistribution.end(); ++itr) {
			if((*itr).second==maxValue) {
				mvhMode = (double) (*itr).first;
				break;
			}
		}

		
		// Compute the average and mode of the mzFidelity score distrubtion just like the mvh score 
		// distribution
		double averageMZFidelity = 0.0;
		totalComps = 0.0;
		maxValue = INT_MIN;
		for(flat_map<int,int>::iterator itr = mzFidelityDistribution.begin(); itr!= mzFidelityDistribution.end(); ++itr) {
			if((*itr).first==0) {
				continue;
			}
			averageMZFidelity += ((*itr).second * (*itr).first);
			totalComps += (*itr).second;
			maxValue = maxValue < (*itr).second ? (*itr).second : maxValue;
		}
		averageMZFidelity /= totalComps;

		double mzFidelityMode;
		for(flat_map<int,int>::iterator itr = mzFidelityDistribution.begin(); itr!= mzFidelityDistribution.end(); ++itr) {
			if((*itr).second==maxValue) {
				mzFidelityMode = (double) (*itr).first;
				break;
			}
		}
		double massError = 2.5;
		// For each search result
		/*for( SearchResultSetType::const_reverse_iterator rItr = resultSet.rbegin(); rItr != resultSet.rend(); ++rItr )
		{
			// Init the default values for all delta scores.
			const_cast< SearchResult& >( *rItr ).deltaMVHSeqType = 0.0;
			const_cast< SearchResult& >( *rItr ).deltaMVHSmartSeqType = 0.0;
			const_cast< SearchResult& >( *rItr ).deltaMVHMode = -1.0;
			const_cast< SearchResult& >( *rItr ).deltaMZFidelitySeqType = 0.0;
			const_cast< SearchResult& >( *rItr ).deltaMZFidelitySmartSeqType = 0.0;
			const_cast< SearchResult& >( *rItr ).deltaMZFidelityMode = -1.0;
			const_cast< SearchResult& >( *rItr ).deltaMVHAvg = -1.0;
			const_cast< SearchResult& >( *rItr ).deltaMZFidelityAvg = -1.0;
			const_cast < SearchResult& >(*rItr ).mvhMode = 0.0;
			const_cast < SearchResult& >(*rItr ).mzFidelityMode = 0.0;

			// Check to see if the mvh and mzFidelity scores are above zero.
			bool validMVHScore = rItr->mvh > 0.0 ? true : false;
			bool validMZFidelityScore = rItr->mzFidelity > 0.0 ? true : false;

			// If the mvh score is valid then compute the deltaMVH using the (thisMVH-averageMVH)/thisMVH
			// Also compute the deltaMVH using (thisMVH-modeMVH)/thisMVH.
			if(validMVHScore) {			
				const_cast< SearchResult& >( *rItr ).deltaMVHAvg = (rItr->mvh-averageMVHValue);
				const_cast< SearchResult& >( *rItr ).deltaMVHMode = (rItr->mvh-mvhMode);
				const_cast < SearchResult& >(*rItr ).mvhMode = mvhMode;
			}
			// Compute the deltaMZFidelity values just like the deltaMVH values described above.
			if(validMZFidelityScore) {
				const_cast< SearchResult& >( *rItr ).deltaMZFidelityAvg = (rItr->mzFidelity-averageMZFidelity);
				const_cast< SearchResult& >( *rItr ).deltaMZFidelityMode = (rItr->mzFidelity-mzFidelityMode);
			}
			
			
			// Compute the smart sequest type deltaMVH value as (thisMVH-nextBestMVH)/thisMVH
			// nextBestMVH is the next lowest MVH that matches to a different sequence. Please
			// note that this treats the peptide sequences that match with same MVH score as
			// same. It also treats a peptide sequence with ambiguous interpretations
			// as same.
			SearchResultSetType::const_reverse_iterator reverIter = rItr;
			// Get the current sequence
			string currentPep = rItr->sequence();
			// Go down the list of results and locate the next best result that doesn't have the
			// same MVH score and the same peptide sequence.
			//while(validMVHScore && reverIter!=resultSet.begin() && (currentPep.compare(reverIter->sequence())==0 || rItr->mvh==reverIter->mvh)) {
			while(validMVHScore && reverIter!=resultSet.rend() && (isIsobaric(static_cast< const DigestedPeptide& >(*rItr),static_cast< const DigestedPeptide& >(*reverIter),massError)==true || rItr->mvh==reverIter->mvh)) {
				++reverIter;
			}
			// Compute the deltaMVH using the located result
			if(reverIter!=resultSet.rend() && validMVHScore && (currentPep.compare(reverIter->sequence())!=0 && rItr->mvh!=reverIter->mvh) && reverIter->mvh > 0.0) {
				const_cast< SearchResult& >( *rItr ).deltaMVHSmartSeqType = (rItr->mvh-reverIter->mvh);
			}

			// Compute the sequest type deltaMVH as (thisMVH-nextBestMVH)/thisMVH.
			// nextBestMVH in this context is the next lowest MVH value regardless of the
			// sequence it matched to. Please note that this treats peptides with ambiguous
			// modification interpretations as different entities.
			reverIter = rItr;
			// Locate the next result with the lowest score
			while(validMVHScore && reverIter!=resultSet.rend() && rItr->mvh==reverIter->mvh) {
				++reverIter;
			}
			// Compute the deltaMVH
			if(reverIter!=resultSet.rend() && validMVHScore && rItr->mvh!=reverIter->mvh && reverIter->mvh > 0.0) {
				const_cast< SearchResult& >( *rItr ).deltaMVHSeqType = (rItr->mvh-reverIter->mvh);
			}

			// Compute the smart sequest type deltaMZFidelity value as (thisMZFidelity-nextBestMZFidelity)/thisMZFidelity
			// nextBestMZFidelity is the next lowest MZFidelity that matches to a different sequence. Please
			// note that this treats the peptide sequences that match with same MZFidelity score as
			// same. It also treats a peptide sequence with ambiguous interpretations
			// as same.
			reverIter = rItr;
			// Go down the list of results and locate the next best result that doesn't have the
			// same MVH score and the same peptide sequence.
			while(validMZFidelityScore && reverIter!=resultSet.rend() && (isIsobaric(static_cast< const DigestedPeptide& >(*rItr),static_cast< const DigestedPeptide& >(*reverIter),massError)==true || rItr->mzFidelity==reverIter->mzFidelity)) {
				++reverIter;
			}
			// Compute the deltaMVH using the located result
			if(reverIter!=resultSet.rend() && validMZFidelityScore && (currentPep.compare(reverIter->sequence())!=0 && rItr->mzFidelity!=reverIter->mzFidelity) && reverIter->mzFidelity > 0.0) {
				const_cast< SearchResult& >( *rItr ).deltaMZFidelitySmartSeqType = (rItr->mzFidelity-reverIter->mzFidelity);
			}

			// Compute the sequest type deltaMZFidelity as (thisMZFidelity-nextBestMZFidelity)/thisMZFidelity.
			// nextBestMZFidelity in this context is the next lowest MZFidelity value regardless of the
			// sequence it matched to. Please note that this treats peptides with ambiguous
			// modification interpretations as different entities.
			reverIter = rItr;
			// Locate the next result with the lowest score
			while(validMZFidelityScore && reverIter!=resultSet.rend() && rItr->mzFidelity==reverIter->mzFidelity) {
				++reverIter;
			}
			// Compute the deltaMZFidelity
			if(reverIter!=resultSet.rend() && validMZFidelityScore && rItr->mzFidelity!=reverIter->mzFidelity && reverIter->mzFidelity > 0.0) {
				const_cast< SearchResult& >( *rItr ).deltaMZFidelitySeqType = (rItr->mzFidelity-reverIter->mzFidelity);
			}
		}*/
	}

    void Spectrum::ScoreSequenceVsSpectrum( SearchResult& result, const string& seq, const vector< double >& seqIons )
    { TRACER_METHOD_START("Spectrum::ScoreSequenceVsSpectrum"); TRACER(result, READ, HEAP, "where to store resulting scores"); TRACER(seq, READ, HEAP, "Sequence to match"); TRACER(seqIons, READ, HEAP, "Sequence ions");
        PeakData::iterator peakItr;
		MvIntKey mzFidelityKey;
        //MvIntKey& mvhKey = result.key;
        MvIntKey mvhKey;

		mvhKey.clear();
		mvhKey.resize( g_rtConfig->NumIntensityClasses+1, 0 ); TRACER(mvhKey, WRITE, STACK, "mvhKey");
		mzFidelityKey.resize( g_rtConfig->NumMzFidelityClasses+1, 0 ); TRACER(mzFidelityKey, WRITE, STACK, "mzFidelityKey");
		result.mvh = 0.0;
		result.mzFidelity = 0.0;
		//result.mzSSE = 0.0;
		//result.newMZFidelity = 0.0;
		//result.mzMAE = 0.0;
		result.matchedIons.clear();
		TRACER(result, WRITE, HEAP, "scores cleaned");  TRACER(peakData, READ, HEAP, "spectrum peak data to match");
		TRACER(mzFidelityThresholds, READ, HEAP, "mz fidelity m/z fragment error thresholds");
		START_PROFILER(6);
		int totalPeaks = (int) seqIons.size(); TRACER(totalPeaks, WRITE, STACK, "number of sequence ions");
		TRACER(g_rtConfig->FragmentMzTolerance, READ, HEAP, "fragment m/z tolerance"); TRACER_OP_START("match sequence ions");
        for( size_t j=0; j < seqIons.size(); ++j )
	    { TRACER_OP_START("process j-th sequence ion"); TRACER(j, WRITE, STACK, "ion index");
		    // skip theoretical ions outside the scan range of the spectrum
		    if( seqIons[j] < mzLowerBound ||
			    seqIons[j] > mzUpperBound )
		    {
			    --totalPeaks; // one less ion to consider because it's out of the scan range
			    TRACER(j, READ, STACK, "peak not within [mzLowerBound, mzUpperBound]"); TRACER(totalPeaks, WRITE, STACK, "total peaks lowered"); TRACER_OP_END("process j-th sequence ion"); continue;
		    }


		    START_PROFILER(7);
		    // Find the fragment ion peak. Consider the fragment ion charge state while setting the
		    // mass window for the fragment ion lookup.
		    peakItr = peakData.findNear( seqIons[j], g_rtConfig->FragmentMzTolerance ); 
            TRACER(seqIons[j], READ, HEAP, "theoretical m/z to match");
		    STOP_PROFILER(7);
			TRACER(mzFidelityThresholds, READ, HEAP, "the m/z error thresholds for mzFidelity");
		    // If a peak was found, increment the sequenceInstance's ion correlation triplet
		    if( peakItr != peakData.end() && peakItr->second.intenClass > 0 )
		    {   TRACER(peakItr->first, READ, STACK, "matching peak m/z"); TRACER(peakItr->second.intenClass, READ, HEAP, "matching peak intensity class");
			    double mzError = fabs( peakItr->first - seqIons[j] );
			    ++mvhKey[ peakItr->second.intenClass-1 ];
			    //result.mzSSE += pow( mzError, 2.0 );
			    //result.mzMAE += mzError;
			    ++mzFidelityKey[ ClassifyError( mzError, mzFidelityThresholds ) ];
			    //int mzFidelityClass = ClassifyError( mzError, g_rtConfig->massErrors );
			    //result.newMZFidelity += g_rtConfig->mzFidelityLods[mzFidelityClass];
			    //result.matchedIons.push_back(peakItr->first);
		    } else
		    {   TRACER(j, READ, STACK, "this ion did not match anything");
			    ++mvhKey[ g_rtConfig->NumIntensityClasses ];
			    //result.mzSSE += pow( 2.0 * g_rtConfig->FragmentMzTolerance, 2.0 );
			    //result.mzMAE += 2.0 * g_rtConfig->FragmentMzTolerance;
			    ++mzFidelityKey[ g_rtConfig->NumMzFidelityClasses ];
		    }
			 
	    TRACER_OP_END("process j-th sequence ion");} TRACER(mvhKey, WRITE, STACK, "mvhKey"); TRACER(mzFidelityKey, WRITE, STACK, "mzFidelityKey"); TRACER_OP_END("match sequence ions");
		STOP_PROFILER(6);

		//result.mzSSE /= totalPeaks;
		//result.mzMAE /= totalPeaks;

		// Convert the new mzFidelity score into normal domain.
		//result.newMZFidelity = exp(result.newMZFidelity);

		double mvh = 0.0; TRACER(mvh, WRITE, STACK, "mvh");
		
        result.fragmentsUnmatched = mvhKey.back(); TRACER(result.fragmentsUnmatched , WRITE, STACK, "unmatched fragments");
		TRACER(g_rtConfig->MinMatchedFragments, READ, HEAP, "minimum of fragments that must match");
		START_PROFILER(8);
		if( result.fragmentsUnmatched != totalPeaks )
		{
            int fragmentsPredicted = accumulate( mvhKey.begin(), mvhKey.end(), 0 ); TRACER(fragmentsPredicted , WRITE, STACK, "total predicted fragments");
			result.fragmentsMatched = fragmentsPredicted - result.fragmentsUnmatched; TRACER(result.fragmentsMatched , WRITE, HEAP, "matched fragments");

            if( result.fragmentsMatched >= g_rtConfig->MinMatchedFragments )
            {
			    //int numHits = accumulate( intenClassCounts.begin(), intenClassCounts.end()-1, 0 );
			    int numVoids = intenClassCounts.back(); TRACER(numVoids, WRITE, STACK, "number of void spaces");
			    int totalPeakBins = numVoids + peakCount; TRACER(totalPeakBins, WRITE, STACK, "total peak bins = voids + peaks");
				TRACER(intenClassCounts, READ, HEAP, "intensity class counts");
			    for( size_t i=0; i < intenClassCounts.size(); ++i ) {
				    mvh += lnCombin( intenClassCounts[i], mvhKey[i] );
			    }
			    mvh -= lnCombin( totalPeakBins, fragmentsPredicted );

				result.mvh = -mvh; TRACER(result.mvh, WRITE, HEAP, "resulting mvh");


			    int N;
			    double sum1 = 0, sum2 = 0;
				int totalPeakSpace = numVoids + fragmentsPredicted; TRACER(totalPeakSpace, WRITE, STACK, "total peak space = voids + # of fragments");
			    double pHits = (double) fragmentsPredicted / (double) totalPeakSpace; TRACER(pHits, WRITE, STACK, "probability of a hit");
			    double pMisses = 1.0 - pHits; TRACER(pMisses, WRITE, STACK, "probability of a miss");

			    N = accumulate( mzFidelityKey.begin(), mzFidelityKey.end(), 0 ); TRACER(N, WRITE, STACK, "sum of all mzFidelityKey values");
			    int p = 0; TRACER(g_rtConfig->minMzFidelityClassCount, READ, HEAP, "minimum mzFidelity class count");

			    //cout << id << ": " << mzFidelityKey << endl;

			    //if( id == 2347 ) cout << pHits << " " << totalPeakSpace << " " << peakData.size() << endl;
			    for( int i=0; i < g_rtConfig->NumMzFidelityClasses; ++i )
			    {
				    p = 1 << i;
				    double pKey = pHits * ( (double) p / (double) g_rtConfig->minMzFidelityClassCount );
				    //if( id == 2347 ) cout << " " << pKey << " " << mzFidelityKey[i] << endl;
				    sum1 += log( pow( pKey, mzFidelityKey[i] ) );
				    sum2 += g_lnFactorialTable[ mzFidelityKey[i] ];
			    }
			    sum1 += log( pow( pMisses, mzFidelityKey.back() ) );
			    sum2 += g_lnFactorialTable[ mzFidelityKey.back() ];
			    result.mzFidelity = -1.0 * double( ( g_lnFactorialTable[ N ] - sum2 ) + sum1 ); TRACER(result.mzFidelity, WRITE, HEAP, "resulting mzFidelity score");
            }
		}

		STOP_PROFILER(8);
	TRACER_METHOD_END("Spectrum::ScoreSequenceVsSpectrum"); }
}
}
