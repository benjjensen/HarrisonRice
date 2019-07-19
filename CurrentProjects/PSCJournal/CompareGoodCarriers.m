clear; close all;

startCarrier = 11;
endCarrier = 55;

addpath 'Functions'

    % Uses RAW data (it is processed by this file)
load('HarrisonTest.mat');
load('smalleyTest.mat');
load('CamachoTest.mat');

harrison = HarrisonTest;
smalley = smalleyTest;
camacho = CamachoTest;

[numCarriers, numSamples] = size(harrison);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% PROCESS DATA (Pwelch, FFT Shift) %%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[harrisonLin, ~] = processArray(harrison, numSamples);
[smalleyLin, ~] = processArray(smalley, numSamples);
[camachoLin, ~] = processArray(camacho, numSamples);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% NORMALIZE (by overall largest) %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    % For this case, signal is on even carriers (to remove DC)
harSignal = harrisonLin(2:2:end, :);
harNoise = harrisonLin(1:2:end, :);
smaSignal = smalleyLin(2:2:end, :);
smaNoise = smalleyLin(1:2:end, :);
camSignal = camachoLin(2:2:end, :);
camNoise = camachoLin(1:2:end, :);

    % Averages the noise floor
for sample = 1:numSamples
    aveNoiseHar(:, sample) = mean(harNoise(startCarrier:endCarrier, sample));
    aveNoiseSma(:, sample) = mean(smaNoise(startCarrier:endCarrier, sample));
    aveNoiseCam(:, sample) = mean(camNoise(startCarrier:endCarrier, sample));    
end 


harSignal = harSignal ./ aveNoiseHar;   % Normalize to account for AGC
    harSignal = sqrt(harSignal);        % Find g from g^2
smaSignal = smaSignal ./ aveNoiseSma;
    smaSignal = sqrt(smaSignal);
camSignal = camSignal ./ aveNoiseCam;
    camSignal = sqrt(camSignal);

    % Find max
trueMax = max([ max(max(harSignal)) max(max(smaSignal)) max(max(camSignal))]);
 
harSignal = harSignal ./ trueMax;
    harSignal = harSignal .^2;
camSignal = camSignal ./ trueMax;
    camSignal = camSignal .^2;
smaSignal = smaSignal ./ trueMax;
    smaSignal = smaSignal .^2;

    % Count number of carriers above a threshold for a range of thresholds
for SNR = 1:100
    counts(1, SNR) = countGoodCarriers(harSignal, aveNoiseHar, SNR, startCarrier, endCarrier);
    counts(2, SNR) = countGoodCarriers(smaSignal, aveNoiseSma, SNR, startCarrier, endCarrier);
    counts(3, SNR) = countGoodCarriers(camSignal, aveNoiseCam, SNR, startCarrier, endCarrier);
end

difference = min((counts(1,:) - counts(2,:)),(counts(1,:) - counts(3,:)));

%%%%%%%%%%%%%%%%%%%%
%%%%%%% PLOT %%%%%%%
%%%%%%%%%%%%%%%%%%%%

figure()
hold on
plot(counts(1,:), 'k');
plot(counts(2,:), 'b');
plot(counts(3,:), 'g');
legend('Harrison','Smalley','Camacho');
ylabel('Number of Carriers above threshold');
xlabel('Threshold (dB above noise)');
grid on
hold off

figure()
plot(difference);
title("Harrison's Advantage");
ylabel("How many more carriers above threshold than the others");
xlabel("Threshold (in dB above the average noisefloor)");
    