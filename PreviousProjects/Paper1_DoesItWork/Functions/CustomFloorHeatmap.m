function [axis] = CustomFloorHeatmap(dataArray, indicateRx)
% Created by Kalin Norman
% This heatmap uses a custom background that is "zoomed in" when compared
% to the original blueprint that was used in the FloorplanHeatmap function.
% The first parameter is the array that needs to be plotted (generally this
% parameter should be capacity or secrecy capacity, generated through the
% use of the GaussianCapacity function), while the second parameter
% indicates whether or not the figure should indicated the location of the
% intended receiver. That part of this function is not automatic, and
% should the data change, it will need to be updated. 

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
q.Position = [.855 .4 .016 .3]; % Resizes the colorbar to be thinner
ylabel(q, '  bits per channel use', 'FontName', 'Times New Roman');
hm.XData = [36; 380];
hm.YData = [49; 139];
hold off
% The following code adjusts the figure to fit better in a two column IEEE
% formatted document. After generating the figure it is advisable to save
% it as a .pdf format or .eps
axis = gca;
axis.FontName = 'Times New Roman'; 
ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;
end

