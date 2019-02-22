clear;
close all;

load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');
num_loops = 200;
har_max = zeros(1,num_loops);
cam_max = zeros(1,num_loops);
sma_max = zeros(1,num_loops);
sec_cap = zeros(1,num_loops);
conf = zeros(1,64);
cham = zeros(1,64);
% [num_rows,num_cols,~] = size(;
% q = -19.95:.05:30;
q = linspace(1,30,num_loops);
for index = 1:num_loops
    tx2threshold = q(index);
    % Finds the max number of carriers in each room with the given threshold
    [cam2,cam_floor] = find_num_carriers(tx2camacho,tx2threshold);
    [harrison2, harrison_floor] = find_num_carriers(tx2harrison,tx2threshold);
    [smalley2, smalley_floor] = find_num_carriers(tx2smalley,tx2threshold);
    
    har_max(index) = max(harrison2);
    cam_max(index) = max(cam2);
    sma_max(index) = max(smalley2);
    
    sec = har_max(index) - max(cam_max(index),sma_max(index));
    if sec < 0
        sec_cap(index) = 0;
    else
        sec_cap(index) = sec;
    end
end
q = 10.^(q/10);
figure()
hold on;
plot(q,har_max);
plot(q,cam_max);
plot(q,sma_max);
ylabel('Bits per channel use')
xlabel('Power attenuation');
set(gca,'XScale','log');
hold off

figure();
plot(q,sec_cap);
ylabel('Secure Bits per channel use')
xlabel('Power attenuation');
set(gca,'XScale','log');
