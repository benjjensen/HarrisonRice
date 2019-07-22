clear; close all;
    
    %%

    % Count number of carriers above a threshold for a range of thresholds
for SNR = 1:100
    [counts(1, SNR), ~] = countGoodCarriers(harSignal, aveNoiseHar, SNR, startCarrier, endCarrier);
    [counts(2, SNR), ~] = countGoodCarriers(smaSignal, aveNoiseSma, SNR, startCarrier, endCarrier);
    [counts(3, SNR), ~] = countGoodCarriers(camSignal, aveNoiseCam, SNR, startCarrier, endCarrier);
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
    