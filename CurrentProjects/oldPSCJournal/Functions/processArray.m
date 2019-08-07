function [processedArray_linear, processedArray_db] = processArray(dataArray, samples)
%processArray: Pwelches and FFTs a data array
%   Takes in an array of data, and the number of samples, and then uses a
%   two-sided boxcar pwelch to process the data. Returns the processed data
%   in linear or dB.
%   Arguments:
%     dataArray (string) - array of data to be processed
%     samples (int) - Number of samples used to generate the array

    Nfft = 128;
    a = pwelch(dataArray,boxcar(Nfft),0,Nfft,'twosided');

        % FFT shift fixes the ordering 
    for runs = 1:samples
        afft(:,runs) = fftshift(a(:,runs));
    end

    processedArray_linear = afft;

    adb = 10*log10(abs(afft));

    processedArray_db = adb;


end

