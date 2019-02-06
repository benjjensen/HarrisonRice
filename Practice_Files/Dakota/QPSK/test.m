clear;
load('linearSignalReducedCarriers.mat');
load linear_signal.mat; % loads in the signal data
load linear_noisefloor.mat; % loads in the noise data

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
snr = logspace(-3,4,num_loops);
epsilon = 0.5;
parfor index = 1:num_loops
    tic;
%     index = index + 1
    disp(index);
    har = zeros(30,34);
    smal = zeros(32,25);
    cam = zeros(32,36);
    har_c = zeros(1,45);
% 
%     for carrier = 11:55
%         har = 2 * (1 - probability_erasure(snr(index),harrison(carrier,har_best(index,1),har_best(index,2)),epsilon));
%         sma = 2 * (1 - probability_erasure(snr(index),smalley(carrier,sma_best(index,1),sma_best(index,2)),epsilon));
%         sec_cap(index) = sec_cap(index) + secrecy_capacity(har,sma);
%         har_cap(index) = har_cap(index) + har;
%         smal_cap(index) = smal_cap(index) + sma;
%     end
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
%     har_cap(index) = harrison_max;
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
%                 cam(row,col) = cam(row,col) + 1 - probability_erasure(snr(index)...
%                     ,camacho(carrier,row,col),epsilon);
%                 if col == 25
%                     cam(row,col+1) = cam(row,col+1) + 1 - probability_erasure(snr(index)...
%                     ,camacho(carrier,row,col+1),epsilon);
%                 end
            end
        end
    end
%     
%     

    sec_cap(index) = min(min(sec_cap_sma));
    
    [smalley_max,I] = max(smal);
    [smalley_max,J] = max(smalley_max);
%     smal_cap(index) = smalley_max;
    smal_best_x(index) = I(J);
    smal_best_y(index) = J;
    
%     [camacho_max,I] = max(cam);
%     [camacho_max,J] = max(camacho_max);
%     cam_cap(index) = camacho_max;
%     cam_best_x(index) = I(J);
%     cam_best_y(index) = J;
    % = [I(J) J];
    
    for carrier = 1:45
%         har = 2 * (1 - probability_erasure(snr(index),harrison(carrier,har_best_x(index),har_best_y(index)),epsilon));
        sma = 2 * (1 - probability_erasure(snr(index),smalley(carrier,smal_best_x(index),smal_best_y(index)),epsilon));
%         sec_cap(index) = sec_cap(index) + secrecy_capacity(har,sma);
%         har_cap(index) = har_cap(index) + har;
        smal_cap(index) = smal_cap(index) + sma;
    end

    
    toc;
end
save('test1_workspace_12');

beep
%%
% har_best = [har_best_x' har_best_y'];
% % cam_best = [cam_best_x' cam_best_y'];
% smal_best = [smal_best_x' smal_best_y'];
% q_har_cap = zeros(1,num_loops);
% q_smal_cap = zeros(1,num_loops);
% q_cam_cap = zeros(1,num_loops);
% parfor index = 1:num_loops
%     sigma2 = snr(index);
%     for carrier = 11:55
%         q_har_cap(index) = q_har_cap(index) + 1 - erasure_probability(snr(index)...
%         * harrison_noise(carrier,har_best(index,1),har_best(index,2)),...
%         harrison(carrier,har_best(index,1),har_best(index,2)),epsilon);
%     
%         q_cam_cap(index) = q_cam_cap(index) + 1 - erasure_probability(snr(index)...
%         * camacho_noise(carrier,cam_best(index,1),cam_best(index,2)),...
%         camacho(carrier,cam_best(index,1),cam_best(index,2)),epsilon);
%     
%         q_smal_cap(index) = q_smal_cap(index) + 1 - erasure_probability(snr(index)...
%         * smalley_noise(carrier,smal_best(index,1),smal_best(index,2)),...
%         smalley(carrier,smal_best(index,1),smal_best(index,2)),epsilon);
%     end
% end
%     for carrier = 1:64
%     har_cap(index) = har_cap(index) + 1 - probability_erasure(sigma2,harrison(carrier,21,1),epsilon);
%     smal_cap(index) = smal_cap(index) + 1 - probability_erasure(sigma2,smalley(carrier,31,22),epsilon);
%     cam_cap(index) = cam_cap(index) + 1 - probability_erasure(sigma2,camacho(carrier,26,1),epsilon);
%     end
toc;
%%
clear
load test1_workspace_12;
snr = 10*log10(snr);

% figure()
% hold on;
% plot(har_cap);
% plot(smal_cap);
% % plot(cam_cap);
% hold off
% 
% 
% figure()
% hold on
% plot(sec_cap);
% % plot(har_cap - cam_cap);
% hold off


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



% conference = linearSignalReducedCarriers(:,1:33,1:98);
% conf = squeeze(mean(conference));
% index = 1;
% for row = 1:33
%     for col = 1:98
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = conf(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [conf2,K] = mean(conf);
% harrison = linearSignalReducedCarriers(:,36:65,65:98);
% har = squeeze(mean(harrison));
% index = 1;
% for row = 1:num_loops
%     for col = 1:34
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = har(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [har2,J] = mean(har);
% smalley = linearSignalReducedCarriers(:,34:65,123:147);
% sma = squeeze(mean(smalley));
% index = 1;
% for row = 1:32
%     for col = 1:25
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = sma(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [sma2,K] = mean(sma);
% camacho = linearSignalReducedCarriers(:,34:65,154:179);
% cam = squeeze(mean(camacho));
% index = 1;
% for row = 1:32
%     for col = 1:26
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = cam(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [cam2,K] = mean(cam);
% chambers = linearSignalReducedCarriers(:,34:65,213:240);
% cham = squeeze(mean(chambers));
% index = 1;
% for row = 1:32
%     for col = 1:28
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = cham(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [cham2,K] = mean(cham);
