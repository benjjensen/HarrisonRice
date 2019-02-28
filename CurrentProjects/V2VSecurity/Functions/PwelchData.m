function [result, mid32carriers] = PwelchData(dataArray, numSamplesPerFrame, indB)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[sizex, sizey, sizez] = size(dataArray);
Nfft = 64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
y = 1;
while (x < sizey) % Run to the end of the dataArray
    z = x + numSamplesPerFrame - 1;
    if (z > sizey)
        z = sizey;
    end
    YY = pwelch(dataArray(x:z),boxcar(Nfft),0,Nfft,'twosided');
    if (indB)
        result(y,:) = 10*log10(abs(fftshift(YY)));
    else
        result(y,:) = fftshift(YY);
    end
    mid32carriers(y,:) = result(y,16:47);
    x = x + numSamplesPerFrame;
    y = y + 1;
end

