clear;
close all;

load linear_signal.mat; % loads in the signal data
load linear_noisefloor.mat; % loads in the noise data

[signal,noise] = shift_normalize_signal_noise(linear_signal,linear_noisefloor); %signal = g^2 and noise is the fft shifted noise

tx2harrison = signal(:,36:65,65:98); % separate the data from harrison's office
tx2camacho = signal(:,34:65,154:179); % separate the data from camacho's office
tx2smalley = signal(:,34:65,123:147); % separate the data from smalley's office
tx2conference = signal(:,1:65,1:61); % separate the data from the conference room
tx2chambers = signal(:,34:65,213:240); % separate the data from chamber's office

db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear 
snr = 10^(2.3/10); % chosen snr value

% [~,Floor] = find_num_carriers(xx,threshold); % xx comes from running array_mesh_all
 % find the number of carriers that are good at each location
% [~,tx2Floor] = find_num_carriers(tx2xx,tx2threshold); % same as above but for the second location
[~,tx2Floor] = find_num_carriers(signal,snr,threshold); % same as above but for the second location

%uncomment to get the heatmap for the first tx location
% figure();
% I = imread('floor_plan.png');
% imshow(I);
% hold on;
% floormap = imagesc(Floor);
% floormap.XData = [13; 315];
% floormap.YData = [61; 137];
% colormap('jet')
% floormap.AlphaData = .6;
% set(floormap,'AlphaData',~isnan(Floor));
% colorbar;
% hold off;

clyde_heatmap(tx2Floor);
% figure()
% imshow(I); % Displays the picture of the floor plan
% hold on;
% tx2floormap = imagesc(tx2Floor); % heatmap of the good carriers 
% tx2floormap.XData = [13; 315]; % get it so the heatmap overlays correctly onto the floor plan
% tx2floormap.YData = [61; 137]; % get it so the heatmap overlays correctly
% myColor = colormap(flipud(colormap('hot'))); % the colormap for the heatmap
% c = colorbar; 
% c.Position = [.89 .155 .016 .775];
% ylabel(c, {'\fontsize{9}Number of sub-carriers such that SNR\geq \tau'});
% text(100, 160, '*', 'Color', 'red', 'FontSize', 16);
% text(105, 165, 'tx', 'Color', 'red', 'FontSize', 7);
% %.Label.String = '# of sub-carriers such that SNR'; % \geq \tau';
% colormap('jet');
% tx2floormap.AlphaData = .6; % makes the heatmap slightly transparent so the floor plan can be seen
% set(tx2floormap,'AlphaData',~isnan(tx2Floor));
% % saveas(gcf,'Heatmap','pdf');
% hold off;