% Power Received

clear; close all;

timeToAverage = 100; % Average 100 ms

load('powerArray.mat');
dataCount = length(powerArray);
saveIndex = 1;

for index = 1:(timeToAverage):(dataCount - timeToAverage)
    startPoint = index;
    endPoint = startPoint + timeToAverage - 1;
    averagedData(saveIndex) = mean(powerArray(1, startPoint:endPoint));
    saveIndex = saveIndex + 1;
end
NANs = nan(1, (3000 - 2372));
averagedData = [averagedData, NANs];

figure()
subplot(2,1,1)
plot(averagedData);
title('Power Received (' + string(timeToAverage) + ' ms)');
xlabel('Time (ms)');
ylabel('Power...?');
xlim([0 3000]);
ylim([0 (10^6)]);

%% Data from VehicleLineOfSightToUS189

X = [527 552 552 566 566 568.3 568.3 571 571 575 575 581 581 584.8 584.8 596 596 603 603 611 611 629 629 656 656 661 661 680 680 682 682 722 722 732 732 771.5 771.5 805 805];
X1 = X - 527;
Y1 = [1 1  0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 nan];


subplot(2,1,2);
plot(X1, Y1, 'LineWidth',2);
ylim([0 1.5]);
xlim([0 300]);
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');



