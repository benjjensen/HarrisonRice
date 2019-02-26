function [axis] = FloorplanHeatmap(dataArray)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
I = imread('Data/FloorplanBackground.png');
figure();
imshow(I);
hold on
text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
text(110, 178, 'tx', 'Color', 'red', 'FontSize', 7);
hm = imagesc(dataArray);
set(hm,'AlphaData',~isnan(dataArray));
q = colorbar;
q.Position = [0.9067 0.2460 0.016 .5579];
colormap(jet);
ylabel(q, {'Number of sub-carriers such that SNR \geq \tau'}, 'FontName', 'Times New Roman');
hm.XData = [16; 333];
hm.YData = [68; 151];
hold off
axis = gca;
axis.FontName = 'Times New Roman';
ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;
% hold on
% text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
% hm = imagesc(dataArray);
% set(hm,'AlphaData',~isnan(dataArray));
% colormap(jet);
% q = colorbar;
% q.Position = [.855 .4 .016 .3];
% ylabel(q, 'bits per channel use');
% hm.XData = [36; 380];
% hm.YData = [49; 139];
% hold off
% cf = gcf;
% cf.PaperSize = [5 4];
% cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];
% axis = gca;
end

