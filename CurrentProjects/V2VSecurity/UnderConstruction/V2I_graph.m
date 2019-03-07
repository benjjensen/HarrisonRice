clear; 
close all;
load('../Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat');
data = graphPwelchedData;
[numRows, numCarriers] = size(data);

    %% Calculate capacity
% comparison = gaussian_capacity(data, 1);    
    
capacityPerCarrier = zeros(numRows, numCarriers);
capacityPerLocation = zeros(numRows, 1);

    % Calculate the capacity per carrier, then sum up for location capacity
SNR = 1000;       % Arbitrary
maxValue = max(max(data));
data = data ./ maxValue;

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

% figure()
% plot(capacity_dB);
% xlabel("Location");
% ylabel("Capacity (dB)");

[maxCapacity, indexOfMaxCapacity] = max(capacityPerLocation);  % Tx is at indexOfMaxCapacity (hopefully)

figure()
plot(capacity_dB);
xlabel("Location");
ylabel("Normalized Capacity (dB)");

figure()
plot(capacityPerLocation);


    %% Calculate secrecy capacity
    
% capacity = capacity(1:10:end);
% numRows = 436;

secrecyCapacity = zeros(numRows);

for bobsLocation = 1:numRows
    for evesLocation = 1:numRows
        if (capacityPerLocation(bobsLocation) <= capacityPerLocation(evesLocation))
            secrecyCapacity(bobsLocation, evesLocation) = 0;
        else 
            secrecyCapacity(bobsLocation, evesLocation) = ...
                capacityPerLocation(bobsLocation) - capacityPerLocation(evesLocation);
        end
    end
end

% index = zeros(numRows, 1);
% for i = 1:numRows
%     index(i) = i - indexOfMaxCapacity;
% end
% index = index(1:floor(numRows/5):end);

%% Plot Figures

secrecyCapacity_dB = mag2db(secrecyCapacity);

% figure()
% h = surf(secrecyCapacity_dB);
% set(h, 'LineStyle','none');     % Removes black lines so you can see the data
% xlabel("Bob's Location");
% % set(gca,'XTickLabel', index);
% % set(gca,'YTickLabel', index);
% ylabel("Eve's Location");
% zlabel("Secrecy Capacity (dB)");
% title("Secrecy Capacity, dB");
% xlim([0 4352]);
% ylim([0 4352]);
% zlim([-150 0]);
% view(45, 60);
% colormap(jet);

figure()
h = surf(secrecyCapacity);
set(h, 'LineStyle','none');     % Removes black lines so you can see the data
ylabel("Bob's Location");
xlabel("Eve's Location");
zlabel("Secrecy Capacity (linear)");
title("Secrecy Capacity, linear");
xlim([0 4352]);
ylim([0 4352]);
view(45, 60);
colorbar;

% figure()
% h = surf(altSecrecyCapacity_dB);
% set(h, 'LineStyle','none');     % Removes black lines so you can see the data
% xlabel("Bob's Location");
% ylabel("Eve's Location");
% zlabel("Secrecy Capacity (dB)");
% title("Secrecy Capacity with 1 instead of 0");
% xlim([0 4352]);
% ylim([0 4352]);
% zlim([-150 0]);
% view(45, 60);
% colormap(jet);

figure()
contour(secrecyCapacity);
ylabel("Bob's Location");
xlabel("Eve's Location");
title("Secrecy Capacity (linear)");
title("Secrecy Capacity Contour, linear");
xlim([0 4352]);
ylim([0 4352]);
colorbar;

% figure()
% contour(secrecyCapacity_dB);
% xlabel("Bob's Location");
% ylabel("Eve's Location");
% title("Secrecy Capacity (dB)");
% title("Secrecy Capacity Contour, dB");
% xlim([0 4352]);
% ylim([0 4352]);
% colorbar;


% function [cap,capacity] = gaussian_capacity(data,snr)
% %UNTITLED Summary of this function goes here
% %   Detailed explanation goes here
% capacity = (1/2) .* log2(1 + (data .* snr));
% [num_columns,~] = size(data);
% cap = zeros(1,num_columns);
%     for col = 1:num_columns
%         if ~isnan(capacity(col))
%             cap(col) = sum(capacity(col,:));
%         end
%     end
% end



