%%% mdrTempPlotFig5

load mdrTempDataFig5

IMin = min(I_imag, I_real);

figure(51);
contourf(real_x,imag_x,IMin);
axis square;
colormap gray;
cmap = colormap;
[cmapr, ~] = size(cmap);
cmap2 = cmap(cmapr:-1:20,:);
cmap3 = cmap(20:cmapr,:);
colormap(cmap2);
c1 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax1 = gca;

figure(52);
contourf(real_x,imag_x,IMin,20);
axis square
colormap(cmap2);
c2 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax2 = gca;


c1.Limits = [0 2];
ax1.FontName = 'Times New Roman';
ax1.XLabel.String = 'In-Phase';
ax1.YLabel.String = 'Quadrature';
ax1.Children(1).Color = 'w';
ax1.Children(1).MarkerFaceColor = 'w';

c2.Limits = [0 2];
ax2.FontName = 'Times New Roman';
ax2.XLabel.String = 'In-Phase';
ax2.YLabel.String = 'Quadrature';
ax2.Children(1).Color = 'w';
ax2.Children(1).MarkerFaceColor = 'w';


figure(51); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(52); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;