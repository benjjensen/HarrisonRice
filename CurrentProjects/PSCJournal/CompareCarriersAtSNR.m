%%%%% CompareCarriersAtSNR (Benjamin Jensen) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Loads in processed data
% At a chosen SNR level, this calculates the following:
%    1) The number of good carriers for each room for each sample
%    2) The average signal strength for each carrier across all samples
%    3) Reliability of each carrier
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear; close all;

%%%%% User Inputs %%%%
SNR = 80;
startCarrier = 1;
endCarrier = 64;
%%%%%%%%%%%%%%%%%%%%%%
    
    % Load in appropriate data
addpath 'Functions'    

load('Testing/July17thTesting/ProcessedNormalized/harProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseHar');
load('Testing/July17thTesting/ProcessedNormalized/smaProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseSma');
load('Testing/July17thTesting/ProcessedNormalized/camProcessed');
    load('Testing/July17thTesting/ProcessedNormalized/aveNoiseCam');

   %% 1) Number of good carriers per sample
    
[~, harrison] = countGoodCarriers(harSignal, aveNoiseHar, SNR, startCarrier, endCarrier);
[~, smalley] = countGoodCarriers(smaSignal, aveNoiseSma, SNR, startCarrier, endCarrier);
[~, camacho] = countGoodCarriers(camSignal, aveNoiseCam, SNR, startCarrier, endCarrier);

figure()
hold on
stem(harrison)
stem(smalley)
stem(camacho)
title('Good Carriers (' + string(SNR) + ' dB)');
legend('Harrison','Smalley','Camacho');
xlabel('Sample Number')
ylabel('Number of carriers')
hold off


    %% 2) Average Strength of each Carrier
    
[aveHarrison] = averageCarrierStrength(harSignal);
[aveSmalley] = averageCarrierStrength(smaSignal);
[aveCamacho] = averageCarrierStrength(camSignal);

figure()
hold on
grid on
stem(aveHarrison)
stem(aveSmalley)
stem(aveCamacho)
title('Average Carrier Strength (' + string(SNR) + ' dB)');
legend('Harrison','Smalley','Camacho')
xlabel('Carrier Number')
ylabel('Number of carriers')
ylim([-50 0])
xlim([0 64])
hold off

    %% 3) Carrier Reliability

relHar = carrierReliability(harSignal, aveNoiseHar, SNR);
relSma = carrierReliability(smaSignal, aveNoiseSma, SNR);
relCam = carrierReliability(camSignal, aveNoiseCam, SNR);

figure()
hold on
stem(relHar)
stem(relSma)
stem(relCam)
legend('Harrison','Smalley','Camacho')
title('Carrier Reliability (' + string(SNR) + ' dB)')
xlabel('Carrier Number')
ylabel('Frequency of being above /tau');
hold off
