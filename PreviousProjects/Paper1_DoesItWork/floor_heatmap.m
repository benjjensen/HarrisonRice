% Generates the data for the threshold heatmap and then calls the heatmap
% function to plot the data
% Created by Dakota Flanary
clear;
close all;
addpath('Functions');
load ('Data/tx2_noise_lin_gc.mat'); % loads in the signal data
load ('Data/tx2_signal_lin_gc.mat'); % loads in the noise data

[signal,noise] = normalize_signal(tx2_signal_lin_gc,tx2_noise_lin_gc); %signal = g^2 and noise is the sigma^2

tx2harrison = signal(:,36:65,65:98); % separate the data from harrison's office
tx2camacho = signal(:,34:65,154:179); % separate the data from camacho's office
tx2smalley = signal(:,34:65,123:147); % separate the data from smalley's office
tx2conference = signal(:,1:65,1:61); % separate the data from the conference room
tx2chambers = signal(:,34:65,213:240); % separate the data from chamber's office

db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
snr = 10^(2.4/10); % chosen snr value

[~,tx2Floor] = find_num_carriers(signal,snr,threshold); % finds the number of good carriers in each location

FloorplanHeatmap(tx2Floor);