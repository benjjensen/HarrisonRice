% Plot LOS vs PowerReceived

clear; close all

timeToAverage = 100; % Average over 100 ms 

powerArray = load('Data/AlpineLoop_pt2B_powerArray.mat');
powerArray = powerArray.powerArray;
dataCount = length(powerArray);
saveIndex = 1;

% Averages 100 points of data (100 ms) at a time

for index = 1:(timeToAverage):(dataCount - timeToAverage)
    startPoint = index;
    endPoint = startPoint + timeToAverage - 1;
    averagedData(saveIndex) = mean(powerArray(1, startPoint:endPoint));
    saveIndex = saveIndex + 1;
end

% NANs = nan(1, 10);
% averagedData = [averagedData, NANs];

%%
X1 = [0 41 41 44 44 85 85 93 93 110 110 118 118];

Y1 = [0 0 1 1 0 0 1 1 0 0 1 1 0];

%%
figure()
yyaxis left;

    % Plots the Power Received
plotPower = averagedData;  % Ignores the first part cause it is weird
plotPower = plotPower - max(plotPower); % Normalizes the power 
t = (0:length(plotPower)-1)*0.1; % seconds
plot(t,plotPower);
ax1 = gca;

    % Makes it pretty for the paper
ax1.FontName = 'Times New Roman';
ax1.XTick = 0:30:length(plotPower)-1;
ax1.Children.Color = 0*[1 1 1]; %0.67*[1 1 1];
ax1.YLabel.String = 'Received Power (dBmax)';
ax1.YColor = 'k';

    % Plots the LOS 
yyaxis right;
plotX = (X1*10-550)*0.1;
plotY = 0.1*Y1 - 0.2;
YES = 0.7;
plot(plotX, YES*Y1);
ax2 = gca;

    % More pretty-ing
ax2.FontName = 'Times New Roman';
ax2.YLabel.String = 'Line of Sight';
ax2.YColor = 'k';
ax2.Children.Color = 0.5*[1 0 0];
ax2.XLim = [0 80];
ax2.Children.LineWidth = 0.5;
ax2.Children.LineStyle = '-';

ax2.YTickLabel = {'N' 'Y'};
% ax2.YTick = [-0.2 -0.1];
% ax2.YLim = [-0.25 1];
ax2.YTick = [0 YES];
ax2.YLim = [-0.1 1.1*YES];
ax2.XLabel.String = 'time (s)';

    % Paper formatting
% f = gcf;
% homer = f.Units;
% f.Units = 'inches';
% bart = f.Position;
% f.Position = [bart(1:2) 10 4];
% f.PaperPositionMode = 'auto';
% f.Units = homer;
% % saveas(gcf,'Figures/PowerReceived','epsc');
