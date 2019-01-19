clear;
close all;

load tx2_linear_signal.mat;
load linear_noisefloor.mat;
tx2_linear_signal = fftshift(tx2_linear_signal);
signal = sqrt(tx2_linear_signal);
harrison = signal(:,36:65,65:98);
r_max = max(max(max(harrison)));
signal = signal/r_max;
signal = signal.^2;
% signal = truncated_linear_signal;
noise = linear_noisefloor;
% noise = truncated_linear_noise;
num_loops = 1000;
harrison_cap = zeros(1,num_loops);
smalley_cap = zeros(1,num_loops);
camacho_cap = zeros(1,num_loops);
sec_cap_cam = zeros(1,num_loops);
sec_cap_sma = zeros(1,num_loops);
sec_cap = zeros(1,num_loops);
index = 0;
snr = logspace(-3,7,num_loops);
% for sigma2 = logspace(-2,4,num_loops)
tic;
parfor index = 1:num_loops
    %% Capacity
%     index = index + 1;
    sigma2 = snr(index);
    cpacity = (1/2) * log2(1 + (signal * sigma2));
    capacity = zeros(90,345);
    for y = 1:90
        for z = 1:345
            capacity(y,z) = sum(cpacity(11:55,y,z));
        end
    end
    
    harrison_max = max(max(capacity(36:65,65:98)));
    harrison_cap(index) = harrison_max;
    %     [X Y] = find(capacity(36:65,65:98) == harrison_max);
    %     max_X = X + 36 - 1;
    %     max_Y = Y + 65 - 1;
    
    %% Secrecy Capacity
    % harrison = tx2_linear_signal(:,36:65,65:98);
    harrison = cpacity(11:55,36:65,65:98);
    smalley = cpacity(11:55,34:65,123:147);
    camacho = cpacity(11:55,34:65,154:179);
    
    har_temp = capacity(36:65,65:98);
    sma_temp = capacity(34:65,123:147);
    cam_temp = capacity(34:65,154:179);
    
    [temp,I] = max(har_temp);
    [temp,J] = max(temp);
%     har_cap(index) = harrison_max;
    har_best_x(index) = I(J);
    har_best_y(index) = J;
%     
    [temp,I] = max(sma_temp);
    [temp,J] = max(temp);
%     smal_cap(index) = smalley_max;
    smal_best_x(index) = I(J);
    smal_best_y(index) = J;
    
    [temp,I] = max(cam_temp);
    [temp,J] = max(temp);
%     cam_cap(index) = camacho_max;
    cam_best_x(index) = I(J);
    cam_best_y(index) = J;
    
    for carrier = 1:45
        sec = harrison(carrier,har_best_x(index),har_best_y(index)) - ...
            smalley(carrier,smal_best_x(index),smal_best_y(index));
        if sec < 0
            sec = 0;
        end
        sec_cap(index) = sec_cap(index) + sec;
        
    end
    
%     smal_max = max(max(smalley));
%     cam_max = max(max(camacho));
%     
%     
%     smalley_cap(index) = smal_max;
%     camacho_cap(index) = cam_max;
%     sec_cap_cam(index) = harrison_max - cam_max;
%     sec_cap_sma(index) = harrison_max - smal_max;
    %
    %     for x = 1:90
    %         for y = 1:345
    %             secrecy_capacity(x,y) = harrison_max - capacity(x,y);
    %             if (secrecy_capacity(x,y) < 0)
    %                 secrecy_capacity(x,y) = 0;
    %             end
    %         end
    %     end
end
toc;
save('test2_workspace_new');
%%
snr = 10*log10(snr);
figure()
hold on;
yyaxis right
plot(snr,harrison_cap,'-');
plot(snr,camacho_cap,'-');
plot(snr,smalley_cap,'-');
ylabel('Bits per channel use')
xlabel('SNR (dB)');
% set(gca,'XScale','log');
% hold off;

% figure()
% hold on;
yyaxis left
plot(snr,sec_cap);
% plot(logspace(-3,10,num_loops),sec_cap_sma,'r');
ylabel('Secure bits per channel use');
xlabel('SNR (dB)');
% set(gca,'XScale','log');
hold off
