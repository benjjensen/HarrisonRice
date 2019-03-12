clear; 
close all;
addpath('Functions');

    % Plot Test Point A
load('Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat');
axisTicks = [-2000 -1000 0 1000 2000];

    % Plot Test Point B
% load('Data/GraphPwelchedData/test-point-B_graph-pwleched-data.mat');
% axisTicks = [-1500 -750 0 750 1500];

gSquared = graphPwelchedData;
[numRows, numCarriers] = size(gSquared);
SNR = 1000;

normalizedGSquared = normalizeChannelGain(gSquared);
[capacityPerLocation, capacityPerCarrier] = gaussian_capacity(normalizedGSquared, SNR);
[secrecyCapacityPerLocation, secrecyCapacityPerCarrier] = ...
    calculateSecrecyCapacity(capacityPerCarrier, capacityPerCarrier);

%% Plot Figures

[maxCapacity, indexOfMaxCapacity] = max(capacityPerLocation);

figure()
h = surf(secrecyCapacityPerLocation);
set(h, 'LineStyle','none');     % Removes black lines so you can see the data
ylabel("Bob's distance from Alice (m)");
xlabel("Eve's distance from Alice (m)");
zlabel("Secrecy Capacity");
title("Secrecy Capacity");
xticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) ...
        indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) ...
        indexOfMaxCapacity+axisTicks(5)]);
xticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), ...
        string(axisTicks(4)/10), string(axisTicks(5)/10)});
yticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) ...
        indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) ...
        indexOfMaxCapacity+axisTicks(5)]);
yticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), ...
        string(axisTicks(4)/10), string(axisTicks(5)/10)});
xlim([0 numRows]);
ylim([0 numRows]);
view(45, 60);
y = colorbar;
ylabel(y, 'Bits per channel use');

figure()
imagesc(secrecyCapacityPerLocation);
ylabel("Bob's distance from Alice (m)");
xlabel("Eve's distance from Alice (m)");
title("Secrecy Capacity");
xticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) ...
        indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) ...
        indexOfMaxCapacity+axisTicks(5)]);
xticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), ...
        string(axisTicks(4)/10), string(axisTicks(5)/10)});
yticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) ...
        indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) ...
        indexOfMaxCapacity+axisTicks(5)]);
yticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), ...
        string(axisTicks(4)/10), string(axisTicks(5)/10)});
xlim([0 numRows]);
ylim([0 numRows]);
y = colorbar;
ylabel(y, 'Bits per channel use');



