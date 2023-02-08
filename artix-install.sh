# Install Script for Arch Based Systems like Arch,Artix ....
# Created on # Wed Feb  8 09:08:48 AM CET 2023

#autoreconf -if && ./configure --prefix=/usr && make && sudo make install
cd final && make clean && sed -i 's/local\///' Makefile.gcc && make debug -f Makefile.gcc && sudo make install -f Makefile.gcc
cd ../examples && make clean && sed -i 's/local\///' Makefile.gcc && make debug -f Makefile.gcc
echo "Install Finished"


