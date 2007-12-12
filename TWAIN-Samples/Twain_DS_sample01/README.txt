TWAIN Data Source [DS]

When finished this will be a TWAIN version 2.0 of the specification compliant
data source.  Instead of interfacing with a real scanner, it talks to a 
virtual scanner.  The virtual scanner acquires its image from a file on disk.

This code can be used as a sample for building your own data source. One way 
to start would be to inherit the CTWAINDS_Base class and implement the pure 
virtual functions.

There is a file, TWAIN1.9_compliancy_checklist.txt, that contains everything
that a source is required to support in order to be TWAIN compliant.

The FreeImage library is used by the virtual scanner to perform transforms on
the base image to meet the negotiated capabilities. You will need to have the 
FreeImage files installed on your system for this source to work properly.

FreeImage website: http://freeimage.sourceforge.net

The DS reads an environment variable, CAP_XFERCOUNT, to get the number of
times to send an image. By default only 1 is sent. Setting this to -1 will
make the source choose a random number.

The source code is documented using the Doxygen documentation system.

Please refer to the TWAIN spec from http://www.TWAIN.org for further details
on TWAIN.


Installation
------------

[Linux]
- install qmake then run it to generate makefiles
- make
- copy images/TWAIN_logo.png to /usr/local/lib/twain
- copy src/TWAINDS_Sample01.ds to /usr/local/lib/twain

notes: it is a future goal to have "make install" do the above copies.

[Windows]
- QMake is used to generate the makefiles. You can get a copy of this free in any
QT Open Source distribution.  Please see http://www.trolltech.com for more info.
- copy the TWAIN_logo.png and TWAINDS_Sample[32|64].ds to /Windows/twain_[32|64] 
directory.  
- Or use the provided Visual Studio project files.



- original: fredh@jflinc.com, May 14, 2005
- updated:  fredh@jflin.com, Nov 23, 2007
- updated:  jimw@jflin.com, Dec 10, 2007
