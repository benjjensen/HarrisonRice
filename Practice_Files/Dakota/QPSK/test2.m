clear;
close all;

load linear_signal.mat;
load linear_noisefloor.mat;
load truncated_linear_signal.mat;
load truncated_linear_noise.mat
signal = linear_signal;
% signal = truncated_linear_signal;
noise = linear_noisefloor;
% noise = truncated_linear_noise;
num_loops = 1000;
harrison_cap = zeros(1,num_loops);
smalley_cap = zeros(1,num_loops);
camacho_cap = zeros(1,num_loops);
sec_cap_cam = zeros(1,num_loops);
sec_cap_sma = zeros(1,num_loops);
index = 0;
q = logspace(-2,4,num_loops);
% for sigma2 = logspace(-2,4,num_loops)
tic;
parfor index = 1:num_loops
    %% Capacity
%     index = index + 1;
    sigma2 = q(index);
    cpacity = (1/2) * log2(1 + (signal ./ (noise*sigma2)));
    capacity = zeros(90,345);
    for y = 1:90
        for z = 1:345
            capacity(y,z) = sum(cpacity(:,y,z));
        end
    end
    
    harrison_max = max(max(capacity(36:65,65:98)));
    harrison_cap(index) = harrison_max;
    %     [X Y] = find(capacity(36:65,65:98) == harrison_max);
    %     max_X = X + 36 - 1;
    %     max_Y = Y + 65 - 1;
    
    %% Secrecy Capacity
    % harrison = tx2_linear_signal(:,36:65,65:98);
    smalley = capacity(34:65,123:147);
    camacho = capacity(34:65,154:179);
    
    smal_max = max(max(smalley));
    cam_max = max(max(camacho));
    
    
    smalley_cap(index) = smal_max;
    camacho_cap(index) = cam_max;
    sec_cap_cam(index) = harrison_max - cam_max;
    sec_cap_sma(index) = harrison_max - smal_max;
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
figure()
hold on;
yyaxis right
plot(logspace(-2,4,num_loops),harrison_cap,'-');
plot(logspace(-2,4,num_loops),camacho_cap,'-');
plot(logspace(-2,4,num_loops),smalley_cap,'-');
ylabel('Bits per channel use')
xlabel('Power attenuation');
set(gca,'XScale','log');
% hold off;

% figure()
% hold on;
yyaxis left
plot(logspace(-2,4,num_loops),min(sec_cap_cam,sec_cap_sma));
% plot(logspace(-3,10,num_loops),sec_cap_sma,'r');
ylabel('Secure bits per channel use');
xlabel('Power attenuation');
set(gca,'XScale','log');
hold off
