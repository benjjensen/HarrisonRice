%%%%%% CCDF %%%%%%%%
clear; close all;

load('Data/powerPerMilliSecondArray.mat');
temp = powerPerMilliSecondArray(1,1.266e4:end); % Cuts off the beginning b/c it is weird
temp = temp / max(temp); % Normalized to max 

% figure()
% plot(10*log10(temp))
% title('Power Array')

    % Generates a Histogram 
figure()
y = histogram(10*log10(temp));
title('Histogram')

    % Averages bins
[~, edgeVals] = size(y.BinEdges);
for i = 1:edgeVals-1
    xaxis(i) = mean(y.BinEdges(i:i+1));
end

yp = y.Values / length(temp);
   
    % Accumulates the values 
cumulative = cumsum(yp);

figure()
semilogy(xaxis(1:end-1), 1-cumulative(1:end-1)); grid on;
grid on

 % Formats the axes
ax = gca;
ax.FontName = 'Times New Roman';
ax.XLabel.String = 'Power Received (dB)';
ax.YLabel.String = 'CCDF of Power Received';
ax.Children.Color = 'k';
ax.Children.LineWidth = 1; 


 % Formats the figure before saving
fig = gcf;
tempUnits = fig.Units;
fig.Units = 'inches';
tempPosition = fig.Position;
fig.Position = [tempPosition(1:2) 5 4];   % 5" x 4" - MDR's preferred sizing
fig.PaperPositionMode = 'auto';
fig.Units = tempUnits;

% saveas(gcf,'Figures/CCDF','epsc');

