close all;

load('camacho.mat');
load('chambers.mat');
load('conference.mat');
load('hall_pwelch.mat');
load('harrison.mat');
load('smalley.mat');
load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');

array_mesh_all(); % combines all of the data into one array with the 
array_mesh_all2(); % location in the matrix corresponding to the location on the floor
Nfft = 2*64; % Number of ffts 
FF = -0.5:1/Nfft:0.5-1/Nfft; % stuff for pwelch
FF = 20*FF; % stuff for pwelch
threshold = 19; % sets the threshold for the first tx location
tx2threshold = 25; % sets the threshold for the second tx location

[~,Floor] = find_num_carriers(xx,threshold); % xx comes from running array_mesh_all
 % find the number of carriers that are good at each location
[~,tx2Floor] = find_num_carriers(tx2xx,tx2threshold); % same as above but for the second location

%uncomment to get the heatmap for the first tx location
% figure(1);
I = imread('floor_plan.png');
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

figure(2)
imshow(I); % Displays the picture of the floor plan
hold on;
tx2floormap = imagesc(tx2Floor); % heatmap of the good carriers 
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
set(tx2floormap,'AlphaData',~isnan(tx2Floor));
saveas(gcf,'Heatmap','pdf');
hold off;