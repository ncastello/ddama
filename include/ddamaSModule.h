/*  ddamaSModule.h
 *
 *  DQM4HEP DAMIC standalone module to create the elements to be monotored
 *  Set the environment variable
 *      DQM4HEP_PLUGIN_DLL=/damic/ddama/lib/ddamaSModule.so
 *  and run the module with:
 *      dqm4hep_start_standalone_module
 *
 * @author Nuria Castello-Mor
 */

#ifndef DDAMASMODULE_H
#define DDAMASMODULE_H

#include "dqm4hep/DQMStandaloneModule.h"

#include<memory>

class TFile;
class TH1F;
class TH2F;

namespace dqm4hep
{
    class DQMMonitorElement;

    class ddamaSModule : public DQMStandaloneModule
    {
        public:
            ddamaSModule();
	        ~ddamaSModule();

        	StatusCode initModule();
	        StatusCode startOfCycle();
    	    StatusCode endOfCycle();
    	    StatusCode readSettings(const TiXmlHandle xmlHandle);
        	StatusCode process();
	        StatusCode endModule();

        protected:
            DQMMonitorElementPtr _meH2RawImage;
	        DQMMonitorElementPtr _meH1PixelDist;
            DQMMonitorElementPtr _meH1PedestalDist;

        private:
            std::unique_ptr<TFile> _inputFile;
            TH2F * _h2RawImage;
            TH1F * _h1PixelDist;
            TH1F * _h1PedestalDist;

            int _extid;

    };
}

#endif
