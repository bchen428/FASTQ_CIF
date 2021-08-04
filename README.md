# FASTQ_CIF
Takes output from Illumina MiSeq (FASTQ, CIF, and LOCS files for a given run) and matches them all together.

# Requirements
Tested/developed in Ubuntu 20.04, compiled with gcc. Uses getline() so likely will not work in any operating system that does not support it.

# Command
./sequenceintensity SEQFILE COORDFILE locsdir cifdir index

For instance, to get the intensity of nucleotide #4 (the 'A' in CCCATTGG) where the sequences are stored in 'sequencefile.txt', the coordinates are stored in 'coordinatefile.txt', the LOCS files are stored in directory 'LOCS', and the CIF files are stored in directory 'CIF':

./sequenceintensity sequencefile.txt coordinatefile.txt LOCS CIF 4

# Instructions

Some preprocessing to the FASTQ files are required. SEQFILE and COORDFILE are generated from the FASTQ files. SEQFILE contains only the sequences (lines numbered 4n+2) while COORDFILE contains only the descriptors (lines numbered 4n+1). This script assumes that where line number is represented by i, for sequence Si in SEQFILE (S1 ... Sn) that the coordinate containing descriptor Ci in COORDFILE (C1 ... Cn), Ci is the descriptor for Si.

It should be fairly trivial to write some code to export lines 4n+1 and 4n+2 to separate files, but if necessary I may upload a quick snippet of code for that process.

# Testing

Unfortunately, I am unable to upload the files we used for testing this (as the .locs and .cif files massively exceed the upload limit of github).
If you require these to confirm the effectiveness of this code, please contact: bchen@aptamatrix.com
