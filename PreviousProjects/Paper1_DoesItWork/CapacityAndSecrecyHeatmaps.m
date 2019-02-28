close all; clear;

load Data/tx2_linear.mat;

addpath('Functions');

SNR = 10^(2.4/10);
[capacity, secrecyCapacity] = GaussianCapacity(tx2_linear, SNR);
CustomFloorHeatmap(capacity, false);

MinSmalley = min(min(secrecyCapacity(34:65,123:148)));
MinConference = min(min(secrecyCapacity(5:65,1:61)));
MinCamacho = min(min(secrecyCapacity(34:65,154:179)));
MinChambers = min(min(secrecyCapacity(34:65,213:240)));
MinHarrison = min(min(secrecyCapacity(36:65,65:98)));

% Sets 3x3 around Bob's best location to nan in order to allow it to be
% seen in the heatmap, when this is done, set the second parameter in the
% CustomFloorHeatmap to true. Otherwise leave it as false.
for x = 35:45
    for y = 65:98
        if (secrecyCapacity(x,y) == MinHarrison)
            secrecyCapacity(x-1:x+1,y-1:y+1) = nan;
        end
    end
end

CustomFloorHeatmap(secrecyCapacity, true);


