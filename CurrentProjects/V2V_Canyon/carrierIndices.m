% Find Carrier Indices:

clear;

%%%%% User Inputs %%%%%

filename = '/media/V2VData01/AlpineLoop_pt2B_20Msps.mat';
nfft = 2048;
sampleRate = 20;    % in MSamples/sec (20)
numSeconds = 0.001; % How many seconds we average over (0.001)
%%%%%%%%%%%%%%%%%%%%%%%

load('carrierIndices.mat')   % Contain indices of the 64 carriers (not noise)

samplesPerMegasample = 10^6;

% Calculate nfft's per numSeconds
samplesPerTime = sampleRate * samplesPerMegasample * numSeconds; % (20000)

mfr = dsp.MatFileReader(filename,'VariableName','y','SamplesPerFrame',samplesPerTime);  % Pull in (20000) samples at a time

largeData = mfr();
largeData = largeData.real + j*largeData.imag;

