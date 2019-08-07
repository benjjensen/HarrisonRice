%%%%% Optimize SNR (Benjamin Jensen) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Loads in processed data
% Checks number of good carriers for each room over a range of SNR values
% Compares the results to determine best SNR value 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear; close all;

%%%%% User Inputs %%%%
minSNR = 1;
stepSNR = 1;
maxSNR = 100;
startCarrier = 11;
endCarrier = 55;
%%%%%%%%%%%%%%%%%%%%%%

    
    % Load in appropriate data
load('Testing/July17thTesting/ProcessedNormalized/harProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseHar');
load('Testing/July17thTesting/ProcessedNormalized/smaProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseSma');
load('Testing/July17thTesting/ProcessedNormalized/camProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseCam');

    
    % Count number of carriers above a threshold for a range of thresholds
for SNR = minSNR:stepSNR:maxSNR
    [counts(1, SNR), ~] = countGoodCarriers(harSignal, aveNoiseHar, SNR, startCarrier, endCarrier);
    [counts(2, SNR), ~] = countGoodCarriers(smaSignal, aveNoiseSma, SNR, startCarrier, endCarrier);
    [counts(3, SNR), ~] = countGoodCarriers(camSignal, aveNoiseCam, SNR, startCarrier, endCarrier);
end

difference = min((counts(1,:) - counts(2,:)),(counts(1,:) - counts(3,:)));
maxDifference = max(difference)
bestSNR = find(difference == maxDifference)

    % Plot the results   
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
    