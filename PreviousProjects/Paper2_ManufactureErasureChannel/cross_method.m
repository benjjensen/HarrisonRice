% Created by Dakota Flanary
% clear;
load('Data/linear_signal.mat'); % loads in the signal data
load('Data/linear_noisefloor.mat'); % loads in the noise data
addpath(genpath('Functions'))

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise
% load('har_best.mat');
% load('sma_best.mat');
close all
tic;
num_loops = 200;

signal = sqrt(signal); % find the g's

harrison = signal(:,36:65,65:98); % get harrison's room
smalley = signal(:,34:65,123:147); % get smalley's room
camacho = signal(:,34:65,154:179); % get camcho's room
conference = signal(:,1:65,1:61);


har_cap = zeros(1,num_loops); % vector of harrison's capacities
smal_cap = zeros(1,num_loops); % vector of smalley's capacities
cam_cap = zeros(1,num_loops); % vector of camcho's capacities
sec_cap = zeros(1,num_loops); % vector of the secrecy capacities
har_best_x = zeros(1,num_loops);
har_best_y = zeros(1,num_loops);
smal_best_x = zeros(1,num_loops);
smal_best_y = zeros(1,num_loops);
cam_best_x = zeros(1,num_loops);
cam_best_y = zeros(1,num_loops);
har_car = zeros(45,num_loops);
sma_car = zeros(45,num_loops);
snr = logspace(-3,4,num_loops);     % Make sure this matches the threshold and gaussian case
epsilon = 0.5;
parfor index = 1:num_loops
    tic;
    disp(index);
    har = zeros(30,34);
    smal = zeros(32,25);
    cam = zeros(32,36);
    har_c = zeros(1,45);

    for row = 1:30
        for col = 1:34
            for carrier = 1:45
                har(row,col) = har(row,col) + 1 - probability_erasure(snr(index),...
                    harrison(carrier,row,col),epsilon);
            end
        end
    end
    
    [harrison_max,I] = max(har);
    [harrison_max,J] = max(harrison_max);
    har_best_x(index) = I(J);
    har_best_y(index) = J;
    
    sec_cap_sma = zeros(32,25);
    
    for carrier = 1:45
        har_c(carrier) = 2 * (1 - probability_erasure(snr(index),harrison(carrier,har_best_x(index),har_best_y(index)),epsilon));       
    end
    har_cap(index) = sum(har_c);
    
    for row = 1:32
        for col = 1:25
            for carrier = 1:45
                sma = 2 * (1 - probability_erasure(snr(index),smalley(carrier,row,col),epsilon));
                smal(row,col) = smal(row,col) + sma;
                sec_cap_sma(row,col) = sec_cap_sma(row,col) + secrecy_capacity(har_c(carrier),sma);
            end
        end
    end    

    sec_cap(index) = min(min(sec_cap_sma));
    
    [smalley_max,I] = max(smal);
    [smalley_max,J] = max(smalley_max);
    smal_best_x(index) = I(J);
    smal_best_y(index) = J;
    
    for carrier = 1:45
        sma = 2 * (1 - probability_erasure(snr(index),smalley(carrier,smal_best_x(index),smal_best_y(index)),epsilon));
        smal_cap(index) = smal_cap(index) + sma;
    end   
    toc;
end
save('test1_workspace_12');
toc;
%%
clear
load test1_workspace_12;
snr = 10*log10(snr);

figure()
hold on
plot(snr,har_cap);
plot(snr,smal_cap);
% plot(snr,cam_cap);
ylabel('Bits per channel use')
xlabel('SNR (dB)');
% set(gca,'XScale','log');
hold off

figure()
hold on
plot(snr,sec_cap);
% plot(snr,har_cap - cam_cap);
ylabel('Secure bits per channel use')
xlabel('SNR (dB)');
% set(gca,'XScale','log');
hold off