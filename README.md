fw2csv
======

Fixed width data files are probably the most annoying data to import into database.
This utility allows user to insert user-defined column separators into user-defided 
positions of flat file table given in fixed-width format.

I am using this utility to import astronomical catalogs provided as flat tables at CDS ftp server;

Build:
 $ make all [cc={icc|gcc}]
 
Install:
 $ make install [prefix=/path/to/your/bin/directory]

Example of usage: 

 $ gzip -dc table.dat.gz | \
	  fw2csv --stops=4,10,11,20,30,36 -d '|' -header 1 | \
	    psql wsdb -c "copy mytable from stdin with csv delimiter '|'" 
	
For the list of available options call 
	$ fw2csv --help 


A. Myznikov
Aug 23 2012
