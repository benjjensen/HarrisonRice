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

array_mesh_all();
array_mesh_all2();
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
threshold = 19;
tx2threshold = 27;

[~,Floor] = find_num_carriers(xx,threshold);
[~,tx2Floor] = find_num_carriers(tx2xx,tx2threshold);

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
imshow(I);
hold on;
tx2floormap = imagesc(tx2Floor);
tx2floormap.XData = [13; 315];
tx2floormap.YData = [61; 137];
myColor = colormap(flipud(colormap('hot')));
c = colorbar;
c.Position = [.89 .155 .016 .775];
ylabel(c, {'\fontsize{9}Number of sub-carriers such that SNR\geq \tau'});
text(100, 160, '*', 'Color', 'red', 'FontSize', 16);
text(105, 165, 'tx', 'Color', 'red', 'FontSize', 7);
%.Label.String = '# of sub-carriers such that SNR'; % \geq \tau';
colormap('jet');
tx2floormap.AlphaData = .6;
set(tx2floormap,'AlphaData',~isnan(tx2Floor));
saveas(gcf,'Heatmap','pdf');
hold off;