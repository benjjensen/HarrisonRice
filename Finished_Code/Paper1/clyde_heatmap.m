function [] = clyde_heatmap(data)
%clyce_heatmap draws the heatmap based on the data array given
%   Draws the 4th floor of the clyde building, then draws the heatmap on
%   top, Nan values are treated as a clear color
I = imread('ClydeGIMPnoRXshowRooms.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(data);
set(hm,'AlphaData',~isnan(data));
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
end

