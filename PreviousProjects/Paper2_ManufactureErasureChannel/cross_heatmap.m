% Created by Dakota Flanary
clear;
close all;
addpath(genpath('Functions'))

load('Data/linear_signal.mat'); % loads in the signal data
load('Data/linear_noisefloor.mat'); % loads in the noise data

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise
signal = sqrt(signal); % normalize all of the gs
num_carriers = 45;
% num_rows = 65; % for ignoring hallway
% num_cols = 240; % for ignoring hallway
num_rows = 90; % to include hallway
num_cols = 345; % to include hallway
snr = 10^(6/10); % chosen snr value
epsilon = 0.5; % chosen epsilon value

map_p_e = zeros(num_carriers,num_rows,num_cols);

tic;
for row = 1:num_rows % for each row on the floor
    row
    for col = 1:num_cols % for each column
        for carrier = 1:45 % for each of the good carriers
            if(isnan(signal(carrier,row,col)))
                map_p_e(:,row,col) = nan; % if the spot is nan it stays nan
                break;
            else
                map_p_e(carrier,row,col) = probability_erasure(snr,...
                    signal(carrier,row,col),epsilon); % calculate the probability
                % of erasure for each carrier in each location
            end
        end
    end
end
toc;
map_capacity_e = 2 * (1 - map_p_e); % a map of the capacity at each location for each carrier

%% Generate Capacity and secrecy capacity

capacity = squeeze(sum(map_capacity_e)); % harrison's capacity
harrison_cap = capacity(36:65,65:98);
harrison_p_e = map_p_e(:,36:65,65:98); % harrison's probability of erasures
[harrison_max,I] = max(harrison_cap); % step to find harrison's best location
[harrison_max,J] = max(harrison_max); % step to find harrison's best location
index = [J I(J)]; % harrison's best location [x,y] or [col,row]

sec_cap = zeros(num_rows,num_cols); % map of secrecy capacity

for row = 1:num_rows % for each row again
    for col = 1:num_cols % for each column
        if isnan(capacity(row,col)) % if it's nan then set sec_cap to nan at that spot
            sec_cap(row,col) = nan;
        else
            for carrier = 1:45 % calculate the secrecy capacity for each carrier
                sec_cap(row,col) = sec_cap(row,col) + secrecy_capacity(...
                    1-harrison_p_e(carrier,index(2),index(1)),1-map_p_e(carrier,row,col));
            end
        end
    end
end

sec_cap = 2 * sec_cap; % multiply by two because two bits per symbol

%% HEATMAP

Heatmap(sec_cap);

conference_sec_cap = sec_cap(1:33,1:98);
[conf,K] = min(conference_sec_cap);
[conf,K] = min(conf)
smalley_sec_cap = sec_cap(34:65,123:147);
[sma,K] = min(smalley_sec_cap);
[sma,K] = min(sma)
camacho_sec_cap = sec_cap(34:65,154:179);
[cam,K] = min(camacho_sec_cap);
[cam,K] = min(cam)
chambers_sec_cap = sec_cap(34:65,213:240);
[cham,K] = min(chambers_sec_cap);
[cham,K] = min(cham)

