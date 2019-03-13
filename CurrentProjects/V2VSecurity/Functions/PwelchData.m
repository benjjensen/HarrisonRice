function [result, mid32carriers] = PwelchData(dataArray, numSamplesPerFrame, indB)
% Created by Kalin Norman
%   This function should be used to take the data collected for V2V
%   security purposes and that has already been run through the processing
%   file created by Dr Rice. This function will then pwelch and fftshift
%   the data, outputting either a linear or dB result, depending on the
%   parameter "indB". Additionally, from our initial calculations, in order
%   to get lambda/10 spacing per frame we want to use a numSamplesPerFrame
%   of 14912, but a parameter for this number has been given in order to
%   allow for other options, should they be desired in the future.

[sizex, sizey, sizez] = size(dataArray); % Reads the size of the array that
% has been passed in (should be a 1x# array, leading sizey to indicated how
% long it really is
Nfft = 64; % Number of sub-carriers
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
y = 1; % Used to count the number of frames
x = 1;
result(1,64) = 0;
mid32carriers(1,32) = 0;
while (x < sizey) % Run to the end of the dataArray
    z = x + numSamplesPerFrame - 1; % from x to z should be 14912, or ...
    % numSamplesPerFrame long
    if (z > sizey) % If z is beyond the end of the data
        z = sizey; % Sets z to the end of of the data
        disp('Last input did not match samples per frame');
    end
    YY = pwelch(dataArray(x:z),boxcar(Nfft),0,Nfft,'twosided');
    YY = YY';
    if (indB) % Result given in dB values
        result(y,:) = 10*log10(abs(fftshift(YY)));
    else % Result given in linear values
        result(y,:) = fftshift(YY);
    end
    mid32carriers(y,:) = result(y,16:47); % Separates the middle 32 carriers
    x = x + numSamplesPerFrame; % Increments x by the appropriate amount
    y = y + 1; % Increments y to begin the next row of the result array
end

