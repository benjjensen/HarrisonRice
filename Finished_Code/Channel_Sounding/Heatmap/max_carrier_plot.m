%% Plots the number of good carriers and unique good carriers for harrison 
%% at each SNR level
clear;
close all;

load tx2_linear_signal.mat;
load linear_noisefloor.mat;

% for y = 1:90
%     for z= 1:345
%         tx2_linear_signal(:,y,z) = fftshift(tx2_linear_signal(:,y,z)); % move the bad carriers to the edge
%         linear_noisefloor(:,y,z) = fftshift(linear_noisefloor(:,y,z)); % move the bad carriers to the edge
%     end
% end
% 
% noise = linear_noisefloor(10:54,:,:); % remove the bad carriers from the noise
% for y = 1:90
%     for z = 1:345
%         noise(:,y,z) = sum(noise(:,y,z)) ./ 45; % average the noise floor
%     end
% end
% 
% signal = tx2_linear_signal(11:55,:,:); % remove the bad carriers
% signal = signal ./ noise; % remove the noise from the signal
% signal = sqrt(signal); % find the g's
% signal = signal ./ max(max(max(signal(:,36:65,65:98)))); % find the best g in harrison's room
% signal = signal.^2;

[signal,noise] = shift_normalize_signal_noise(tx2_linear_signal,linear_noisefloor);

tx2harrison = signal(:,36:65,65:98);
tx2camacho = signal(:,34:65,154:179);
tx2smalley = signal(:,34:65,123:147);
tx2conference = signal(:,1:65,1:61);
tx2chambers = signal(:,34:65,213:240);

i = 0;
num_loops = 200;
mm_cam = zeros(num_loops,1);
mm_cham = zeros(num_loops,1);
mm_conf = zeros(num_loops,1);
mm_har = zeros(num_loops,1);
mm_smal = zeros(num_loops,1);
mum_har = zeros(num_loops,1);
eve = zeros(num_loops,1);
snr = logspace(-3,5,num_loops);
threshold = 10^(0/10);
for index = 1:num_loops
    i = i + 1;
    cam2 = find_num_carriers(tx2camacho,snr(index),threshold);
    chambers2 = find_num_carriers(tx2chambers,snr(index),threshold);
    conference2 = find_num_carriers(tx2conference,snr(index),threshold);
    harrison2 = find_num_carriers(tx2harrison,snr(index),threshold);
    smalley2 = find_num_carriers(tx2smalley,snr(index),threshold);
    
    camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,snr(index),threshold);
    chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,snr(index),threshold);
    conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,snr(index),threshold);
    harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,snr(index),threshold);
    smalley_best_carrier2 = find_best_carriers(smalley2,tx2smalley,4,snr(index),threshold);
    
    smal_sec = find_sec_cap(harrison_best_carrier2,smalley_best_carrier2);
    cam_sec = find_sec_cap(harrison_best_carrier2,camacho_best_carrier2);
    cham_sec = find_sec_cap(harrison_best_carrier2,chambers_best_carrier2);
    conf_sec = find_sec_cap(harrison_best_carrier2,conference_best_carrier2);
    
    mum_har(i) = 2*min([smal_sec,cam_sec,cham_sec,conf_sec]);
    
    mm_cam(i) = 2*max(cam2);
    mm_cham(i) = 2*max(chambers2);
    mm_conf(i) = 2*max(conference2);
    mm_har(i) = 2*max(harrison2);
    mm_smal(i) = 2*max(smalley2);
    
    eve(i) = max([mm_cam(i),mm_cham(i),mm_conf(i),mm_smal(i)]);
    
end
%%
snr = 10*log10(snr);
figure()
hold on;
plot(snr,mm_har,'DisplayName','Bob');
plot(snr,eve,'DisplayName','Eve');
plot(snr,mum_har, 'DisplayName', 'Secrecy Capacity');
xlabel('SNR (dB)');
ylabel('Bits per channel use');
legend;
hold off;
