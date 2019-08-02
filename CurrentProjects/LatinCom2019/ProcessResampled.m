%%% Process Resampled Data
clear;

disp('Beginning to manually pwelch at ' + string(datetime));
%%%%%%%%%%%%%%%%%%%%%
% User Inputs
%%%%%%%%%%%%%%%%%%%%%
% filename = '/media/V2V/Sundance-To-US189-25June2019_20Msps.mat'; % Down the canyon
filename = '/media/V2V/US189-To-Sundance-25June2019_20Msps.mat'; % Up the canyon
nfft = 2048;
%%%%%%%%%%%%%%%%%%%%%

halfStep = nfft / 2;
isfirst = true;
count = 0;
processed(1:nfft,1) = zeros();

% File Reader
mfr = dsp.MatFileReader(filename,'VariableName','y','SamplesPerFrame',nfft);

% Read in file and perform calculations
while ~isDone(mfr)
    a = mfr(); % Gets next input from file (reads in as a struct when the result is complex)
    a = a.real + j*a.imag; % Turns the struct into a single complex variable
    if isDone(mfr) % Ignores the last little bit because I didn't have time to get fancy and work with every last bit of the data
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

processed = processed ./ count;
shifted = fftshift(processed);

% save Processed-Sundance-US189-25June2019.mat processed shifted; % Down Canyon
save Data/Processed-US189-Sundance-25June2019.mat processed shifted; % Up Canyon

disp('Finished manual pwelch at ' + string(datetime));