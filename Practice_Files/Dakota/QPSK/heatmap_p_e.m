clear;
close all;

load('tx2_linear_signal.mat');
load('linear_signal.mat');
load('linear_noisefloor.mat');
num_carriers = 64;
% num_rows = 65; % for ignoring hallway
% num_cols = 240; % for ignoring hallway
num_rows = 90; % to include hallway
num_cols = 345; % to include hallway
sigmaSquared = 289.4;
epsilon = .01;
threshold = .1;
map_p_e = zeros(num_carriers,num_rows,num_cols);
good_map_p_e = [];
tic;
for row = 1:num_rows
    row
   for col = 1:num_cols
      for carrier = 1:num_carriers
            if(isnan(linear_signal(carrier,row,col)))
                map_p_e(:,row,col) = nan;
                break;
            else
                map_p_e(carrier,row,col) = probability_erasure(...
                    linear_noisefloor(carrier,row,col)*sigmaSquared,...
                    linear_signal(carrier,row,col),epsilon);
            end
      end
   end
end
toc;
mean_map_p_e = zeros(num_rows,num_cols);
for row = 1:num_rows
  for col = 1:num_cols
        mean_map_p_e(row,col) = mean(map_p_e(:,row,col));
  end
end

%% Generate Capacity and secrecy capacity

capacity = num_carriers - (num_carriers * mean_map_p_e);
harrison_cap = capacity(36:65,65:98);
[harrison_max,I] = max(harrison_cap);
[harrison_max,J] = max(harrison_max);
index = [I(J) J];

secrecy_capacity = capacity;

for row = 1:num_rows
    for col = 1:num_cols
        if isnan(capacity(row,col))
        elseif capacity(row,col) > harrison_max
            secrecy_capacity(row,col) = 0;
        else
            secrecy_capacity(row,col) = harrison_max - secrecy_capacity(row,col);
        end
    end
end

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
I = imread('ClydeGIMPnoRX.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(secrecy_capacity);
set(hm,'AlphaData',~isnan(secrecy_capacity));
q = colorbar;
q.Position = [.855 .4 .016 .3];
colormap(jet);
ylabel(q, 'bits per channel use');
hm.XData = [36; 380];
hm.YData = [49; 139];
hold off
cf = gcf;
cf.PaperSize = [5 4];
cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];

conference_sec_cap = secrecy_capacity(1:33,1:98);
[conf,K] = min(conference_sec_cap);
[conf,K] = min(conf)
smalley_sec_cap = secrecy_capacity(34:65,123:147);
[sma,K] = min(smalley_sec_cap);
[sma,K] = min(sma)
camacho_sec_cap = secrecy_capacity(34:65,154:179);
[cam,K] = min(camacho_sec_cap);
[cam,K] = min(cam)
chambers_sec_cap = secrecy_capacity(34:65,213:240);
[cham,K] = min(chambers_sec_cap);
[cham,K] = min(cham)

