clear;
close all;


load linear_signal.mat; % loads in the signal data
load linear_noisefloor.mat; % loads in the noise data

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise

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
    
    %% Secrecy Capacity
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
    har_best_x(index) = J;
    har_best_y(index) = I(J);
     
    [temp,I] = max(eve_temp);
    [temp,J] = max(temp);
    eve_cap(index) = temp;
    eve_best_x(index) = J;
    eve_best_y(index) = I(J);
    
    for carrier = 1:45
        sec = harrison(carrier,har_best_y(index),har_best_x(index)) - ...
            eve_cp(carrier,eve_best_y(index),eve_best_x(index));
        if sec < 0
            sec = 0;
        end
        sec_cap(index) = sec_cap(index) + sec;
        
    end
    
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
