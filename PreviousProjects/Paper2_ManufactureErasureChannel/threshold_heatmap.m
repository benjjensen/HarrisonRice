% Created by Dakota Flanary
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

db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
snr = 10^(2.3/10); % chosen snr value

[~,tx2Floor] = find_num_carriers(signal,snr,threshold); % finds the number of good carriers in each location

Heatmap(tx2Floor);