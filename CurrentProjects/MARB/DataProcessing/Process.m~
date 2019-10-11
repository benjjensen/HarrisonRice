clear; close all;
tic;

addpath('../Data/');
inFileNames = ["Harrison1a.mat", "Harrison1b.mat", "Farah2a.mat", ...
    "Farah2b.mat", "Kalin3a.mat", "Kalin3b.mat", "Benj4a.mat", ...
    "Benj4b.mat", "Amy5a.mat", "Amy5b.mat", "Nathan6a.mat", ...
    "Nathan6b.mat", "Autumn7a.mat", "Autumn7b.mat", "Bryan9a.mat", ...
    "Bryan9b.mat", "Spencer11a.mat", "Spencer11b.mat", ...
    "Morteza12a.mat", "Morteza12b.mat"];
[~, numDataFiles] = size(inFileNames);

addpath('../RefSignalFolder/');
refFileNames = ["Radio1_ref.mat", "Radio2_ref.mat", "Radio3_ref.mat", ...
    "Radio4_ref.mat", "Radio5_ref.mat", "Radio6_ref.mat", ...
    "Radio7_ref.mat", "Radio9_ref.mat", "Radio11_ref.mat", ...
    "Radio12_ref.mat"];
[~, numRefFiles] = size(refFileNames);

for i = 1:numDataFiles
%     tic;
    structContainingData = load(inFileNames(i)); % Load in raw data
    % The data is loaded in as a struct, so the desired array must be
    % extracted from the 1x1 struct
    dataArray = struct2array(structContainingData);
   
    Nfft = 128; % Number of FFT bins
    % Gets the processed data showing each carrier, function inputs are the
    % raw data array and the number of fft bins for the pwelch function
    linearData = PwelchAndFFTShift(dataArray, Nfft);
    
    % The pluto radios attentuate the edge carriers, RemoveAttentuatedEdges
    % sets all of those attenuated carriers to a value of 0. The function
    % inputs are the processed data (in linear or dB), whether or not the
    % noise carriers are on the odd or even numbered indexes (true for odd,
    % false for even), and whether or not the data is linear or in dB (true
    % for linear, false for dB)
    noiseOnOddCarriers = true;
    isolatedGoodData = RemoveAttenuatedEdges(linearData, noiseOnOddCarriers, true);
    data(i,:,:) = isolatedGoodData;
%     toc;
end

% Finds the common carriers across all the data sets and frames that are
% good and removes any inconsistent carriers so that the data may be
% compared appropriately across all of the data sets
[numFiles, numCarriers, numFrames] = size(data);
allNonZero = numFiles * numFrames;
leftCarrier = 0;
rightCarrier = 0;
for i = 1:numCarriers
    if nnz(data(:,i,:)) == allNonZero
        if leftCarrier == 0
            leftCarrier = i;
        end
        rightCarrier = i;
    end
end
goodCarriers = zeros(numCarriers,1);
if mod(sum(goodCarriers(:,1)),2) ~= 0
    goodCarriers(leftCarrier:rightCarrier,1) = 1;
    halfwayCarrier = round(numCarriers / 2);
    numLeftCarriers = sum(goodCarriers(1:halfwayCarrier,1));
    numRightCarriers = sum(goodCarriers(halfwayCarrier:numCarriers,1));
    if numLeftCarriers > numRightCarriers
        leftCarrier = leftCarrier + 1;
    else
        rightCarrier = rightCarrier - 1;
    end
end

data(:,1:leftCarrier-1,:) = 0;
data(:,rightCarrier+1:numCarriers,:) = 0;

% Plots one frame of the data showing which carriers were common across all
% the data sets allowing for visual confirmation for the user
stem(10*log10(abs(linearData(:,1))));
hold on
maskedData = zeros(numCarriers,1);
maskedData(leftCarrier:rightCarrier,1) = linearData(leftCarrier:rightCarrier,1);
stem(10*log10(abs(maskedData)), 'MarkerFaceColor','k');
title({'Resulting Data that is Analyzed', ...
    'Plot values are in dB for visual clarity'});
hold off

% Scales the data so that the average noise power is equal to 1
scaledData = ScaleAvgNoiseToOne(data, noiseOnOddCarriers);
disp('Finished Base Processing of Data Sets at ' + string(datetime));

% Loads in the Reference Signal data, processes it, and isolates the same
% carriers found in the measured data
for i = 1:numRefFiles
    structContainingData = load(refFileNames(i)); % Load in raw data
    % The data is loaded in as a struct, so the desired array must be
    % extracted from the 1x1 struct
    dataArray = struct2array(structContainingData);
   
    Nfft = 128; % Number of FFT bins
    % Gets the processed data showing each carrier, function inputs are the
    % raw data array and the number of fft bins for the pwelch function
    linearData = PwelchAndFFTShift(dataArray, Nfft);
    [x, y] = size(linearData);
    
    % Restricts the carriers that we care about to match the carriers from
    % the data sets
    isolatedGoodData = zeros(x,y);
    isolatedGoodData(leftCarrier:rightCarrier,:) = linearData(leftCarrier:rightCarrier,:);
    refData(i,:,:) = isolatedGoodData;
end

% Scales the reference signal so that the sum of the signal carriers is
% equal to 1
scaledRefData = ScaleRefSignalToSumToOne(refData, noiseOnOddCarriers);
disp('Finished Base Processing of Reference Signals at ' + string(datetime));

% In the frequency domain, divides the signal carriers for each data set by
% the reference carriers for the radio used in that data set
signalCarriers = zeros(numDataFiles,numCarriers,numFrames);
refSignalCarriers = zeros(numRefFiles,numCarriers);
signalCarriers(:,2:2:numCarriers,:) = scaledData(:,2:2:numCarriers,:);
refSignalCarriers(:,2:2:numCarriers) = scaledRefData(:,2:2:numCarriers);
for i = 1:numRefFiles
    b = 2 * i;
    a = b - 1;
    for j = 1:numFrames
        removedRefSignal(a,:,j) = signalCarriers(a,:,j) ./ refSignalCarriers(i,:);
        removedRefSignal(b,:,j) = signalCarriers(b,:,j) ./ refSignalCarriers(i,:);
    end
end

% Normalize each carrier by dividing by the max carrier overall
maxCarrierVal = max(max(max(removedRefSignal)));
processedData = removedRefSignal ./ maxCarrierVal;
toc;


%% Desired Process for the Code

% Pwelch and FFTShift everything

% Scale the data so that the avg noise power is 1
%%% Verify that the signal to noise difference is equal for before and
%%% after scaling in dB

% Normalize the reference signal by scaling the height of all of the
% carriers to sum to 1

% In frequency domain: divide the signal carriers by the reference carriers

% Repeat 1-4 for each frame

% Repeat 1-5 for each channel

% Normalize all the channels by the strongest carrier of all of the
% channels
