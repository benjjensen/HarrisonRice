%% Plots the number of good carriers at each DB threshold
clear;
close all;

% load('camacho.mat');
% load('chambers.mat');
% load('conference.mat');
% load('hall_pwelch.mat');
% load('harrison.mat');
% load('smalley.mat');
% load('tx2camacho.mat');
% load('tx2chambers.mat');
% load('tx2conference.mat');
% load('tx2hallway.mat');
% load('tx2harrison.mat');
% load('tx2smalley.mat');

load linear_signal.mat;
load linear_noisefloor.mat;

for y = 1:90
    for z = 1:345
        linear_noisefloor(:,y,z) = fftshift(linear_noisefloor(:,y,z));
    end
end

signal = linear_signal(11:55,:,:);
noise = linear_noisefloor(10:54,:,:);

for y = 1:90
    for z = 1:345
        noise(:,y,z) = sum(noise(:,y,z)) / 45;
    end
end
signal = signal ./ noise;
signal = sqrt(signal);
signal = signal / max(max(max(signal(:,36:65,65:98))));
signal = signal.^2;

tx2harrison = signal(:,36:65,65:98);
tx2camacho = signal(:,34:65,154:179);
tx2smalley = signal(:,34:65,123:147);
tx2conference = signal(:,1:65,1:61);
tx2chambers = signal(:,34:65,213:240);

% tx2harrison_signal = sqrt(10.^(tx2harrison(:,:,1:2:127)/10));
% tx2harrison_noise = sqrt(10.^(tx2harrison(:,:,2:2:128)/10));
% for row = 1:30
%     for col = 1:34
%         tx2harrison_signal(row,col,11:55) = tx2harrison_signal(row,col,11:55)...
%             ./(sum(tx2harrison_noise(row,col,10:54))./45);
%     end
% end
% gmax = max(max(max(tx2harrison_signal)));
% tx2harrison = (tx2harrison_signal(:,:,11:55)/gmax).^2;
% tx2camacho_signal = (sqrt(10.^(tx2camacho(:,:,1:2:127)/10)));
% tx2camacho_noise = (sqrt(10.^(tx2camacho(:,:,2:2:128)/10)));
% for row = 1:32
%     for col = 1:26
%         tx2camacho_signal(row,col,11:55) = tx2camacho_signal(row,col,11:55)...
%             ./(sum(tx2camacho_noise(row,col,10:54))./45);
%     end
% end
% tx2chambers_signal = (sqrt(10.^(tx2chambers(:,:,1:2:127)/10))/gmax).^2;
% tx2chambers_noise = (sqrt(10.^(tx2chambers(:,:,2:2:128)/10))/gmax).^2;
% for row = 1:32
%     for col = 1:28
%         tx2chambers_signal(row,col,11:55) = tx2chambers_signal(row,col,11:55)...
%             ./(sum(tx2chambers_noise(row,col,10:54))./45);
%     end
% end
% tx2conference_signal = (sqrt(10.^(tx2conference(:,:,1:2:127)/10)));
% tx2conference_noise = (sqrt(10.^(tx2conference(:,:,2:2:128)/10)));
% for row = 1:65
%     for col = 1:61
%         tx2conference_signal(row,col,11:55) = tx2conference_signal(row,col,11:55)...
%             ./(sum(tx2conference_noise(row,col,10:54))./45);
%     end
% end
% tx2smalley_signal = (sqrt(10.^(tx2smalley(:,:,1:2:127)/10)));
% tx2smalley_noise = (sqrt(10.^(tx2smalley(:,:,1:2:127)/10)));
% for row = 1:32
%     for col = 1:26
%         tx2smalley_signal(row,col,11:55) = tx2smalley_signal(row,col,11:55)...
%             ./(sum(tx2smalley_noise(row,col,10:54))./45);
%     end
% end
% tx2camacho = (tx2camacho_signal(:,:,11:55)/gmax).^2;
% tx2smalley = (tx2smalley_signal(:,:,11:55)/gmax).^2;
% tx2chambers = (tx2chambers_signal(:,:,11:55)/gmax).^2;
% tx2conference = (tx2conference_signal(:,:,11:55)/gmax).^2;



i = 0;
num_loops = 300;
mm_cam = zeros(num_loops,1);
mm_cham = zeros(num_loops,1);
mm_conf = zeros(num_loops,1);
mm_har = zeros(num_loops,1);
mm_smal = zeros(num_loops,1);
mum_har = zeros(num_loops,1);
snr = logspace(-3,8,num_loops);
threshold = 10^(0/10);
% for threshold = 0:.1:35
for index = 1:num_loops
    i = i + 1;
    cam2 = find_num_carriers(tx2camacho,snr(index),threshold);
    chambers2 = find_num_carriers(tx2chambers,snr(index),threshold);
    conference2 = find_num_carriers(tx2conference,snr(index),threshold);
    harrison2 = find_num_carriers(tx2harrison,snr(index),threshold);
    smalley2 = find_num_carriers(tx2smalley,snr(index),threshold);
    
    % uncomment to find the carriers from the first transmitter location
%     camacho_best_carrier = find_best_carriers(cam1,camacho,3,threshold);
    camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,snr(index),threshold);
%     chambers_best_carrier = find_best_carriers(chambers1,chambers,2,threshold);
    chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,snr(index),threshold);
%     conference_best_carrier = find_best_carriers(conference1,conference,1,threshold);
    conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,snr(index),threshold);
%     harrison_best_carrier = find_best_carriers(harrison1,harrison,5,threshold);
    harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,snr(index),threshold);
%     smalley_best_carrier = find_best_carriers(smalley1,smalley,4,threshold);
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
%     [mm_cam(i,1),mm_cham(i,1),mm_conf(i,1),...
%         mm_har(i,1),mm_smal(i,1),mum_har(i,1)] = find_max_matched(...
%         camacho_best_carrier2,chambers_best_carrier2,...
%         conference_best_carrier2,harrison_best_carrier2,...
%         smalley_best_carrier2,4);
    
end
%%
snr = 10*log10(snr);
figure(1)
hold on;
% plot(snr,mm_cam,'DisplayName','Camacho');
% plot(snr,mm_cham,'DisplayName','Chambers');
% plot(snr,mm_conf,'DisplayName','Conference Room');
plot(snr,mm_har,'DisplayName','Bob');
plot(snr,eve,'DisplayName','Eve');
% plot(snr,mm_smal,'DisplayName','Smalley');
plot(snr,mum_har, 'DisplayName', 'Secrecy Capacity');
xlabel('SNR (dB)');
ylabel('Bits per channel use');
% xlim(x);
legend;
hold off;
