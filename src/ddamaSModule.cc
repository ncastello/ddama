/*
 * ddamaSModule.cc
 *
 * @author Nuria Castello-Mor
 */


#include "ddamaSModule.h"

// dqm4hep headers
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMModuleApi.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMPlugin.h"

// ROOT headers
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

//#include <ctime>
//#include <cstdlib>
//#include <limits>
#include<csignal>

namespace dqm4hep
{

    // module plugin declaration
    DQM_PLUGIN_DECL( ddamaSModule , "ddamaSModule" )

    ddamaSModule::ddamaSModule() :
        DQMStandaloneModule(),
        _meH2RawImage(nullptr),
        _meH1PixelDist(nullptr),
        _meH1PedestalDist(nullptr),
        _inputFile(nullptr),
        _h2RawImage(nullptr),
        _h1PixelDist(nullptr),
        _h1PedestalDist(nullptr)
    {
        setVersion(0, 1, 0);
    }


    ddamaSModule::~ddamaSModule()
    {
    }


    StatusCode ddamaSModule::readSettings(const TiXmlHandle /*xmlHandle*/)
    {
        /*LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- readSettings()" );

	    m_min = -400;
	    RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND,
                !=, DQMXmlHelper::readParameterValue(xmlHandle,
			    "Min", m_min));

	    m_max = 400;
	    RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND,
                !=, DQMXmlHelper::readParameterValue(xmlHandle,
			    "Max", m_max));

	    // max must be greater than min ...
	        if(m_min >= m_max)
		return STATUS_CODE_INVALID_PARAMETER;
        */
        return STATUS_CODE_SUCCESS;
    }


    StatusCode ddamaSModule::startOfCycle()
    {
        // Initialization input ROOT file(s)
        // XXX hardcoded file name, not checked if file or histos exists, ...
        _inputFile = std::unique_ptr<TFile>(new TFile("/damic/working/d44_snolab_Int-800_Exp-30000_3337_1.root"));

        _h2RawImage = static_cast<TH2F*>(_inputFile->Get("image_raw"));
        _h1PixelDist = static_cast<TH1F*>(_inputFile->Get("pixel_distribution"));
        _h1PedestalDist = static_cast<TH1F*>(_inputFile->Get("pedestal_fit_results/pedestal_mean_col"));

        _meH1PixelDist->setToPublish(true);
        _meH1PedestalDist->setToPublish(true);
        _meH2RawImage->setToPublish(true);

	    return STATUS_CODE_SUCCESS;
    }

    StatusCode ddamaSModule::endOfCycle()
    {
        _inputFile->Close();
        /*std::raise(SIGINT);
         */
	    return STATUS_CODE_SUCCESS;
    }

    StatusCode ddamaSModule::initModule()
    {
    	LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- init()" );

    	DQMModuleApi::cd(this);

        DQMModuleApi::bookRealHistogram1D(this, _meH1PedestalDist, "PEDESTAL_COLUMN", \
                "Column Pedestal distribution",8544, 0.0, 8544.0 );
    	_meH1PedestalDist->setDescription("Pedestal mean column distribution");

        DQMModuleApi::bookRealHistogram1D(this, _meH1PixelDist, "PIXEL_CHARGE", \
                "Pixel charge distribution", 109317,-54658.0, 54659.0);
    	_meH1PixelDist->setDescription("Charge Pixel distribution");

        DQMModuleApi::bookRealHistogram2D(this, _meH2RawImage, "RAW_IMAGE", \
                "Raw Image", 8544, 0.0, 8544.0, 193, 0.0, 193.0 );
    	_meH2RawImage->setDescription("Raw image");


    	// print directory structure of monitor element storage in console
    	DQMModuleApi::ls(this, true);

    	return STATUS_CODE_SUCCESS;
    }

    StatusCode ddamaSModule::endModule()
    {
    	LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- end()" );
    	return STATUS_CODE_SUCCESS;
    }


    StatusCode ddamaSModule::process()
    {
        // dark current: b_pedestal - b_overscan, both comming from the fit to
        // the pedestal_mean_col histogram
        for(unsigned int i=1; i<_h1PedestalDist->GetNbinsX()+1;++i)
        {
    	    _meH1PedestalDist->get<TH1F>()->SetBinContent(i,_h1PedestalDist->GetBinContent(i));
        }
        _meH1PedestalDist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PedestalDist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PedestalDist->get<TH1F>()->SetMarkerColor(12);
        _meH1PedestalDist->get<TH1F>()->SetLineColor(12);
        _meH1PedestalDist->get<TH1F>()->SetLineWidth(1);
        _meH1PedestalDist->setDrawOption("E");

        for(unsigned int i=1; i<_h1PixelDist->GetNbinsX()+1;++i)
        {
    	    _meH1PixelDist->get<TH1F>()->SetBinContent(i,_h1PixelDist->GetBinContent(i));
        }
        _meH1PixelDist->set<TH1F>() = _h1PixelDist.Clone()
        _meH1PixelDist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PixelDist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PixelDist->get<TH1F>()->SetMarkerColor(12);
        _meH1PixelDist->get<TH1F>()->SetLineColor(12);
        _meH1PixelDist->get<TH1F>()->SetLineWidth(1);
        _meH1PixelDist->setDrawOption("E");
        _meH1PixelDist->get<TH1F>()->GetXaxis()->SetRangeUser(-150., 150.);

        //for(unsigned int i=1; i<_h2RawImage->GetNbinsX()+1;++i)
        //{
        //    for(unsigned int j=0; i<_h2RawImage->GetNbinsY()+1;++j)
        //    {
        //        _meH2RawImage->get<TH2F>()->SetBinContent(i,j,_h2RawImage->GetBinContent(i,j));
        //    }
        //}

        return STATUS_CODE_SUCCESS;
    }

}

