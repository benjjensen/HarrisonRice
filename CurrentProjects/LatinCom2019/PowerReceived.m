% Power Received

clear; close all;

timeToAverage = 100; % Average 100 ms


load('powerArray.mat');
dataCount = length(powerArray);

for index = 1:(timeToAverage):(dataCount - timeToAverage)
    startPoint = index;
    endPoint = startPoint + timeToAverage - 1;
    averagedData(index) = mean(powerArray(1, startPoint:endPoint));
end
NANs = nan(1, (300000 - 237201));
averagedData = [averagedData, NANs];

figure()
subplot(2,1,1)
plot(averagedData);
title('Power Received (' + string(timeToAverage) + ' ms)');
xlabel('Time (ms)');
ylabel('Power...?');
xlim([0 300000]);


