% Normalizes the channel gain, finds the average gain^2 value, and then
% calculates capacity and secrecy capacity for each carrier and at each
% location. Also plots said data. 

clear; 
close all;

SNR = 1000;

    % Loads in the appropriate data
load('Data/CorrectlyAveragedData/test-point-A_jagged-mid32.mat');
addpath('Functions');

[numRows, numCarriers] = size(pwelched);
normalizedGSquared = normalizeChannelGain(pwelched);

averageGSquared = getAverageGSquared(pwelched);

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
xticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 ... 
        indexOfMaxCapacity indexOfMaxCapacity+1000 ...
        indexOfMaxCapacity+2000]);
xticklabels([-200, -100, 0, 100, 200]); % Modifies what text is displayed on the x axis tick labels
yticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 ...
        indexOfMaxCapacity indexOfMaxCapacity+1000 ...
        indexOfMaxCapacity+2000]);
yticklabels([-200, -100, 0, 100, 200]); % Modifies what text is displayed on the y axis tick labels
xlim([indexOfMaxCapacity-2000 indexOfMaxCapacity+2000]);
ylim([indexOfMaxCapacity-2000 indexOfMaxCapacity+2000]);
view(45, 60);
y = colorbar;
ylabel(y, 'Equivocation at Eve (bits/channel use)');

figure()
imagesc(secrecyCapacityPerLocation);
ylabel("Bob's distance from Alice (m)");
xlabel("Eve's distance from Alice (m)");
title("Secrecy Capacity");
xticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 ...
        indexOfMaxCapacity indexOfMaxCapacity+1000 ...
        indexOfMaxCapacity+2000]);
xticklabels([-200, -100, 0, 100, 200]); 
yticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 ...
        indexOfMaxCapacity indexOfMaxCapacity+1000 ...
        indexOfMaxCapacity+2000]);
yticklabels([-200, -100, 0, 100, 200]);
xlim([indexOfMaxCapacity-2000 indexOfMaxCapacity+2000]);
ylim([indexOfMaxCapacity-2000 indexOfMaxCapacity+2000]);
y = colorbar;
ylabel(y, 'Equivocation at Eve (bits/channel use)');
% saveas(gcf,'V2ISecrecyCapacity','epsc');


%% Functions

function [averageGSquared] = getAverageGSquared(gSquared)
%getAverageGSquared is used to get the average g^2 value
% sums up all the gainSquared values and then divides by the count
%This information was used as a reference for the magazine article

    [numRows, numCarriers] = size(gSquared);
    sum = 0;
    count = 0;
    for loc = 1:numRows
        for carrier = 1:numCarriers
            sum = sum + gSquared(loc, carrier);
            count = count + 1;
        end
    end
    averageGSquared = sum/count;
    
end

