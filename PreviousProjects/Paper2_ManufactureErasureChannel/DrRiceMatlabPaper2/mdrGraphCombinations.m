%%% mdrGraphCombinations

load DataMatrix_Workspace.mat dataMatrix

figure(61); clf;
subplot(311); plot(dataMatrix(1,:),dataMatrix(2:4,:)); grid on; ax1 = gca;
lx1 = legend('Bob','Eve','Secrecy');
subplot(312); plot(dataMatrix(1,:),dataMatrix(5:7,:)); grid on; ax2 = gca;
lx2 = legend('Bob','Eve','Secrecy');
subplot(313); plot(dataMatrix(1,:),dataMatrix(8:10,:)); grid on; ax3 = gca;
lx3 = legend('Bob','Eve','Secrecy');

figure(62); clf;
subplot(311); plot(dataMatrix(1,:),log10(dataMatrix(2:4,:))); grid on; ax4 = gca;
lx4 = legend('Bob','Eve','Secrecy');
subplot(312); plot(dataMatrix(1,:),log10(dataMatrix(5:7,:))); grid on; ax5 = gca;
lx5 = legend('Bob','Eve','Secrecy');
subplot(313); plot(dataMatrix(1,:),log10(dataMatrix(8:10,:))); grid on; ax6 = gca;
lx6 = legend('Bob','Eve','Secrecy');

figure(63); clf;
subplot(211); 
plot(dataMatrix(1,:),dataMatrix([2:3 5:6 8:9],:));
grid on;
ax7 = gca;
lx7 = legend('Bob (Gaussian Model)','Eve (Gaussian Model)',...
    'Bob (Threshold model)','Eve (Threshold Model)',...
    'Bob (Cross Model)','Eve (Cross Model)');
subplot(212);
plot(dataMatrix(1,:),dataMatrix([4 7 10],:));
grid on;
ax8 = gca;
lx8 = legend('Gaussian Model','Threshold Model','Cross Model');

ax1.FontName = 'Times New Roman';
ax1.XLim = [-10 50];
ax1.YLim = [0 100];
ax1.XLabel.String = 'SNR (dB)';
ax1.YLabel.String = 'bits/use';
ax1.Children(1).LineWidth = 1;
ax1.Children(1).Color = 'k';
ax1.Children(2).LineWidth = 1;
ax1.Children(2).Color = 'k';
ax1.Children(2).LineStyle = '--';
ax1.Children(3).LineWidth = 1;
ax1.Children(3).Color = 'k';
ax1.Children(3).LineStyle = '-.';

lx1.Location = 'NorthWest';

ax2.FontName = 'Times New Roman';
ax2.XLim = [-10 50];
ax2.XLabel.String = 'SNR (dB)';
ax2.YLabel.String = 'bits/use';
ax2.Children(1).LineWidth = 1;
ax2.Children(1).Color = 'k';
ax2.Children(2).LineWidth = 1;
ax2.Children(2).Color = 'k';
ax2.Children(2).LineStyle = '--';
ax2.Children(3).LineWidth = 1;
ax2.Children(3).Color = 'k';
ax2.Children(3).LineStyle = '-.';

lx2.Location = 'NorthWest';

ax3.FontName = 'Times New Roman';
ax3.XLim = [-10 50];
ax3.XLabel.String = 'SNR (dB)';
ax3.YLabel.String = 'bits/use';
ax3.Children(1).LineWidth = 1;
ax3.Children(1).Color = 'k';
ax3.Children(2).LineWidth = 1;
ax3.Children(2).Color = 'k';
ax3.Children(2).LineStyle = '--';
ax3.Children(3).LineWidth = 1;
ax3.Children(3).Color = 'k';
ax3.Children(3).LineStyle = '-.';

lx3.Location = 'NorthWest';

ax4.FontName = 'Times New Roman';
ax4.XLim = [-10 50];
ax4.XLabel.String = 'SNR (dB)';
ax4.YLabel.Interpreter = 'LaTeX';
ax4.YLabel.String = '$\log_{10}({\rm bits/use})$';
ax4.Children(1).LineWidth = 1;
ax4.Children(1).Color = 'k';
ax4.Children(2).LineWidth = 1;
ax4.Children(2).Color = 'k';
ax4.Children(2).LineStyle = '--';
ax4.Children(3).LineWidth = 1;
ax4.Children(3).Color = 'k';
ax4.Children(3).LineStyle = '-.';

lx4.Location = 'SouthEast';

ax5.FontName = 'Times New Roman';
ax5.XLim = [-10 50];
ax5.XLabel.String = 'SNR (dB)';
ax5.YLabel.Interpreter = 'LaTeX';
ax5.YLabel.String = '$\log_{10}({\rm bits/use})$';
ax5.Children(1).LineWidth = 1;
ax5.Children(1).Color = 'k';
ax5.Children(2).LineWidth = 1;
ax5.Children(2).Color = 'k';
ax5.Children(2).LineStyle = '--';
ax5.Children(3).LineWidth = 1;
ax5.Children(3).Color = 'k';
ax5.Children(3).LineStyle = '-.';

lx5.Location = 'SouthEast';

ax6.FontName = 'Times New Roman';
ax6.XLim = [-10 50];
ax6.XLabel.String = 'SNR (dB)';
ax6.YLabel.Interpreter = 'LaTeX';
ax6.YLabel.String = '$\log_{10}({\rm bits/use})$';
ax6.Children(1).LineWidth = 1;
ax6.Children(1).Color = 'k';
ax6.Children(2).LineWidth = 1;
ax6.Children(2).Color = 'k';
ax6.Children(2).LineStyle = '--';
ax6.Children(3).LineWidth = 1;
ax6.Children(3).Color = 'k';
ax6.Children(3).LineStyle = '-.';

lx6.Location = 'SouthEast';

ax7.FontName = 'Times New Roman';
ax7.XLim = [-10 50];
ax7.XLabel.String = 'SNR (dB)';
ax7.YLabel.String = 'C (bits/use)';

ax7.Children(1).LineWidth = 2;
ax7.Children(1).Color = 'k';

ax7.Children(2).LineWidth = 2;
ax7.Children(2).Color = 'k';
ax7.Children(2).LineStyle = '--';

ax7.Children(3).LineWidth = 1;
ax7.Children(3).Color = 'k';

ax7.Children(4).LineWidth = 1;
ax7.Children(4).Color = 'k';
ax7.Children(4).LineStyle = '--';

ax7.Children(5).LineWidth = 0.5;
ax7.Children(5).Color = 'k';

ax7.Children(6).LineWidth = 0.5;
ax7.Children(6).Color = 'k';
ax7.Children(6).LineStyle = '--';

lx7.Location = 'NorthWest';

ax8.FontName = 'Times New Roman';
ax8.XLim = [-10 50];
ax8.XLabel.String = 'SNR (dB)';
ax8.YLabel.String = 'Secrecy (bits/use)';

ax8.Children(1).LineWidth = 2;
ax8.Children(1).Color = 'k';

ax8.Children(2).LineWidth = 1;
ax8.Children(2).Color = 'k';

ax8.Children(3).LineWidth = 0.5;
ax8.Children(3).Color = 'k';

lx8.Location = 'North';

figure(61);
ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 6];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(62);
ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 6];
ff.PaperPositionMode = 'auto';
ff.Units = homer;

figure(63);
ff = gcf;
homer = ff.Units;
ff.Units = 'inches';
bart = ff.Position;
ff.Position = [bart(1:2) 5 5];
ff.PaperPositionMode = 'auto';
ff.Units = homer;