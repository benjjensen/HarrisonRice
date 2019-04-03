function [capacity,secrecyCapacity] = GaussianCapacity(linearData,SNR)
% Created by Kalin Norman
% This function is used to convert linear data with a chosen SNR into the
% capacity and secrecy capacity arrays for the data that was passed in.
% Parameters:
% linearData should contain both the signal and the noise carriers
% SNR is the signal-to-noise ratio used in the calculations

tempsignal = linearData(1:2:end,:,:); % First separates the signal (64 carriers)
signal = tempsignal(11:55,:,:); % Then keeps only the carriers that are useful
tempnoise = linearData(2:2:end,:,:); % Separates out the noise (64 carriers)
noise = tempnoise(10:54,:,:); % Keeps only the carriers that are useful

[numCarriers, numRows, numColumns] = size(signal);

% Normalizes the signal (g^2) before calculating the capacity
nonnormalizedGsquared = signal ./ noise; % Divides out the noise to account for the AGC (automatic gain control)
g = sqrt(nonnormalizedGsquared);
bob = g(:,36:65,65:98); % Isolates bob's locations
gNormalized = g ./ max(max(max(bob))); % Normalizes with respect to bob's locations
gSquared = gNormalized .^ 2; % Normalized g^2 values

%% Capacity Calculations
% Capacity = .5 * log2(1 + g^2 * SNR)
capacityPerCarrier = (1/2) * log2(1 + (gSquared .* SNR));

bobCapacity = capacityPerCarrier(:,36:65,65:98);
bobMaxCarrier = max(max(max(bobCapacity)));
[bobcarriers, bobrows, bobcolumns] = size(bobCapacity);
% Find the location of Bob's max carrier
for y = 1:bobrows
    for z = 1:bobcolumns
        for x = 1:bobcarriers
            if (bobCapacity(x,y,z) == bobMaxCarrier)
                bobMaxRow = y;
                bobMaxCol = z;
            end
        end
    end
end
% Finds the capacity at the location of Bob's best carrier
bobMaxCapacity = bobCapacity(:,bobMaxRow,bobMaxCol);

%% Secrecy Capacity Calculation
% Secrecy Capacity = Capacity at location of reference - capacity everywhere else 
% (on a carrier-per-carrier basis), any negative results are set to 0
% Then sum up the values for the carriers at each location
for y = 1:numRows
    for z = 1:numColumns
        for x = 1:numCarriers
            secrecyCapacityPerCarrier(x,y,z) = bobMaxCapacity(x) - capacityPerCarrier(x,y,z);
            if (secrecyCapacityPerCarrier(x,y,z) < 0)
                secrecyCapacityPerCarrier(x,y,z) = 0;
            end
        end
    end
end

%% Final Capacity and Secrecy Capacity Arrays
% Sums up the results at each location (sum of the values for all of the
% carriers)
for y = 1:numRows
    for z = 1:numColumns
        capacity(y,z) = sum(capacityPerCarrier(:,y,z));
        secrecyCapacity(y,z) = sum(secrecyCapacityPerCarrier(:,y,z));
    end
end

end

