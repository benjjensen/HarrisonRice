clear;
close all;

load('tx2_linear_signal.mat');
load('linear_signal.mat');
% load('linear_noisefloor.mat');
for y = 1:90
    for z = 1:345
        tx2_linear_signal(:,y,z) = fftshift(tx2_linear_signal(:,y,z)); % correct way to fftshift
    end
end
harrison = sqrt(tx2_linear_signal(:,36:65,65:98)); % find g for harrisons room
r_max = max(max(max(harrison))); % find his max
tx2_linear_signal = sqrt(tx2_linear_signal)/r_max; % normalize all of the gs
num_carriers = 64;
% num_rows = 65; % for ignoring hallway
% num_cols = 240; % for ignoring hallway
num_rows = 90; % to include hallway
num_cols = 345; % to include hallway
snr = .636; % chosen snr value
epsilon = 0.1; % chosen epsilon value

map_p_e = zeros(num_carriers,num_rows,num_cols);
good_map_p_e = [];
tic;
for row = 1:num_rows % for each row on the floor
    row
    for col = 1:num_cols % for each column
        for carrier = 11:55 % for each of the good carriers
            if(isnan(tx2_linear_signal(carrier,row,col)))
                map_p_e(:,row,col) = nan; % if the spot is nan it stays nan
                break;
            else
                map_p_e(carrier,row,col) = probability_erasure(snr,...
                    tx2_linear_signal(carrier,row,col),epsilon); % calculate the probability
                % of erasure for each carrier in each location
            end
        end
    end
end
toc;
map_capacity_e = 2 * (1 - map_p_e); % a map of the capacity at each location for each carrier

%% Generate Capacity and secrecy capacity

capacity = sum(map_capacity_e); % harrison's capacity
harrison_cap = capacity(36:65,65:98);
harrison_p_e = map_p_e(:,36:65,65:98); % harrison's probability of erasures
[harrison_max,I] = max(harrison_cap); % step to find harrison's best location
[harrison_max,J] = max(harrison_max); % step to find harrison's best location
index = [J I(J)]; % harrison's best location [x,y] or [col,row]

sec_cap = zeros(num_rows,num_cols); % map of secrecy capacity

for row = 1:num_rows % for each row again
    for col = 1:num_cols % for each column
        if isnan(capacity(row,col)) % if it's nan then set sec_cap to nan at that spot
            sec_cap(row,col) = nan;
        else
            for carrier = 11:55 % calculate the secrecy capacity for each carrier
                sec_cap(row,col) = sec_cap(row,col) + secrecy_capacity(...
                    harrison_p_e(carrier,index(2),index(1)),map_p_e(capacity,row,col));
            end
        end
    end
end

sec_cap = 2 * sec_cap; % multiply by two because two bits per symbol

% bad_carriers = [];
% i = 1;
% harrison_carrier_p_e = map_p_e(:,36:65,65:98);
% for carrier = 1:num_carriers
%     if harrison_carrier_p_e(carrier,index(1),index(2)) > threshold
%         bad_carriers(i) = carrier;
%         i = i + 1;
%     end
% end
% harrison_carrier_p_e(bad_carriers,:,:) = [];
% map_p_e(bad_carriers,:,:) = [];
%
% [num_carriers,~,~] = size(map_p_e);
% map_cap = zeros(num_rows,num_cols);
% for row = 1:num_rows
%     for col = 1:num_cols
%         for carrier = 1:num_carriers
%             if isnan(map_p_e(carrier,row,col))
%                 map_cap(row,col) = nan;
%                 break;
%             else
%                 map_cap(row,col) = map_cap(row,col) + (1 - map_p_e(carrier,row,col));
%             end
%         end
%     end
% end
%
% harrison_good_cap = map_cap(36:65,65:98);
% [harrison_good_max,I] = max(harrison_good_cap);
% [harrison_good_max,J] = max(harrison_good_max);
%
% good_cap = map_cap;
% for row = 1:num_rows
%     for col = 1:num_cols
%         if isnan(map_cap(row,col))
%             good_cap(row,col) = nan;
%         else
%             good_cap(row,col) = harrison_good_max - map_cap(row,col);
%         end
%     end
% end
%% HEATMAP
I = imread('ClydeGIMPnoRXshowRooms.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(sec_cap);
set(hm,'AlphaData',~isnan(sec_cap));
colormap(jet);
q = colorbar;
q.Position = [.855 .4 .016 .3];
ylabel(q, 'bits per channel use');
hm.XData = [36; 380];
hm.YData = [49; 139];
hold off
cf = gcf;
cf.PaperSize = [5 4];
cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];

conference_sec_cap = sec_cap(1:33,1:98);
[conf,K] = min(conference_sec_cap);
[conf,K] = min(conf)
smalley_sec_cap = sec_cap(34:65,123:147);
[sma,K] = min(smalley_sec_cap);
[sma,K] = min(sma)
camacho_sec_cap = sec_cap(34:65,154:179);
[cam,K] = min(camacho_sec_cap);
[cam,K] = min(cam)
chambers_sec_cap = sec_cap(34:65,213:240);
[cham,K] = min(chambers_sec_cap);
[cham,K] = min(cham)

