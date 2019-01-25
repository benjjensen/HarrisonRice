clear;
close all;
load linear_signal.mat;
load linear_noisefloor.mat;
dbSignal = 10*log10(abs(fftshift(linear_signal)));
dbNoise = 10*log10(abs(fftshift(linear_noisefloor)));
dbCombined(1:2:127,:,:) = dbSignal;
dbCombined(2:2:128,:,:) = dbNoise;
carrierStrengths(128) = zeros();
linCombined(1:2:127,:,:) = linear_signal;
linCombined(2:2:128,:,:) = linear_noisefloor;
linStrength(128) = zeros();
justSignal(64) = zeros();
count = 0
for x = 1:128
    for y = 1:90
        for z = 1:345
            if isnan(dbCombined(x,y,z))
            else
                carrierStrengths(x) = carrierStrengths(x) + dbCombined(x,y,z);
            end
            if ~isnan(linCombined(x,y,z))
                linStrength(x) = linStrength(x) + linCombined(x,y,z);
                count = count + 1;
            end
            if (x <= 64)
                if ~isnan(linear_signal(x,y,z))
                    justSignal(x) = justSignal(x) + linear_signal(x,y,z);
                end
            end
        end
    end
end
figure();
bar(carrierStrengths); % Plot of all of the carriers (used to determine which ones to keep)
ylim([-9e5 0]);
title("Sum of Individual Carrier 'Strengths' in dB");
figure();
linStrength = linStrength ./ count;
bar(fftshift(linStrength));
title("Averages");
dbStrength = 10*log10(linStrength);
figure();
bar(dbStrength);
title("Averages dB");
%%
dbS = dbSignal;
dbN = dbNoise;
if (isnan(dbS))
else
    dbS(1:10,:,:) = 0; % keep carriers 11 - 55 of the signal
    dbS(56:64,:,:) = 0;
end
if (isnan(dbN))
else
    dbN(1:9,:,:) = 0; % keep carriers 10 - 54 of the noise
    dbN(55:64,:,:) = 0;
end
dbC(1:2:127,:,:) = dbS; % if using one array, keep carriers 20 - 109
dbC(2:2:128,:,:) = dbN;
carrierModified(128) = zeros();
for x = 1:128
    for y = 1:90
        for z = 1:345
            if isnan(dbC(x,y,z))
            else
                carrierModified(x) = carrierModified(x) + dbC(x,y,z);
            end
        end
    end
end
figure();
bar(carrierModified); % Plot of carriers that were kept (compare to original
% plot to verify that desired carriers were kept)
ylim([-9e5 0]);
title("Modified Sum of Individual Carrier 'Strengths' in dB");

linearSignalReducedCarriers = linear_signal(11:55,:,:);
linearNoiseReducedCarriers = linear_noisefloor(10:54,:,:);
avgSignal(45) = zeros();
for x = 1:45
    count = 0;
    for y = 1:90
        for z = 1:345
            if isnan(linearSignalReducedCarriers(x,y,z))
            else
                avgSignal(x) = avgSignal(x) + linearSignalReducedCarriers(x,y,z);
                count = count + 1;
            end
        end
    end
    avgSignal(x) = avgSignal(x) / count;
end
overallAvgSignal = sum(avgSignal) / 45;
figure();
bar(avgSignal);
title("Average Linear Signal Value Per Carrier");