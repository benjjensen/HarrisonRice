This folder contains the data, functions, and other files used to form the 
(XXXXXXXXXXXXXXXX) paper, during the second half of 2019 (August-September ish). 

Data Sources:

       For this project, data was gathered in numerous V2V scenarios, with a 
    transmittervvan and a receiver van driving through various environments. 

    Specifically:

    - Diamond Fork

    - Alpine Loop (pt. 1 - about halfway through the loop, on the AF Canyon side)

    - Alpine Loop (pt. 2 - about 3/4 through the loop, on the AF Canyon  side)

    - Downtown Provo Test Cases:
        
        i.   V2I close lane
        ii.  V2I far lane
        iii. V2V running in opposite directions (Tx on NuSkin side)
        iv.  V2V, Rx immediately behind Tx
        v.   V2V, Rx one car distance behind Tx
        vi.  V2V, Rx half a block behind Tx
        vii. V2V, Rx one block behind Tx


Processing Process:

       The data .dat and -meta.txt files were converted to .mat files using the
    convertMatFiles script (V2VCarlos folder?). Those .mat files were then 
    downsampled to 20 Msps using resampleData.m. From there, the following figures/
    information was calculated:

        Power Spectral Density: Performs a non-parametric manual pwelch on data files
  
            1) Reads in a block of data (n = NFFT)
            2) Windows the data (Blackman window)
            3) Takes the FFT, and then the magnitude squared
            4) Shifts by NFFT/2 and repeats, summing the results
            5) Divides by the total number lf length NFFT blocks 

                (Note that the current method to do this uses a special process
                    described in the file.)

        Average Power per Millisecond:

        Line-Of-Sight:

        Delay Spread:

        CCDF:

Figures:

Data File Names: