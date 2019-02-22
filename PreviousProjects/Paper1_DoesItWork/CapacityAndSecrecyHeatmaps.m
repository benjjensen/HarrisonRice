close all; clear;

load Data/tx2_linear.mat;

addpath('Functions');

SNR = 10^(2.3/10);
[capacity, secrecyCapacity] = GaussianCapacity(tx2_linear, SNR);
Heatmap(capacity);
Heatmap(secrecyCapacity);

MinSmalley = min(min(secrecyCapacity(34:65,123:148)));
MinConference = min(min(secrecyCapacity(5:65,1:61)));
MinCamacho = min(min(secrecyCapacity(34:65,154:179)));
MinChambers = min(min(secrecyCapacity(34:65,213:240)));
MinHarrison = min(min(secrecyCapacity(36:65,65:98)));
