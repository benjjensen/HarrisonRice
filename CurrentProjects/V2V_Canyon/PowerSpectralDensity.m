%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% Power Spectral Density %%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Sept 24 2019

%%%%% Overview %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Performs a non-parametric manual pwelch on a data files
%    1) Reads in a block of data (n = NFFT)
%    2) Windows the data (Blackman window)
%    3) Takes the FFT, and then the magnitude squared
%    4) Shifts by NFFT/2 and repeats, summing the results
%    5) Divides by the total number lf length NFFT blocks 
%
%   (Note - for speed, MatFileReade is used; however, with the way MFR
%   reads data in, we could not actually shift by NFFT/2 and pull in data
%   again. Because of this, the file actual pulls in NFFT, then splits it
%   into 2 and uses each half twice)
%
% Files Used:
%
%   - Data files downampled to 20 Msamples per second (result of resampleData.m) 
%
% Files Produced:
%
%   - Processed Data
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%% Process Resampled Data
clear;

disp('Beginning to manually pwelch at ' + string(datetime));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% User Inputs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inFilename = '/media/V2VData01/DiamondForkA_20Msps.mat'; % Up the canyon
outFilename = '/media/V2VData01/temp.mat'; 
nfft = 2048;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

halfStep = nfft / 2;
isfirst = true;
count = 0;
processed(1:nfft,1) = zeros();

% File Reader
mfr = dsp.MatFileReader(inFilename,'VariableName','y','SamplesPerFrame',nfft);

% Read in file and perform calculations
while ~isDone(mfr)
    a = mfr();              % Gets next input from file (reads in as a struct when the result is complex)
    a = a.real + j*a.imag;  % Turns the struct into a single complex variable
    if isDone(mfr)          % Ignores the last little bit because I didn't have time to get fancy and work with every last bit of the data
    else
        if isfirst
            b(1:halfStep,1) = a(halfStep+1:nfft,1);
            isfirst = false;
        else
            b(halfStep+1:nfft,1) = a(1:halfStep,1);
            b1 = b .* blackman(nfft);
            b2 = fft(b1);
            b3 = abs(b2) .^ 2;
            processed = processed + b3;
            count = count + 1;

        end
        a1 = a .* blackman(nfft);
        a2 = fft(a1);
        a3 = abs(a2) .^ 2;
        processed = processed + a3;
        count = count + 1;

        if ~isDone(mfr)
            b(1:halfStep,1) = a(halfStep+1:nfft,1);
        end
    end
end
release(mfr);

outData = processed ./ count;
outDataShifted = fftshift(processed);

save(outFilename, 'outData', 'outDataShifted');

disp('Finished manual pwelch at ' + string(datetime));