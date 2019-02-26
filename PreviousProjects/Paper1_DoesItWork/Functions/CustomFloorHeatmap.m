function [axis] = CustomFloorHeatmap(dataArray, indicateRx)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
I = imread('Data/HeatmapBackground.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(dataArray);
set(hm,'AlphaData',~isnan(dataArray));
if (indicateRx)
    text(108, 75, 'rx', 'Color', 'black', 'FontSize', 8);
end
colormap(jet);
q = colorbar;
q.Position = [.855 .4 .016 .3];
ylabel(q, '  bits per channel use');
hm.XData = [36; 380];
hm.YData = [49; 139];
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
% cf = gcf;
% cf.PaperSize = [5 4];
% cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];

end

