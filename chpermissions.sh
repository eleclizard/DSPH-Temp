#!/bin/bash

chmod +x bin/linux/*_linux64

# chmod +x examples/boundcorrection/*/*.sh
chmod +x examples/chrono/*/*.sh
chmod +x examples/inletoutlet/*/*.sh
chmod +x examples/main/*/*.sh
chmod +x examples/motion/*.sh
chmod +x examples/others/*/*.sh
chmod +x examples/wavecoupling/*/*.sh
chmod +x examples/moordyn/*/*.sh
chmod +x examples/mdbc/*/*.sh
chmod +x examples/mphase_liquidgas/*/*.sh
chmod +x examples/mphase_nnewtonian/*/*.sh

# dos2unix examples/boundcorrection/*/*.sh
dos2unix examples/chrono/*/*.sh
dos2unix examples/inletoutlet/*/*.sh
dos2unix examples/main/*/*.sh
dos2unix examples/motion/*.sh
dos2unix examples/others/*/*.sh
dos2unix examples/wavecoupling/*/*.sh
dos2unix examples/moordyn/*/*.sh
dos2unix examples/mdbc/*/*.sh
dos2unix examples/mphase_liquidgas/*/*.sh
dos2unix examples/mphase_nnewtonian/*/*.sh

chmod +x src_extra/ToVTK_v5/bin/*_linux64
chmod +x src_extra/ToVTK_v5/bin/*.sh
dos2unix src_extra/ToVTK_v5/bin/*.sh




