%%% makeDelaySpreadPlot (MDR)

load Data/avgDelaySpreadResults.mat

%   Name      Size            Bytes  Class     Attributes
%
%   x         1x64              512  double    

BW = 10 - 0.15625;
dtau = 1/BW;
tau = (0:length(x)-1)*dtau;

A = x/x(1);
AdB = 10*log10(A);

figure(1);
plot(tau,AdB); grid on;
ax = gca;

ax.FontName = 'Times New Roman';
ax.XLabel.String = 'excess delay (\mus)';
ax.YLabel.String = 'magnitude (dB)';
ax.Children.Color = 'k';
ax.Children.LineWidth = 1;

f = gcf;
homer = f.Units;
f.Units = 'inches';
bart = f.Position;
f.Position = [bart(1:2) 5 4];
f.PaperPositionMode = 'auto';
f.Units = homer;

% delay spread calculations
% see https://en.wikipedia.org/wiki/Delay_spread

mean_delay = (tau * A')/sum(A);
delay_spread = (((tau-mean_delay).^2)*A')/sum(A);
rms_delay_spread = sqrt(delay_spread);

% zoomed-in plot including mean delay and rms delay spread

% linear interpolation to find Amean
lisa = find(tau <= mean_delay);
m = lisa(end);                   % basepoint_index;
mu = (mean_delay - tau(m))/dtau; % fractional interval;
Amean = mu * AdB(m+1) + (1-mu)*AdB(m);

% linear interpolation to find Arms
lisa = find(tau <= mean_delay + rms_delay_spread);
m = lisa(end);                                      % basepoint index
mu = (mean_delay + rms_delay_spread - tau(m))/dtau; % fractional interval
Arms = mu * AdB(m+1) + (1-mu) * AdB(m);

figure(2); clf;
plot(tau,AdB); grid on;
ax2 = gca;

ax2.FontName = 'Times New Roman';
ax2.XLabel.String = 'excess delay (\mus)';
ax2.YLabel.String = 'magnitude (dB)';
ax2.Children.Color = 'k';
ax2.Children.LineWidth = 1;
ax2.XLim = [0 1];

f = gcf;
homer = f.Units;
f.Units = 'inches';
bart = f.Position;
f.Position = [bart(1:2) 5 4];
f.PaperPositionMode = 'auto';
f.Units = homer;

line(mean_delay*[1 1],[ax2.YLim(1) Amean],'Color','k','LineStyle','--');
line(rms_delay_spread*[1 1],[ax2.YLim(1) Arms],'Color','k','LineStyle','--');

pos = ax2.Position;
arStart = [mean_delay, -30];     % (x,y) of start (no arrow head)
arEnd = [rms_delay_spread, -30]; % (x,y) of end (arrow head)

arPosX = [ (arStart(1) + abs(min(xlim)))/diff(xlim) * pos(3) + pos(1), ...
    (arEnd(1) + abs(min(xlim)))/diff(xlim) * pos(3) + pos(1) ];

arPosY = [ (arStart(2) - min(ylim))/diff(ylim) * pos(4) + pos(2), ...
    (arEnd(2) - min(ylim))/diff(ylim) * pos(4) + pos(2)];
    
ar = annotation('arrow',arPosX,arPosY);
ar.Color = 'k';
ar.LineStyle = '-';

atextX = 0.5*(mean_delay + rms_delay_spread);
atextY = arStart(2);
atext = sprintf('$\\tau_{\\rm RMS} = $ %1.2f $\\mu$s',rms_delay_spread);
text(atextX,atextY,atext,'Interpreter','LaTeX',...
    'HorizontalAlignment','Center','VerticalAlignment','Bottom');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% really advanced plot
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure(3); clf;
plot(tau,AdB); grid on;
ax3 = gca;

ax3.FontName = 'Times New Roman';
ax3.XLabel.String = 'excess delay (\mus)';
ax3.YLabel.String = 'magnitude (dB)';
ax3.Children.Color = 'k';
ax3.Children.LineWidth = 1;

f = gcf;
homer = f.Units;
f.Units = 'inches';
bart = f.Position;
f.Position = [bart(1:2) 5 4];
f.PaperPositionMode = 'auto';
f.Units = homer;

ax4 = axes('Position',[0.4 0.5 0.45 0.4]);
plot(tau,AdB); grid on;

ax4.FontName = 'Times New Roman';
ax4.Children.Color = 'k';
ax4.Children.LineWidth = 1;
ax4.XLim = [0 1];

line(mean_delay*[1 1],[ax4.YLim(1) Amean],'Color','k','LineStyle','--');
line(rms_delay_spread*[1 1],[ax4.YLim(1) Arms],'Color','k','LineStyle','--');

ar4Start = [mean_delay, -30];
ar4End = [rms_delay_spread, -30];
pos4 = ax4.Position;

arPos4X = [ (ar4Start(1) + abs(min(xlim)))/diff(xlim) * pos4(3) + pos4(1), ...
    (ar4End(1) + abs(min(xlim)))/diff(xlim) * pos4(3) + pos4(1) ];

arPos4Y = [ (ar4Start(2) - min(ylim))/diff(ylim) * pos4(4) + pos4(2), ...
    (ar4End(2) - min(ylim))/diff(ylim) * pos4(4) + pos4(2)];
    
ar4 = annotation('arrow',arPos4X,arPos4Y);
ar4.Color = 'k';
ar4.LineStyle = '-';

atextX4 = 0.5*(mean_delay + rms_delay_spread);
atextY4 = ar4Start(2)+1;
atext = sprintf('$\\tau_{\\rm RMS} = $ %1.2f $\\mu$s',rms_delay_spread);
text(atextX4,atextY4,atext,'Interpreter','LaTeX',...
    'HorizontalAlignment','Center','VerticalAlignment','Bottom');

saveas(gcf,'Figures/DelaySpread','epsc');


%Scale to 0.75 (include graphics scale = 0.75) -> in LaTeX