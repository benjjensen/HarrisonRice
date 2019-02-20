% load Cs_threshold.mat;
I = imread('ClydeGIMPnoRXshowRooms.png');
figure();
imshow(I);
hold on
text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
hm = imagesc(Cs_threshold);
set(hm,'AlphaData',~isnan(Cs_threshold));
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