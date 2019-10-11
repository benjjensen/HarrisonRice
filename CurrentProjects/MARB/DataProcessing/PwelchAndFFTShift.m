function processedData = PwelchAndFFTShift(rawData, Nfft)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    pwelchOutput = pwelch(rawData,boxcar(Nfft),0,Nfft,'twosided');

    [~, frames] = size(pwelchOutput);
    shiftedOutput = zeros(Nfft,frames);
    for i = 1:frames
        shiftedOutput(:,i) = fftshift(pwelchOutput(:,i));
    end
    processedData = shiftedOutput;
end

