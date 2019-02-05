function [capacity,secrecyCapacity] = GaussianCapacity(linearData,SNR)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
tempsignal = linearData(1:2:end,:,:);
signal = tempsignal(11:55,:,:);
tempnoise = linearData(2:2:end,:,:);
noise = tempnoise(10:54,:,:);

[numCarriers, numRows, numColumns] = size(signal);

nonnormalizedGsquared = signal ./ noise;
g = sqrt(nonnormalizedGsquared);
bob = g(:,36:65,65:98);
gNormalized = g ./ max(max(max(bob)));
gSquared = gNormalized .^ 2;

% Capacity Calculations
capacityPerCarrier = (1/2) * log2(1 + (gSquared .* SNR));

bobCapacity = capacityPerCarrier(:,36:65,65:98);
bobMaxCapacity = max(max(max(bobCapacity)));
% Secrecy Capacity Calculation
for y = 1:numRows
    for z = 1:numColumns
        for x = 1:numCarriers
            secrecyCapacityPerCarrier(x,y,z) = bobMaxCapacity - capacityPerCarrier(x,y,z);
            if (secrecyCapacityPerCarrier(x,y,z) < 0)
                secrecyCapacityPerCarrier(x,y,z) = 0;
            end
        end
    end
end

% Final Capacity and Secrecy Capacity Arrays
for y = 1:numRows
    for z = 1:numColumns
        capacity(y,z) = sum(capacityPerCarrier(:,y,z));
        secrecyCapacity(y,z) = sum(secrecyCapacityPerCarrier(:,y,z));
    end
end

end

