clear;
close all;

load linear_signal.mat; % loads in the signal data
load linear_noisefloor.mat; % loads in the noise data

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise

db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
snr = 10^(2.3/10); % chosen snr value

[~,tx2Floor] = find_num_carriers(signal,snr,threshold); % finds the number of good carriers in each location

clyde_heatmap(tx2Floor); % displays the heatmap of the data