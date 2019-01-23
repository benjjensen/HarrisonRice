clear;
close all;

load tx2_linear_signal.mat;
load linear_noisefloor.mat;

for y = 1:90
    for z = 1:345
        tx2_linear_signal(:,y,z) = fftshift(tx2_linear_signal(:,y,z)); % shift the data so the bad carriers are on the edges
        linear_noisefloor(:,y,z) = fftshift(linear_noisefloor(:,y,z)); % shift the data so the bad carriers are on the edges
    end
end
noise = linear_noisefloor(10:54,:,:);
for y = 1:90
    for z = 1:345
        noise(:,y,z) = sum(noise(:,y,z)) / 45; % average the noise floor
    end
end
signal = tx2_linear_signal(11:55,:,:); % remove the bad carriers
signal = signal ./ noise; % remove the noise from the data
signal = sqrt(signal); % find the g's 
harrison = signal(:,36:65,65:98); % separate the data for harrison's room
r_max = max(max(max(harrison))); % find the max g value in harrison's room
signal = signal./r_max; % normalize the g's with respect to harrison's max
signal = signal(:,1:65,1:240); % remove the hallway data
signal = signal.^2; 
num_loops = 200; % number of data points to test
harrison_cap = zeros(1,num_loops); % vector for harrison's capacity
smalley_cap = zeros(1,num_loops); % vector for sma
camacho_cap = zeros(1,num_loops);
sec_cap_cam = zeros(1,num_loops);
sec_cap_sma = zeros(1,num_loops);
sec_cap = zeros(1,num_loops);
eve_cap = zeros(1,num_loops);
index = 0;
snr = logspace(-3,5,num_loops); % snr values to test(in linear)
% for sigma2 = logspace(-2,4,num_loops)
tic;
for index = 1:num_loops
    %% Capacity
%     index = index + 1;
    cpacity = (1/2) .* log2(1 + (signal .* snr(index)));
    capacity = nan(90,345);
    for y = 1:65
        for z = 1:240
            if ~isnan(cpacity(1,y,z))
                capacity(y,z) = sum(cpacity(:,y,z));
            end
        end
    end
    
    harrison_max = max(max(capacity(36:65,65:98)));
    harrison_cap(index) = harrison_max;
    %     [X Y] = find(capacity(36:65,65:98) == harrison_max);
    %     max_X = X + 36 - 1;
    %     max_Y = Y + 65 - 1;
    
    %% Secrecy Capacity
    % harrison = tx2_linear_signal(:,36:65,65:98);
    harrison = cpacity(:,36:65,65:98);
    smalley = cpacity(:,34:65,123:147);
    camacho = cpacity(:,34:65,154:179);
    
    eve_cp = cpacity;
    eve_cp(:,36:65,65:98) = 0;
    
    har_temp = capacity(36:65,65:98);
    sma_temp = capacity(34:65,123:147);
    cam_temp = capacity(34:65,154:179);
    
    eve_temp = capacity;
    eve_temp(36:65,65:98) = 0;
    
    [temp,I] = max(har_temp);
    [temp,J] = max(temp);
%     har_cap(index) = harrison_max;
    har_best_x(index) = J;
    har_best_y(index) = I(J);
%     
    [temp,I] = max(eve_temp);
    [temp,J] = max(temp);
    eve_cap(index) = temp;
    eve_best_x(index) = J;
    eve_best_y(index) = I(J);

%     [temp,I] = max(sma_temp);
%     [temp,J] = max(temp);
%     smalley_cap(index) = temp;
%     smal_best_x(index) = J;
%     smal_best_y(index) = I(J);
%     
%     [temp,I] = max(cam_temp);
%     [temp,J] = max(temp);
% %     cam_cap(index) = camacho_max;
%     cam_best_x(index) = J;
%     cam_best_y(index) = I(J);
    
    for carrier = 1:45
        sec = harrison(carrier,har_best_y(index),har_best_x(index)) - ...
            eve_cp(carrier,eve_best_y(index),eve_best_x(index));
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

snr = 10*log10(snr);
%%
figure()
hold on;
yyaxis right
plot(snr,harrison_cap,'--');
% plot(snr,camacho_cap,'-');
plot(snr,eve_cap,'-');
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

gaussian_capacity_data = [snr;harrison_cap;smalley_cap;sec_cap];
