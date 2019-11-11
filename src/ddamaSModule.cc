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

#include<csignal>

namespace dqm4hep
{

    // module plugin declaration
    DQM_PLUGIN_DECL( ddamaSModule , "ddamaSModule" )

    ddamaSModule::ddamaSModule() :
        DQMStandaloneModule(),
        _meH2RawImage(nullptr),
        _meH1PixelDist(nullptr),
        _meH1PedMeanDist(nullptr),
        _meH1PedSigmaDist(nullptr),
        _meH1PedChi2Dist(nullptr),
        _inputFile(nullptr),
        _h2RawImage(nullptr),
        _h1PixelDist(nullptr),
        _h1PedMeanDist(nullptr),
        _h1PedSigmaDist(nullptr),
        _h1PedChi2Dist(nullptr)
    {
        setVersion(0, 1, 0);
    }


    ddamaSModule::~ddamaSModule()
    {
    }


    StatusCode ddamaSModule::readSettings(const TiXmlHandle xmlHandle)
    {
        LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- readSettings()" );

        // XML inputs for the ME
        RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, DQMXmlHelper::readParameterValue(xmlHandle,"RunID", _runID));

        // XML inputs for the QT
        // testQuality1
        RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, DQMModuleApi::registerQualityTestFactory(this,
                    "MeanWithinExpectedTest", new DQMMeanWithinExpectedTest::Factory()));
        RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, DQMXmlHelper::createQualityTest(this, xmlHandle,
                    "MeanAround0Short"));
        // testQuality2
        RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, DQMModuleApi::registerQualityTestFactory(this,
                    "Chi2FitFunctionTest", new DQMChi2FitFunctionTest::Factory()));
        RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, DQMXmlHelper::createQualityTest(this, xmlHandle,
                    "Chi2FittingGauss"));

        return STATUS_CODE_SUCCESS;
    }


    StatusCode ddamaSModule::startOfCycle()
    {
        // Initialization input ROOT file(s)
        // XXX hardcoded file name, not checked if file or histos exists, ...

        const char * rootfilename = "d44_snolab_Int-800_Exp-30000_3337_1.root";
        _inputFile = std::unique_ptr<TFile>(new TFile(rootfilename));

        _h2RawImage = static_cast<TH2F*>(_inputFile->Get("image_raw"));
        _h1PixelDist = static_cast<TH1F*>(_inputFile->Get("pixel_distribution"));
        _h1PedMeanDist = static_cast<TH1F*>(_inputFile->Get("pedestal_fit_results/pedestal_mean_col"));
        _h1PedSigmaDist = static_cast<TH1F*>(_inputFile->Get("pedestal_fit_results/pedestal_sigma_col"));
        _h1PedChi2Dist = static_cast<TH1F*>(_inputFile->Get("pedestal_fit_results/pedestal_chi2_ndf_col"));

        _meH1PixelDist->setToPublish(true);
        _meH1PedMeanDist->setToPublish(true);
        _meH1PedSigmaDist->setToPublish(true);
        _meH1PedChi2Dist->setToPublish(true);
        _meH2RawImage->setToPublish(true);

	    return STATUS_CODE_SUCCESS;
    }

    StatusCode ddamaSModule::endOfCycle()
    {
        LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- endOfCycle()" );

        _inputFile->Close();
        // XXX To stop running DDAMA
        /*std::raise(SIGINT);
         */
        // run all quality tests on all
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::runQualityTests(this));

	    return STATUS_CODE_SUCCESS;
    }

    StatusCode ddamaSModule::initModule()
    {
    	LOG4CXX_INFO( dqmMainLogger , "Module : " << getName() << " -- init()" );

    	DQMModuleApi::cd(this);

        // ME::PedMean Column distribution
        DQMModuleApi::bookRealHistogram1D(this, _meH1PedMeanDist, "FIT_PEDESTAL_COL_MEAN",
                "mean-column Pedestal distribution",8544, 0.0, 8544.0 );
    	_meH1PedMeanDist->setDescription("Mean of the fitted column pedestal distribution");
        _meH1PedMeanDist->setResetPolicy(END_OF_CYCLE_RESET_POLICY);

        // ME::PedSigma Column distribution
        DQMModuleApi::bookRealHistogram1D(this, _meH1PedSigmaDist, "FIT_PEDESTAL_COL_SIGMA",
                "sigma-column Pedestal distribution",8544, 0.0, 8544.0 );
    	_meH1PedSigmaDist->setDescription("Sigma of the fitted column pedestal distribution");
        _meH1PedSigmaDist->setResetPolicy(END_OF_CYCLE_RESET_POLICY);

        // ME::PedChi2 Column distribution
        DQMModuleApi::bookRealHistogram1D(this, _meH1PedChi2Dist, "FIT_PEDESTAL_COL_CHI",
                "chi-squared column Pedestal distribution",8544, 0.0, 8544.0 );
    	_meH1PedChi2Dist->setDescription("chi-squared of the fitted column pedestal distribution");
        _meH1PedChi2Dist->setResetPolicy(END_OF_CYCLE_RESET_POLICY);

        // ME::Pixel distribution
        DQMModuleApi::bookRealHistogram1D(this, _meH1PixelDist, "PIXEL_CHARGE",
                "Pixel charge distribution", 109317,-54658.0, 54659.0);
    	_meH1PixelDist->setDescription("Pixel value distribution in blanks images (white noise)"
                " and in 8 hours exposure (white noise, leakage current and signal) when operating at "
                "T~140K");
        _meH1PixelDist->setResetPolicy(END_OF_CYCLE_RESET_POLICY);
        // -----> Include the QT to the ME
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::addQualityTest(this,
                                _meH1PixelDist, "MeanAround0Short"));
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMModuleApi::addQualityTest(this,
                                _meH1PixelDist, "Chi2FittingGauss"));


        // ME::Raw Image
        DQMModuleApi::bookRealHistogram2D(this, _meH2RawImage, "RAW_IMAGE",
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
        for(unsigned int i=1; i<_h1PedMeanDist->GetNbinsX()+1;++i)
        {
    	    _meH1PedMeanDist->get<TH1F>()->SetBinContent(i,_h1PedMeanDist->GetBinContent(i));
        }
        _meH1PedMeanDist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PedMeanDist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PedMeanDist->get<TH1F>()->SetMarkerColor(12);
        _meH1PedMeanDist->get<TH1F>()->SetLineColor(12);
        _meH1PedMeanDist->get<TH1F>()->SetLineWidth(1);
        _meH1PedMeanDist->setDrawOption("E");

        for(unsigned int i=1; i<_h1PedSigmaDist->GetNbinsX()+1;++i)
        {
    	    _meH1PedSigmaDist->get<TH1F>()->SetBinContent(i,_h1PedSigmaDist->GetBinContent(i));
        }
        _meH1PedSigmaDist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PedSigmaDist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PedSigmaDist->get<TH1F>()->SetMarkerColor(12);
        _meH1PedSigmaDist->get<TH1F>()->SetLineColor(12);
        _meH1PedSigmaDist->get<TH1F>()->SetLineWidth(1);
        _meH1PedSigmaDist->setDrawOption("E");

        for(unsigned int i=1; i<_h1PedChi2Dist->GetNbinsX()+1;++i)
        {
    	    _meH1PedChi2Dist->get<TH1F>()->SetBinContent(i,_h1PedChi2Dist->GetBinContent(i));
        }
        _meH1PedChi2Dist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PedChi2Dist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PedChi2Dist->get<TH1F>()->SetMarkerColor(12);
        _meH1PedChi2Dist->get<TH1F>()->SetLineColor(12);
        _meH1PedChi2Dist->get<TH1F>()->SetLineWidth(1);
        _meH1PixelDist->get<TH1F>()->GetXaxis()->SetRangeUser(0.0, 2.);
        _meH1PedChi2Dist->setDrawOption("E");

        for(unsigned int i=1; i<_h1PixelDist->GetNbinsX()+1;++i)
        {
    	    _meH1PixelDist->get<TH1F>()->SetBinContent(i,_h1PixelDist->GetBinContent(i));
        }
        _meH1PixelDist->get<TH1F>()->SetMarkerStyle(2);
        _meH1PixelDist->get<TH1F>()->SetMarkerSize(0.5);
        _meH1PixelDist->get<TH1F>()->SetMarkerColor(12);
        _meH1PixelDist->get<TH1F>()->SetLineColor(12);
        _meH1PixelDist->get<TH1F>()->SetLineWidth(1);
        _meH1PixelDist->get<TH1F>()->GetXaxis()->SetRangeUser(-150., 150.);
        _meH1PixelDist->setDrawOption("E");

        return STATUS_CODE_SUCCESS;
    }

}

