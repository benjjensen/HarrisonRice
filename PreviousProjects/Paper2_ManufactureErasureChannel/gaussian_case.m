% Created by Dakota Flanary
clear;
close all;

addpath(genpath('Functions'))

load('Data/linear_signal.mat'); % loads in the signal data
load('Data/linear_noisefloor.mat'); % loads in the noise data

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise

num_loops = 200; % number of data points to test
harrison_cap = zeros(1,num_loops); % matrix for harrison's capacity
smalley_cap = zeros(1,num_loops); % matrix for smalley's
camacho_cap = zeros(1,num_loops); % matrix for camacho's
sec_cap_cam = zeros(1,num_loops); % matrix for secrecy capacity in camacho's room
sec_cap_sma = zeros(1,num_loops); % matrix for smalley's
sec_cap = zeros(1,num_loops); % worst case secrecy capacity
eve_cap = zeros(1,num_loops); % eve's capacity

snr = logspace(-3,4,num_loops); % snr values to test(in linear) - make sure to match cross and thresh
tic;
for index = 1:num_loops
    %% Capacity
    % calculate the capacity for each carrier in each location
    floor_carrier_capacity = (1/2) .* log2(1 + (signal .* snr(index)));
    capacity = nan(90,345); % matrix for sum of carrier capacity in each location
    for y = 1:65
        for z = 1:240
            if ~isnan(floor_carrier_capacity(1,y,z))
                % sum up the capacity on the carriers in each location
                capacity(y,z) = sum(floor_carrier_capacity(:,y,z));
            end
        end
    end
    
    harrison_max = max(max(capacity(36:65,65:98))); % finds bob's max capacity
    harrison_cap(index) = harrison_max; 
    
    %% Secrecy Capacity
    harrison = floor_carrier_capacity(:,36:65,65:98); % separate harrison's room
    smalley = floor_carrier_capacity(:,34:65,123:147); % separate smalley's room
    camacho = floor_carrier_capacity(:,34:65,154:179); % separate camacho's room
    
    eve_cp = floor_carrier_capacity; % initialize eve's capacity to be the floor's capacity
    eve_cp(:,36:65,65:98) = 0; % set the capacities in harrison's office to 0
                                % because we don't need to check those for
                                % secrecy capacity
    
    har_temp = capacity(36:65,65:98);
    sma_temp = capacity(34:65,123:147);
    cam_temp = capacity(34:65,154:179);
    
    eve_temp = capacity;
    eve_temp(36:65,65:98) = 0;
    
    [temp,I] = max(har_temp);
    [temp,J] = max(temp);
    har_best_x(index) = J; % find the best spot in harrison's office
    har_best_y(index) = I(J);
     
    [temp,I] = max(eve_temp);
    [temp,J] = max(temp);
    eve_cap(index) = temp; % find eve's best spot
    eve_best_x(index) = J;
    eve_best_y(index) = I(J);
    
    for carrier = 1:45 % calculate the secrecy capacity between harrison's
        % best spot and eve's best location
        sec = harrison(carrier,har_best_y(index),har_best_x(index)) - ...
            eve_cp(carrier,eve_best_y(index),eve_best_x(index));
        if sec < 0
            sec = 0;
        end
        sec_cap(index) = sec_cap(index) + sec;
        
    end
    
end
toc;
% save('test2_workspace_new');

snr = 10*log10(snr);
%%
figure()
hold on;
yyaxis right
plot(snr,harrison_cap,'--');
plot(snr,eve_cap,'-');
ylabel('Bits per channel use')
xlabel('SNR (dB)');
yyaxis left
plot(snr,sec_cap);
ylabel('Secure bits per channel use');
xlabel('SNR (dB)');
% set(gca,'XScale','log');
hold off

gaussian_capacity_data = [snr;harrison_cap;smalley_cap;sec_cap];
