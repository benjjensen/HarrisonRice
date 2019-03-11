clear; 
close all;

% load('../Functions');

load('../Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat');
axisTicks = [-2000 -1000 0 1000 2000];

% load('../Data/GraphPwelchedData/test-point-B_graph-pwleched-data.mat');
% axisTicks = [-1500 -750 0 750 1500];

gSquared = graphPwelchedData;
[numRows, numCarriers] = size(gSquared);

    %% Calculate capacity  
    
capacityPerCarrier = zeros(numRows, numCarriers);
capacityPerLocation = zeros(numRows, 1);

    % Calculate the capacity per carrier, then sum up for location capacity
    
SNR = 1000;       % Arbitrary
g = sqrt(gSquared);
maxValue = max(max(g));
normalizedG = g ./ maxValue;
gSquared = normalizedG.^2;

for location = 1:numRows
    for carrier = 1:numCarriers
        capacityPerCarrier(location, carrier) = 0.5*log2(1+SNR*gSquared(location, carrier));
    end
end
   
for location = 1:numRows
    temp = 0;
    for carrier = 1:numCarriers
        temp = temp + capacityPerCarrier(location,carrier);
    end
    capacityPerLocation(location) = temp;
end

    % Plot in dB to see differences better
capacity_dB = mag2db(capacityPerLocation);

[maxCapacity, indexOfMaxCapacity] = max(capacityPerLocation);  % Tx is at indexOfMaxCapacity (hopefully)

figure()
plot(capacityPerLocation);
xlabel("Location");
ylabel("Capacity");


    %% Calculate secrecy capacity

secrecyCapacity = zeros(numRows);

for bobsLocation = 1:numRows
    for evesLocation = 1:numRows
        temp = 0;
        for carrier = 1:numCarriers
            if (capacityPerCarrier(bobsLocation, carrier) > capacityPerCarrier(evesLocation, carrier))
                temp = temp + (capacityPerCarrier(bobsLocation, carrier) ...
                    - capacityPerCarrier(evesLocation, carrier));
            end 
        end
        secrecyCapacity(bobsLocation, evesLocation) = temp;
    end
end


%% Plot Figures

secrecyCapacity_dB = mag2db(secrecyCapacity);


figure()
h = surf(secrecyCapacity);
set(h, 'LineStyle','none');     % Removes black lines so you can see the data
ylabel("Bob's distance from Alice (m)");
xlabel("Eve's distance from Alice (m)");
zlabel("Secrecy Capacity (bits)");
title("Secrecy Capacity");
xticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) indexOfMaxCapacity+axisTicks(5)]);
xticklabels({string(axisTicks(1)), string(axisTicks(2)), string(axisTicks(3)), string(axisTicks(4)), string(axisTicks(5))});
yticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) indexOfMaxCapacity+axisTicks(5)]);
yticklabels({string(axisTicks(1)), string(axisTicks(2)), string(axisTicks(3)), string(axisTicks(4)), string(axisTicks(5))});
xlim([0 numRows]);
ylim([0 numRows]);
view(45, 60);
colorbar;



% x = linspace(-2000, 2000, 4000);
% y = x;
% y1 = -x;

hold on 
imagesc(secrecyCapacity);
plot(y,x,'LineWidth',2);
plot(y1,x,'LineWidth',2);

% figure()
% contour(secrecyCapacity);
ylabel("Bob's distance from Alice (m)");
xlabel("Eve's distance from Alice (m)");
title("Secrecy Capacity");
xticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) indexOfMaxCapacity+axisTicks(5)]);
xticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), string(axisTicks(4)/10), string(axisTicks(5)/10)});
yticks([indexOfMaxCapacity+axisTicks(1) indexOfMaxCapacity+axisTicks(2) indexOfMaxCapacity+axisTicks(3) indexOfMaxCapacity+axisTicks(4) indexOfMaxCapacity+axisTicks(5)]);
yticklabels({string(axisTicks(1)/10), string(axisTicks(2)/10), string(axisTicks(3)/10), string(axisTicks(4)/10), string(axisTicks(5)/10)});
xlim([0 numRows]);
ylim([0 numRows]);
colorbar;

hold off

