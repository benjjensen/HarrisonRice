close all; clear;

load Data/tx2_linear.mat;

addpath('Functions');

SNR = 10^(2.4/10);
[capacity, secrecyCapacity] = GaussianCapacity(tx2_linear, SNR);
CustomFloorHeatmap(capacity, false);
% The following line allows bob's best location to be seen in black in the Heatmap
secrecyCapacity(39:41,74:76) = nan; % Sets 3x3 around Bob's best location to nan
CustomFloorHeatmap(secrecyCapacity, true);

MinSmalley = min(min(secrecyCapacity(34:65,123:148)));
MinConference = min(min(secrecyCapacity(5:65,1:61)));
MinCamacho = min(min(secrecyCapacity(34:65,154:179)));
MinChambers = min(min(secrecyCapacity(34:65,213:240)));
MinHarrison = min(min(secrecyCapacity(36:65,65:98)));
% Harrison's min currently at (40,75)
