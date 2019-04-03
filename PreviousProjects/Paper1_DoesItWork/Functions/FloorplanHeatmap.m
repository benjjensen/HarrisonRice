function [axis] = FloorplanHeatmap(dataArray)
% Created by Kalin Norman

% Very similar to CustomFloorHeatmap (see that file for a more detailed
% explanation). The major difference being that this heatmap uses the
% blueprint of the Clyde building as the background and the images are
% scaled appropriately to fit atop that image. 

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

% Sets the text of the figure to Times New Roman and sizes the figure
% appropriately to fit into a double column paper. 
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

