%%% mdrTempPlot

load mdrTempDataFig4.mat

figure(41); clf;
contourf(real_x,imag_x,I);
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

figure(42); clf;
contourf(real_x,imag_x,I);
axis square;
colormap(cmap3);
c2 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax2 = gca;

figure(43); clf;
contourf(real_x,imag_x,I,20);
axis square;
colormap(cmap2);
c3 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax3 = gca;

figure(44); clf;
contourf(real_x,imag_x,I,20);
axis square;
colormap(cmap3);
c4 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax4 = gca;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure(45); clf;
contourf(real_x,imag_x,C);
axis square;
colormap(cmap2);
c5 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax5 = gca;

figure(46); clf;
contourf(real_x,imag_x,C);
axis square;
colormap(cmap3);
c6 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax6 = gca;

figure(47); clf;
contourf(real_x,imag_x,C,20);
axis square;
colormap(cmap2);
c7 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax7 = gca;

figure(48); clf;
contourf(real_x,imag_x,C,20);
axis square;
colormap(cmap3);
c8 = colorbar;
hold on; plot([1 1 -1 -1],[1 -1 1 -1],'o'); hold off;
ax8 = gca;

%%
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
ax2.Children(1).Color = 'k';
ax2.Children(1).MarkerFaceColor = 'k';

c3.Limits = [0 2];
ax3.FontName = 'Times New Roman';
ax3.XLabel.String = 'In-Phase';
ax3.YLabel.String = 'Quadrature';
ax3.Children(1).Color = 'w';
ax3.Children(1).MarkerFaceColor = 'w';

c4.Limits = [0 2];
ax4.FontName = 'Times New Roman';
ax4.XLabel.String = 'In-Phase';
ax4.YLabel.String = 'Quadrature';
ax4.Children(1).Color = 'k';
ax4.Children(1).MarkerFaceColor = 'k';

c5.Limits = [0 4];
ax5.FontName = 'Times New Roman';
ax5.XLabel.String = 'In-Phase';
ax5.YLabel.String = 'Quadrature';
ax5.Children(1).Color = 'w';
ax5.Children(1).MarkerFaceColor = 'w';

c6.Limits = [0 4];
ax6.FontName = 'Times New Roman';
ax6.XLabel.String = 'In-Phase';
ax6.YLabel.String = 'Quadrature';
ax6.Children(1).Color = 'k';
ax6.Children(1).MarkerFaceColor = 'k';

c7.Limits = [0 4];
ax7.FontName = 'Times New Roman';
ax7.XLabel.String = 'In-Phase';
ax7.YLabel.String = 'Quadrature';
ax7.Children(1).Color = 'w';
ax7.Children(1).MarkerFaceColor = 'w';

c8.Limits = [0 4];
ax8.FontName = 'Times New Roman';
ax8.XLabel.String = 'In-Phase';
ax8.YLabel.String = 'Quadrature';
ax8.Children(1).Color = 'k';
ax8.Children(1).MarkerFaceColor = 'k';

figure(41); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(42); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(43); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(44); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(45); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(46); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(47); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(48); ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 4];
ff.PaperPositionMode = 'auto';
ff.Units = homer;