# STOL(*S*afe *T*ransfer *O*n*L*ine)

This is a program for securely transferring files over the internet. This program was written almost entirely by ChatGPT.
The folder "process" contains a general description of how I communicated with ChatGPT and New Bing.

## Pre-Send Processing

1. Select multiple files and drag them onto send.exe.
2. Enter the password when prompted.
3. The program will call the local 7zip compression software to encrypt and compress the selected files into a 7z file.
4. The program will then randomly select 10-30 bytes in the compressed package and change them all to "0xFF".
5. Record the positions of these bytes and their original values in modified_bits.txt in the same directory as the compressed package. Each line corresponds to the modification status of a byte, with the format "position before modification value".

## Post-Receive Processing

1. Select a 7z compressed package and drag it onto recv.exe.
2. Enter the password when prompted.
3. The program will first repair the modified bytes in the compressed package according to modified_bits.txt in its same directory.
4. Then call local 7zip compression software to decompress the selected compressed package.