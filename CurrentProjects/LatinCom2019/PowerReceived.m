% Power Received

clear; close all;

timeToAverage = 100; % Average 100 ms

load('Data/powerPerMilliSecondArray.mat');
dataCount = length(powerPerMilliSecondArray);
saveIndex = 1;

% Averages 100 points of data at a time (100 ms)
for index = 1:(timeToAverage):(dataCount - timeToAverage)
    startPoint = index;
    endPoint = startPoint + timeToAverage - 1;
    averagedData(saveIndex) = mean(powerPerMilliSecondArray(1, startPoint:endPoint));
    saveIndex = saveIndex + 1;
end
NANs = nan(1, (3000 - 2372));   % Fills in the end with NANs to align with LOS plot
averagedData = [averagedData, NANs];  

averagedData = 10*log10(averagedData); % Converts to dB

%% Data from VehicleLineOfSightToUS189

X = [527 552 552 566 566 568.3 568.3 571 571 575 575 581 581 584.8 584.8 596 596 603 603 611 611 629 629 656 656 661 661 680 680 682 682 722 722 732 732 771.5 771.5 805 805];
X1 = X - 527;
Y1 = [1 1  0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 nan];


%% Plot 

% Option 1 - Plots the LOS data and the power data on top of eachother 
figure(100); clf;
yyaxis left;

    % Plots the Power Received
plotPower = averagedData(122:end);  % Ignores the first part cause it is weird
plotPower = plotPower - max(plotPower); % Normalizes the power 
t = (0:length(plotPower)-1)*0.1; % seconds
plot(t,plotPower);
ax1 = gca;

    % Makes it pretty for the paper
ax1.FontName = 'Times New Roman';
ax1.XTick = 0:30:length(plotPower)-1;
ax1.Children.Color = 0*[1 1 1]; %0.67*[1 1 1];
ax1.YLabel.String = 'Received Power (dB)';
ax1.YColor = 'k';

    % Plots the LOS 
yyaxis right;
plotX = (X1*10-349)*0.1;
plotY = 0.1*Y1 - 0.2;
YES = 0.7;
plot(plotX, YES*Y1);
ax2 = gca;

    % More pretty-ing
ax2.FontName = 'Times New Roman';
ax2.YLabel.String = 'Line of Sight';
ax2.YColor = 'k';
ax2.Children.Color = 0.5*[1 1 1];
ax2.XLim = [0 240];
ax2.Children.LineWidth = 0.5;
ax2.Children.LineStyle = '-';

ax2.YTickLabel = {'N' 'Y'};
% ax2.YTick = [-0.2 -0.1];
% ax2.YLim = [-0.25 1];
ax2.YTick = [0 YES];
ax2.YLim = [-0.1 1.1*YES];
ax2.XLabel.String = 'time (s)';

f = gcf;
homer = f.Units;
f.Units = 'inches';
bart = f.Position;
f.Position = [bart(1:2) 10 4];
f.PaperPositionMode = 'auto';
f.Units = homer;
% saveas(gcf,'Figures/PowerReceived','epsc');


    % Option 2 - Plots the LOS underneath the power received data
figure(101);

plotY = 5*Y1 - 41;
plot(t,plotPower,plotX,plotY);
ax3 = gca;

ax3.FontName = 'Times New Roman';
ax3.XTick = 0:30:length(plotPower)-1;
% ax3.Children.Color = 0*[1 1 1]; %0.67*[1 1 1];
ax3.YLabel.String = 'Received Power (dB)';
ax3.YColor = 'k';
ax3.XLim = [0 240];
ax3.Children(1).Color = 'k';
ax3.Children(2).Color = 'k';
ax3.YLim = [-45 0];
ax3.XLabel.String = 'time (s)';

f = gcf;
homer = f.Units;
f.Units = 'inches';
bart = f.Position;
f.Position = [bart(1:2) 5 4];
f.PaperPositionMode = 'auto';
f.Units = homer;



