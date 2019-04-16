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
tx2camacho = signal(:,34:65,154:179); % separate the data from camacho's office
tx2smalley = signal(:,34:65,123:147); % separate the data from smalley's office
tx2conference = signal(:,1:65,1:61); % separate the data from the conference room
tx2chambers = signal(:,34:65,213:240); % separate the data from chamber's office

num_loops = 200; % number of data points to test
cap_cam = zeros(num_loops,1); % capcity in camcho's office
cap_cham = zeros(num_loops,1); % capacity in chamber's office
cap_conf = zeros(num_loops,1); % capacity in the conference room
cap_har = zeros(num_loops,1); % capacity in harrison's office
cap_smal = zeros(num_loops,1); % capacity in smalley's office
sec_har = zeros(num_loops,1); % the secrecy capacity for harrison
eve = zeros(num_loops,1); % eve's capacity
snr = logspace(-3,4, num_loops); % the range of snr values to test
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
for index = 1:num_loops
    % Find the number of carriers(capacity) in each office at each location
    cam2 = find_num_carriers(tx2camacho,snr(index),threshold);
    chambers2 = find_num_carriers(tx2chambers,snr(index),threshold);
    conference2 = find_num_carriers(tx2conference,snr(index),threshold);
    harrison2 = find_num_carriers(tx2harrison,snr(index),threshold);
    smalley2 = find_num_carriers(tx2smalley,snr(index),threshold);
    
    % Find the carriers that are good in each office's best location
    camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,snr(index),threshold);
    chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,snr(index),threshold);
    conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,snr(index),threshold);
    harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,snr(index),threshold);
    smalley_best_carrier2 = find_best_carriers(smalley2,tx2smalley,4,snr(index),threshold);
    
    % find the secrecy capacity corresponding to each office 
    smal_sec = threshold_find_sec_cap(harrison_best_carrier2,smalley_best_carrier2);
    cam_sec = threshold_find_sec_cap(harrison_best_carrier2,camacho_best_carrier2);
    cham_sec = threshold_find_sec_cap(harrison_best_carrier2,chambers_best_carrier2);
    conf_sec = threshold_find_sec_cap(harrison_best_carrier2,conference_best_carrier2);
    
    % find the min secrecy capacity which is the limit of the secrecy
    % capacity
    sec_har(index) = 2*min([smal_sec,cam_sec,cham_sec,conf_sec]);
    
    % multiply the capacities by 2 because we are doing QPSK
    cap_cam(index) = 2*max(cam2);
    cap_cham(index) = 2*max(chambers2);
    cap_conf(index) = 2*max(conference2);
    cap_har(index) = 2*max(harrison2);
    cap_smal(index) = 2*max(smalley2);
    
    % Eve's capacity is the max of all the offices not harrison's
    eve(index) = max([cap_cam(index),cap_cham(index),cap_conf(index),cap_smal(index)]);
    
end
%%

% plot the capacities and secrecy capacities
snr = 10*log10(snr);
figure()
hold on;
plot(snr,cap_har,'DisplayName','Bob');
plot(snr,eve,'DisplayName','Eve');
plot(snr,sec_har, 'DisplayName', 'Secrecy Capacity');
xlabel('SNR (dB)');
ylabel('Bits per channel use');
axis([-10 20 0 100]);
legend;
hold off;



