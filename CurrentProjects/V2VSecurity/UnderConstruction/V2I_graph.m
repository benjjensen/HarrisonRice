clear; 
close all;
load('../Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat');
data = graphPwelchedData;
[numRows, numCarriers] = size(data);

    %% Calculate capacity
    
capacity = zeros(numRows, 1);

    % Calculate the capacity per carrier, then sum up for location capacity
for location = 1:numRows
        temp = 0;
    for carrier = 1:numCarriers
        temp = temp + 0.5*log2(1+data(location, carrier));
    end
    capacity(location) = temp;
end

    % Normalize 
maxValue = max(max(capacity));
capacity = capacity ./ maxValue;

    % Plot in dB to see differences better
capacity_dB = mag2db(capacity);

figure()
plot(capacity_dB);
[maxCapacity, indexOfMaxCapacity] = max(capacity);  % Tx is at indexOfMaxCapacity (hopefully)

    
    %% Calculate secrecy capacity
    
% capacity = capacity(1:10:end);
% numRows = 436;

secrecyCapacity = zeros(numRows);

for bobsLocation = 1:numRows
    for evesLocation = 1:numRows
        if (capacity(bobsLocation) <= capacity(evesLocation))
            secrecyCapacity(bobsLocation, evesLocation) = 0;
        else 
            secrecyCapacity(bobsLocation, evesLocation) = ...
                capacity(bobsLocation) - capacity(evesLocation);
        end
    end
end

secrecyCapacity_dB = mag2db(secrecyCapacity);
figure()
h = surf(secrecyCapacity_dB);
set(h, 'LineStyle','none');     % Removes black lines so you can see the data
xlabel("Bob's Location");
ylabel("Eve's Location");
xlim([0 4352]);
ylim([0 4352]);
 zlim([-150 0]);
view(45, 60);
colormap(jet);

figure()
h = surf(secrecyCapacity);
set(h, 'LineStyle','none');     % Removes black lines so you can see the data
xlabel("Bob's Location");
ylabel("Eve's Location");
xlim([0 4352]);
ylim([0 4352]);
view(45, 60);
colormap(jet);



% 4) Repeat with "North" data


