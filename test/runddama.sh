#!/bin/bash

export DIM_DNS_NODE="localhost"
export DIM_HOST_NODE="localhost"

dns > dns.log 2>&1 &

sudo /damic/dqm4hep/bin/dimjc_daemon &

#dqm4hep_start_run_control_server -r ddamaRunControl -k dama &
#dqm4hep_start_run_control_interface -r ddamaRunControl &

dqm4hep_start_monitor_element_collector -c damic_me_collector &

dqm4hep_start_dqm_monitor_gui -f ddamaMonitorGUI.xml &

dqm4hep_start_standalone_module -f ddamaSModule.xml

