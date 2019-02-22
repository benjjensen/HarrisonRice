% Created by Dakota Flanary
%% Plots the number of good carriers and unique good carriers for harrison 
%% at each SNR level
clear;
close all;

load('Data/linear_signal.mat'); % loads in the signal data
load('Data/linear_noisefloor.mat'); % loads in the noise data
addpath(genpath('Functions'))

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise

tx2harrison = signal(:,36:65,65:98); % separate the data from harrison's office

snr_db = 2.3;
snr = 10^(snr_db/10);
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
    % Find the number of carriers(capacity) in bob's office at each location
harrison2 = find_num_carriers(tx2harrison,snr,threshold);
    
    % Find the carriers that are good in bob's office's best location
harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,snr,threshold);
    
    % find the secrecy capacity corresponding to each location 
[~,secrecy_capacity] = threshold_floor_sec_cap(harrison_best_carrier2,signal,snr,threshold); 
% returns the number of good matching carriers in each location 

bob_cap = sum(harrison_best_carrier2)/5; % finds bob's capacity (the 5 is 
    % a magic number used for another purpose that is part of the 
    % find_best_carriers function where each good carrier was given the 
    % value 5 so it needs to be divided out)
secrecy_capacity = bob_cap - secrecy_capacity; % the secrecy capacity is 
    % bob's capacity minus the matching good carriers of eve's
%%

Heatmap(secrecy_capacity); % calls the heatmap function
