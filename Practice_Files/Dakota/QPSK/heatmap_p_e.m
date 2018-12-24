load('linearSignalReducedCarriers.mat');
num_carriers = 45;
num_rows = 90;
num_cols = 345;
sigmaSquared = .03;
epsilon = .01;
map_p_e = zeros(num_carriers,num_rows,num_cols);
for carrier = 1:num_carriers
   for row = 1:num_rows
      for col = 1:num_cols
            if(isnan(linearSignalReducedCarriers(carrier,row,col)))
                map_p_e(carrier,row,col) = nan;
            else
                map_p_e(carrier,row,col) = probability_erasure(sigmaSquared,...
                    linearSignalReducedCarriers(carrier,row,col),epsilon);
            end
      end
   end
end

mean_map_p_e = zeros(num_rows,num_cols);
for row = 1:num_rows
  for col = 1:num_cols
        mean_map_p_e(row,col) = mean(map_p_e(:,row,col));
  end
end

%% Generate Capacity and secrecy capacity

capacity = num_carriers - (num_carriers * mean_map_p_e);
harrison_cap = capacity(36:65,65:98);
[Y,I] = max(harrison_cap);
[Y,J] = max(Y);

secrecy_capacity = capacity;

for row = 1:num_rows
    for col = 1:num_cols
        if isnan(capacity(row,col))
        elseif capacity(row,col) > Y
            secrecy_capacity(row,col) = 0;
        else
            secrecy_capacity(row,col) = Y - secrecy_capacity(row,col);
        end
    end
end
%% HEATMAP
I = imread('floor_plan.png');
figure()
imshow(I); % Displays the picture of the floor plan
hold on;
tx2floormap = imagesc(secrecy_capacity); % heatmap of the good carriers 
tx2floormap.XData = [13; 315]; % get it so the heatmap overlays correctly onto the floor plan
tx2floormap.YData = [61; 137]; % get it so the heatmap overlays correctly
myColor = colormap(flipud(colormap('hot'))); % the colormap for the heatmap
c = colorbar; 
c.Position = [.89 .155 .016 .775];
ylabel(c, {'\fontsize{9}Number of sub-carriers such that SNR\geq \tau'});
text(100, 160, '*', 'Color', 'red', 'FontSize', 16);
text(105, 165, 'tx', 'Color', 'red', 'FontSize', 7);
%.Label.String = '# of sub-carriers such that SNR'; % \geq \tau';
colormap('jet');
tx2floormap.AlphaData = .6; % makes the heatmap slightly transparent so the floor plan can be seen
set(tx2floormap,'AlphaData',~isnan(secrecy_capacity));
% saveas(gcf,'Heatmap','pdf');
hold off;

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