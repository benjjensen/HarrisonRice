clear; 
close all;
load('../Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat');
data = graphPwelchedData;
[numRows, numCarriers] = size(data);

    %% Calculate capacity  
    
capacityPerCarrier = zeros(numRows, numCarriers);
capacityPerLocation = zeros(numRows, 1);

    % Calculate the capacity per carrier, then sum up for location capacity
    
SNR = 1000;       % Arbitrary
data = sqrt(data);
maxValue = max(max(data));
data = data ./ maxValue;
data = data.^2;

for location = 1:numRows
    for carrier = 1:numCarriers
        capacityPerCarrier(location, carrier) = 0.5*log2(1+SNR*data(location, carrier));
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

% figure()
% plot(capacity_dB);
% xlabel("Location");
% ylabel("Capacity (dB)");

figure()
plot(capacityPerLocation);
xlabel("Location");
ylabel("Capacity (linear)");


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
ylabel("Bob's Location");
xlabel("Eve's Location");
zlabel("Secrecy Capacity (linear)");
title("Secrecy Capacity, linear");
xticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 indexOfMaxCapacity indexOfMaxCapacity+1000 indexOfMaxCapacity+2000]);
xticklabels({'-2000', '-1000', '0', '1000', '2000'});
yticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 indexOfMaxCapacity indexOfMaxCapacity+1000 indexOfMaxCapacity+2000]);
yticklabels({'-2000', '-1000', '0', '1000', '2000'});
xlim([0 4352]);
ylim([0 4352]);
view(45, 60);
colorbar;

figure()
contour(secrecyCapacity);
ylabel("Bob's Location");
xlabel("Eve's Location");
title("Secrecy Capacity Contour, linear");
xticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 indexOfMaxCapacity indexOfMaxCapacity+1000 indexOfMaxCapacity+2000]);
xticklabels({'-2000', '-1000', '0', '1000', '2000'});
yticks([indexOfMaxCapacity-2000 indexOfMaxCapacity-1000 indexOfMaxCapacity indexOfMaxCapacity+1000 indexOfMaxCapacity+2000]);
yticklabels({'-2000', '-1000', '0', '1000', '2000'});
xlim([0 4352]);
ylim([0 4352]);
colorbar;





