TWAIN Data Source [DS]

When finished this will be a TWAIN version 2.5 of the specification compliant
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

[Linux] (not recently tested!!!)
- install QT 5.9.9 then run it to generate makefiles
- make
- copy images/TWAIN_logo.png to /usr/local/lib/twain/sample2
- copy src/TWAINDS_Sample01.ds to /usr/local/lib/twain/sample2

notes: it is a future goal to have "make install" do the above copies.

[Windows]
- install QT 5.9.9 SDK for windows
- Set environment variable QTDIR to C:\Qt\Qt5.9.9\5.9.9\msvc2017_64
- Use the provided Visual Studio project files.

[MAC] (not recently tested!!!)
- install QT 5.9.9 SDK for MAC
- install Xcode
- build the project
- build will automatically copy DS bundle to /Library/Image Capture/TWAIN Data Sources/


- original: fredh@jflinc.com, May 14, 2005
- updated:  fredh@jflin.com, Nov 23, 2007
- updated:  jimw@jflin.com, Dec 10, 2007
- updated:  mihailm@jflin.com, Dec 23, 2009
- updated:  mihailm@jflin.com, Dec 30, 2009
- updated:  mihailm@jflin.com, Apr 20, 2010
- updated:  mark.mclaughlin@kodakalaris.com, Apr 30, 2020