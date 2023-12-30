Welcome to Baken 0.5.4

The idea behind baken is to allow for the visualisation of European VHF/UHF
and microwave beacons on a map of Europe. As input it takes the file
produced by G3UUT the Region 1 Beacon Co-ordinator, and attempts to extract
the beacon data from it. Please note that you need to the text version of
his file, not the Word version.

Most of the program is simple of understand. At any given time the map shows
the beacons for any one band from 50 MHz to 47 GHz, including the wonderful
70 MHz, a band that I miss. By moving the mouse to a square and clicking the
left mouse button, another window opens displaying the beacons in that
square in more detail, and any cities to be found lying about. By moving the
mouse near to a beacon (within 20kms to be precise), information about the
beacon is displayed in a window below the map.

There is an option to display a list of all the beacons in the current band,
sorted by frequency, with their frequencies, callsigns, locators and their
bearings and distances.

The new program "iaru" converts the IARU beacon file which is in comma
separated values format into the format used for this program.

Thats more or less all there is to it.

Please note that using older versions of GTK+ can produce problems. I
recommend that you try and get the latest version that you can. My
development was done with GTK+ version 1.2.2.

Jonathan  G4KLX
