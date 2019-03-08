% Created by Kalin Norman
% This file is used to create the Figures 5 and 6 in the paper.
% It obtains the capacity and secrecy arrays from the GaussianCapacity
% function, after which the two heatmaps are plotted using the
% CustomFloorHeatmap function.
close all; clear;

load Data/tx2_linear.mat; % Loads the linear data
addpath('Functions'); % Allows the functions to be called

SNR = 10^(2.4/10); % The chosen SNR value, 2.4 dB, converted to a linear value
[capacity, secrecyCapacity] = GaussianCapacity(tx2_linear, SNR);
CustomFloorHeatmap(capacity, false); % Plots the capacity, the second parameter
                                     % is set to false, as there is no
                                     % receiver indicated in the capacity
                                     % heatmap
                                     
% The minimums for each possible room in which Eve could be found are
% calculated. The minimum among them corresponds to the rate at which
% information could securely be transmitted to Bob without Even being able
% to receive enough information to decode the message
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

CustomFloorHeatmap(secrecyCapacity, true); % Plots the secrecy capacity.
                                           % The second parameter is set to
                                           % true, as this time we do want
                                           % to see the location of
                                           % reference for the secrecy
                                           % capacity.
